<?php

/**
 * A link-friendly page which redirects to the gateway, in practice this will
 * trigger step 1, and render the pre-like page with a JavaScript redirect to
 * the Canvas page.
 * This way Google gets og:data, mobile uers are redirected to post-like and
 * desktop users are redirected to the page tab.
 */
function campaign_fb_redirect($context) {
  drupal_goto(campaign_get_gateway_path($context->name));
}

/**
 * Act as a gateway redirecting the user to the correct page depending on the
 * signed request posted by facebook.
 *
 * 1. User arrives via direct link
 * 1.1 Desktop: Renders pre-like and redirects to canvas page with JavaScript.
 * This way Google gets the og:data.
 * 1.2. Mobile: Redirect directly to after-like (@see http://goo.gl/Hmch8).
 * 2. User arrives via Canvas page (eg. apps.facebook.com/ID), renders pre-like
 * and redirects to page tab with JavaScript.
 * 3. User arrives at page tab and has liked the page, renders post-like page.
 * 4. User arrives at page tab and has NOT liked the page, renders pre-like
 * page.
 * 5. A mobile user will be redirected to post-like through facebooks redirect
 * logic on the canvas page.
 */
function campaign_gateway($namespace, $config) {
  // Bootstrap DRUPAL_BOOTSTRAP_LANGUAGE
  drupal_language_initialize();
  // libraries_get_path() requires drupal_get_path which is loaded in DRUPAL_BOOTSTRAP_FULL.
  require_once DRUPAL_ROOT . '/includes/common.inc';
  // Using url() requires path.inc which is loaded in DRUPAL_BOOTSTRAP_FULL.
  require_once DRUPAL_ROOT . '/' . variable_get('path_inc', 'includes/path.inc');
  require_once DRUPAL_ROOT . '/sites/all/libraries/Mobile-Detect/Mobile_Detect.php';

  $query = NULL;
  $data = campaign_get_signed_request($config['fb']['appid'], $config['fb']['secret']);
  $detect = new Mobile_Detect();
  // (1) The visitor is coming from a direct link, redirect to pre-like and canvas page.
  if (empty($data)) {
    // (1.1) Desktop, redirect to apps.facebook.com with javascript
    if (!($detect->isMobile() || $detect->isTablet())) {
      $data['page']['liked'] = 0;
      $query = array('js_redirect' => url('//apps.facebook.com/' . $namespace, array('external' => TRUE)));
    }
    // (1.2) Mobile, redirect to after-like.
    else {
      $data['page']['liked'] = 1;
    }
  }
  // (2) The visitor is on the canvas page, redirect to pre-like and page tab.
  // We do this with JavaScript so we can use window.top and redirect not only
  // the iframe.
  elseif (empty($data['page'])) {
    $data['page']['liked'] = 0;
    $query = array('js_redirect' => $config['fb']['tab']);
  }

  // IE8.0 renders a blank screen while on Facebook, for now we redirect IE8
  // users to the website.
  // @FIXME
  if ($detect->version('MSIE') == '8.0') {
    $query = array('js_redirect' => url($config['config']['postlike'], array('external' => TRUE)));
  }

  // @TODO for some reason, url() does not prepend the language prefix.
  global $language;
  $prefix = !empty($language->prefix) ? $language->prefix . '/' : '';

  // In case the campaign is inactive and an inactive page is set, redirect to it.
  // @TODO can we move this up in the flow?
  if (isset($config['active']) && $config['active'] === FALSE && !empty($config['config']['inactive'])) {
    header('Location: ' . url($config['config']['inactive'], array('absolute' => TRUE, 'query' => $query, 'prefix' => $prefix)));
  }
  // (3) If the user has liked the page, redirect to the configured page.
  elseif ($data['page']['liked']) {
    header('Location: ' . url($config['config']['postlike'], array('absolute' => TRUE, 'query' => $query, 'prefix' => $prefix)));
  }
  // (4) If not redirect to the pre-liked page.
  else {
    header('Location: ' . url($config['config']['prelike'], array('absolute' => TRUE, 'query' => $query, 'prefix' => $prefix)));
  }
}
