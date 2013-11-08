<?php

/**
 * A link-friendly page which redirects to the gateway.
 */
function campaign_fb_redirect($context) {
  drupal_goto(campaign_get_gateway_path($context->name));
}

/**
 * Act as a gateway redirecting the user to the correct page depending on the
 * signed request posted by facebook.
 *
 * (1) Render the post like page for crawlers.
 * (2) Redirect mobile visitors to post like page.
 * (3) Redirect visitors arriving by a direct link to the page tab.
 * (4) In the unlikely case of visitors arriving to the canvas page, redirect
 *     them with JS to the page tab.
 * (5) If the visitor is in the page tab and has not liked, redirect to the
 *     pre like page.
 * (6) If the visitor is on the page tab and has liked, redirect to the
 *     post like page.
 *
 * - If using IE8, prevent render bug by redirecting to post like.
 * - If campaign has ended, redirect mobile/liked/not-liked to the inactive
 *   page.
 */
function campaign_gateway($namespace, $config) {
  // So far Drupal is in the middle of DRUPAL_BOOTSTRAP_PAGE_HEADER
  // Bootstrap DRUPAL_BOOTSTRAP_LANGUAGE without sending out headers in
  // DRUPAL_BOOTSTRAP_PAGE_HEADER.
  drupal_language_initialize().
  // libraries_get_path() in campaign_get_signed_request() requires
  // common.inc is loaded during DRUPAL_BOOTSTRAP_FULL which we are not
  // doing. However we need:
  // - libraries_get_path() for campaign_get_signed_request().
  // - drupal_get_query_parameters()
  require_once DRUPAL_ROOT . '/includes/common.inc';
  // Using url() requires path.inc which is loaded in DRUPAL_BOOTSTRAP_FULL.
  require_once DRUPAL_ROOT . '/sites/all/libraries/Mobile-Detect/Mobile_Detect.php';

  $state = 0;
  $query = drupal_get_query_parameters();
  $data = campaign_get_signed_request($config['fb']['appid'], $config['fb']['secret']);
  $is_inactive = isset($config['active']) && $config['active'] === FALSE && !empty($config['config']['inactive']);
  $detect = new Mobile_Detect();
  // @TODO for some reason, url() does not prepend the language prefix.
  global $language;
  $prefix = !empty($language->prefix) ? $language->prefix . '/' : '';

  // (1) Crawlers
  if (preg_match('/googlebot|bingbot|facebookexternalhit/i', $_SERVER['HTTP_USER_AGENT'])) {
    $state = 'crawler';
  }
  // (2) Mobile users
  elseif ($detect->isMobile() || $detect->isTablet()) {
    $state = 'mobile';
  }
  // (3) User arrived from a direct link.
  elseif (empty($data)) {
    $state = 'direct';
  }
  // (4) User somehow arrived on canvas page.
  elseif (empty($data['page'])) {
    $state = 'canvas';
  }
  // (5) User is on page tab and has liked
  elseif ($data['page']['liked']) {
    $state = 'liked';
  }
  // (6) User is on page tab and has not liked
  else {
    $state = 'not-liked';
  }

  // @FIXME IE8.0 renders a blank screen while on Facebook, for now we redirect IE8
  // users to the website.
  if ($detect->version('MSIE') == '8.0') {
    $state = 'liked';
  }
  // Inactive campaign page only applies to real users.
  if ($is_inactive && in_array($state, array('mobile', 'liked', 'not-liked'))) {
    $state = 'inactive';
  }

  switch ($state) {
    // For crawlers we try to render the postlike page.
    // This will reinvoke a few hook_boots() but that is very unlikely
    // to cause problems.
    case 'crawler':
      bootstrap_invoke_all('boot');
      _drupal_bootstrap_full();
      // Set the q parameter correctly so current_path() is set to postlike,
      // otherwise metatag wont get the data as no node exists.
      // However the canonical link and the og:url has to be specified manually
      // in the UI, otherwise it wil be set to the postlike page, and
      // resolved by crawlers.
      // @see _metatag_entity_is_page().
      $_GET['q'] = $config['config']['postlike'];
      menu_execute_active_handler($config['config']['postlike']);
      return;
    case 'mobile':
    case 'liked':
      $location = $config['config']['postlike'];
      break;
    // This should never be reached, but if it's reached, redirect to page
    // tab using JavaScript redirection on the pre-like page. We do this
    // to redirect window.top, not only the iframe.
    case 'canvas':
      $query = array('js_redirect' => $config['fb']['tab']);
      // Fall through
    case 'not-liked':
      $location = $config['config']['prelike'];
      break;
    case 'inactive':
      $location = $config['config']['inactive'];
      break;
    case 'direct':
    default:
      // Push current GET parameters to app_data so they can be passed through
      // the Facebook tab.
      $params = drupal_get_query_parameters();
      $query = array('app_data' => urlencode(json_encode($params)));
      $prefix = ''; // External url shouldnt use a prefix.
      $location = $config['fb']['tab'];
      break;
  }

  // Move app_data back into the page by merging it with the query before
  // redirecting.
  if (!empty($data['app_data'])) {
    try {
      $params = json_decode(urldecode($data['app_data']), TRUE);
      $query += (array) $params;
    } catch (Exception $e) {
    }
  }

  // Using url() requires path.inc which is loaded in DRUPAL_BOOTSTRAP_FULL.
  if (!function_exists('drupal_get_path_alias')) {
    require_once DRUPAL_ROOT . '/' . variable_get('path_inc', 'includes/path.inc');
  }

  header('Location: ' . url($location, array(
    'absolute' => TRUE,
    'query' => $query,
    'prefix' => $prefix
  )));
}
