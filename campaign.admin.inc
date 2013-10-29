<?php
/**
 * @file
 * Admin settings page for Campaign.
 */

/**
 * Output an overview table of all campaigns.
 */
function campaign_overview() {
  $contexts = context_enabled_contexts(TRUE);
  $rows = array();

  foreach ($contexts as $name => $context) {
    // Skip unless campaign context
    if (!(isset($context->reactions['campaign_context_reaction']['campaign_admin']) && $context->reactions['campaign_context_reaction']['campaign_admin'])) {
      continue;
    }
    // Get the configurations and output them as a summary table.
    $configs = $context->reactions['campaign_context_reaction']['campaign'];
    $fb_data = campaign_get_graph_app($configs['fb']['appid']);

    $details = '<div class="campaign-config-label collapsed"><a href="#" class="toggle-details">' . check_plain($context->name) . '</a></div>';
    $details .= '<div class="campaign-config-details">' . campaign_summary($context) . '</div>';

    $row['data']['icon'] = '<img src="' . $fb_data->logo_url . '" />';
    $row['data']['name'] = $details;

    $operations['edit'] = array(
      'title' => t('Edit'),
      'href' => 'admin/config/campaigns/' . $context->name,
    );
    $operations['rename'] = array(
      'title' => t('Rename'),
      'href' => 'admin/config/campaigns/' . $context->name . '/rename',
    );
    $operations['delete'] = array(
      'title' => t('Delete'),
      'href' => 'admin/config/campaigns/' . $context->name . '/delete',
    );

    $facebook['inspect'] = array(
      'title' => t('Developers'),
      'href' => 'https://developers.facebook.com/apps/' . $configs['fb']['appid'] . '/summary/',
      'attributes' => array(
        'target' => '_blank'
      ),
    );
    $facebook['pagetab'] = array(
      'title' => t('Add tab'),
      'href' => 'http://www.facebook.com/dialog/pagetab?app_id=' . $configs['fb']['appid'] . '&redirect_uri=' . url('admin/config/campaigns', array('absolute' => TRUE)),
      'attributes' => array(
        'target' => '_blank'
      ),
    );

    $row['data']['facebook'] = array(
      'data' => array(
        '#theme' => 'links',
        '#links' => $facebook,
        '#attributes' => array('class' => array('links', 'inline')),
      ),
    );
    $row['data']['operations'] = array(
      'data' => array(
        '#theme' => 'links',
        '#links' => $operations,
        '#attributes' => array('class' => array('links', 'inline')),
      ),
    );
    $rows[] = $row;
  }
  $build['overview_table'] = array(
    '#theme' => 'table',
    '#header' => array(
      'icon' => t('Icon'),
      'name' => t('Name'),
      'facebook' => t('Facebook'),
      'operations' => t('Operations'),
    ),
    '#attributes' => array(
      'class' => array('campaign-config-overview'),
    ),
    '#rows' => $rows,
    '#attached' => array(
      'js' => array(
        drupal_get_path('module', 'campaign') . '/campaign.admin.js',
      ),
      'css' => array(
        drupal_get_path('module', 'campaign') . '/campaign.admin.css',
      ),
    ),
  );

  return $build;
}

/**
 * Output a table summarizing e campaign context.
 */
function campaign_summary($context) {
  $configs = $context->reactions['campaign_context_reaction']['campaign'];
  $languages = language_list();
  $langcode = $configs['config']['langcode'];
  $language = $languages[$langcode];

  $canvas_page = 'http://apps.facebook.com/' . $context->name;
  $gateway = campaign_get_gateway_path($context->name, $language);
  $paths = isset($context->conditions['path']) ? htmlspecialchars(implode(', ', $context->conditions['path']['values'])) : t('No path conditions');
  $app_id = $configs['fb']['appid'];
  $redirect_url = url('fb/' . $context->name, array('absolute' => TRUE, 'language' => $language));

  $summary = array(
    array('Path', $paths),
    array('Language', $language->name),
    array('Application ID', $app_id),
    array('Application Secret', isset($configs['fb']['secret']) ? $configs['fb']['secret'] : t('Not configured')),
    array('Facebook Tab URL', isset($configs['fb']['tab']) ? l($configs['fb']['tab'], $configs['fb']['tab']) : t('Not configured')),
    array('Canvas Page', l($canvas_page, $canvas_page)),
    array('Redirect URL', l($redirect_url, $redirect_url)),
  );

  if (!empty($configs['config']['prelike'])) {
    $prelike = $configs['config']['prelike'];
    $edit = l(t('edit'), drupal_get_normal_path($prelike) . '/edit');
    $summary[] = array('Before like', l($prelike, $prelike, array('language' => $language)) . ' (' . $edit . ')');
  }
  if (!empty($configs['config']['postlike'])) {
    $postlike = $configs['config']['postlike'];
    $edit = l(t('edit'), drupal_get_normal_path($postlike) . '/edit');
    $summary[] = array('After like', l($postlike, $postlike, array('language' => $language)) . ' (' . $edit . ')');
  }

  $summary[] = array('App / Canvas', l($gateway, $gateway, array('language' => $language)));
  $summary[] = array('Page Tab', l($gateway, $gateway, array('language' => $language)));
  if (isset($postlike)) {
    // Path needs to be suffixed with an '/' to be accepted on Facebook.
    $postlike = url($postlike, array('absolute' => TRUE, 'language' => $language)) . '/';
    $summary[] = array('Mobile Web', l($postlike, $postlike));
  }
  else {
    $summary[] = array('Mobile Web', t('Missing after like'));
  }

  $rules = _campaign_ui_get_rules_by_tag($context->name);
  if (!empty($rules)) {
    $summary[] = array('Related rules', implode(', ', $rules));
  }

  $views = _campaign_ui_get_views_by_tag($context->name);
  if (!empty($views)) {
    $summary[] = array('Related views', implode(', ', $views));
  }

  drupal_alter('campaign_summary', $summary, $context);
  return theme('table', array(
    'rows' => $summary,
    'attributes' => array('class' => array('campaign-config-summary')),
  ));
}

/**
 * Return an array of links to rules tagged with tag name.
 */
function _campaign_ui_get_rules_by_tag($name) {
  $entities = entity_load('rules_config', FALSE, array('tags' => array($name)));
  $rules = array();
  foreach ($entities as $id => $entity) {
    $rules[] = l($entity->label, RulesPluginUI::path($id));
  }
  return $rules;
}

/**
 * Return an array of links to views tagged with tag name.
 */
function _campaign_ui_get_views_by_tag($name) {
  $views = array();
  foreach (views_get_all_views() as $view) {
    if (!empty($view->tag)) {
      $tags = drupal_explode_tags($view->tag);
      if (in_array($name, $tags)) {
        $views[] = l($view->get_human_name(), 'admin/structure/views/view/' . $view->name . '/edit');
      }
    }
  }
  return $views;
}

/**
 * Output a form to provide the machine name of a campaign. This is the first
 * step.
 */
function campaign_context_add_form($form, &$form_state) {
  // Include the name as a hidden value, this will be filled out
  // programmatically by fetching the namespace from Facebook Graph API.
  $form['name'] = array(
    '#title' => 'Name',
    '#type' => 'hidden',
    '#default_value' => '',
  );

  $form['campaign']['fb']['appid'] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook Application ID'),
    '#description' => t('The application ID this campaign will use.'),
    '#required' => 1,
    '#maxlength' => 15,
    '#default_value' => '',
    '#element_validate' => array('campaign_context_app_validate'),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/campaigns',
  );

  return $form;
}

/**
 * Validate a namespace. Returns TRUE if valid, otherwise an error message is
 * returned.
 */
function _campaign_validate_namespace($namespace) {
  // Make sure the namespace is valid.
  if (!preg_match('/^[a-z0-9_-]+$/', $namespace)) {
    return t('The machine name retrieved from the facebook application can only consist of lowercase letters, underscores, dashes, and numbers.');
  }

  // Make sure the context doesn't already exist.
  ctools_include('export');
  if (ctools_export_crud_load('context', $data->namespace)) {
    // Clear the cache.
    cache_clear_all('campaign_graph:' . $element['#value'], 'cache');
    return t('A context with this same name as the facebook namespace already exists. Please rename it on facebook and try again.');
  }

  return TRUE;
}

function campaign_context_name_validate($element, &$form_state) {
  if (($message = _campaign_validate_namespace($element['#value'])) !== TRUE) {
    form_error($element, $message);
  }
}

function campaign_context_app_validate($element, &$form_state) {
  // Validate the Application ID.
  if (!preg_match('/^[0-9]+$/', $element['#value'])) {
    form_error($element, t('The Application ID can only contain numbers.'));
    return;
  }

  // Check Graph API errors.
  $data = campaign_get_graph_app($element['#value'], TRUE);
  if (isset($data->error)) {
    if ($data->error->message == 'Unsupported get request.') {
      form_error($element, t('Unsupported get request. Make sure you disabled sandbox-mode.'));
    }
    else {
      form_error($element, $data->error->message);
    }
    return;
  }

  // Make sure the generated namespace is valid.
  if (($message = _campaign_validate_namespace($data->namespace)) !== TRUE) {
    form_error($element, $message);
  }

  // Set the machine name according to the facebook application namespace.
  $form_state['values']['name'] = $data->namespace;
}

/**
 * Create the initial empty campaign context and redirect to the configuration
 * page.
 */
function campaign_context_add_form_submit($form, &$form_state) {
  $context = campaign_load_default_context();
  $context->name = $form_state['values']['name'];
  // Preset the app id.
  $context->reactions['campaign_context_reaction']['campaign']['fb']['appid'] = $form_state['values']['appid'];
  context_save($context);
  $form_state['redirect'] = 'admin/config/campaigns/' . $context->name;
}

/**
 * Output a configuration form to edit campaign contexts.
 */
function campaign_context_edit_form($form, &$form_state, $context) {
  $form_state['campaign']['context'] = $context;
  // $form = array();

  campaign_campaign_edit_form($form, $form_state, $context->name, $context->reactions['campaign_context_reaction']['campaign']);

  $form['path'] = array(
    '#title' => t('Path'),
    '#description' => t('Set this metatag context when any of the paths above match the page path. Put each path on a separate line. You can use the <code>*</code> character (asterisk) as a wildcard and the <code>~</code> character (tilde) to exclude one or more paths. Use <code>&lt;front&gt;</code> for the site front page. Only local paths (e.g. "example/page") will work, do not use relative URLs ("/example/page") or absolute URLs ("http://example.com/example/page").'),
    '#type' => 'textarea',
    '#default_value' => isset($context->conditions['path']['values']) ? html_entity_decode(implode('&#13;&#10;', $context->conditions['path']['values'])) : '',
    '#required' => 1,
    '#weight' => -100,
  );

  $form['name'] = array(
    '#title' => t('Namespace / Context ID'),
    '#description' => t('This machine name has to be identical to the Facebook app namespace. !edit', array(
      '!edit' => l(t('Edit'), 'admin/config/campaigns/' . $context->name . '/rename'),
    )),
    '#type' => 'textfield',
    '#default_value' => $context->name,
    '#weight' => -101,
    '#disabled' => TRUE,
  );

  // If other conditions are assigned, mention it.
  $conditions = array_keys($context->conditions);
  foreach ($conditions as $key => $condition) {
    if ($condition == 'path') {
      unset($conditions[$key]);
    }
  }
  if (!empty($conditions)) {
    $form['other_conditions'] = array(
      '#prefix' => '<p><em>',
      '#markup' => t('Other conditions have been assigned that must be controlled through the main Context settings page.'),
      '#suffix' => '</em></p>',
      '#weight' => -99.9,
    );
  }

  $form['summary'] = array(
    '#type' => 'fieldset',
    '#title' => t('Summary'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#weight' => 99,
  );
  $form['summary']['table'] = array(
    '#markup' => campaign_summary($context),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/campaigns',
  );
  $form['#submit'][] = 'campaign_context_edit_form_submit';

  return $form;
}

/**
 * Save the context from editing it in within the campaign UI.
 */
function campaign_context_edit_form_submit($form, &$form_state) {
  $context = $form_state['campaign']['context'];

  // Store assets as arrays
  $form_state['values']['campaign']['assets']['css'] = explode("\n", str_replace("\r", "", $form_state['values']['campaign']['assets']['css']));
  $form_state['values']['campaign']['assets']['js'] = explode("\n", str_replace("\r", "", $form_state['values']['campaign']['assets']['js']));

  $context->reactions['campaign_context_reaction']['campaign'] = $form_state['values']['campaign'];
  $paths = explode("\n", str_replace("\r", "", $form_state['values']['path']));
  $paths = array_combine($paths, $paths);
  $context->conditions['path']['values'] = $paths;

  $form_state['redirect'] = 'admin/config/campaigns';

  drupal_alter('campaign_save', $form, $form_state, $context);
  context_save($context);
}

function campaign_context_delete_form($form, &$form_state, $context) {
  $form_state['campaign']['context'] = $context;
  $form['delete'] = array(
    '#value' => 'This action will permanently remove this item from your database.',
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/campaigns',
  );
  return $form;
}

function campaign_context_delete_form_submit($form, &$form_state) {
  context_delete($form_state['campaign']['context']);
  $form_state['redirect'] = 'admin/config/campaigns';
}

function campaign_context_rename_form($form, &$form_state, $context) {
  $form_state['campaign']['context'] = $context;

  $form['original_name'] = array(
    '#type' => 'hidden',
    '#default_value' => $context->name,
  );
  $form['name'] = array(
    '#title' => t('Namespace / Context ID'),
    '#description' => t('This machine name has to be identical to the Facebook app namespace.'),
    '#type' => 'textfield',
    '#default_value' => $context->name,
    '#weight' => -101,
    '#element_validate' => array('campaign_context_name_validate'),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Rename'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/campaigns',
  );
  return $form;
}
function campaign_context_rename_form_submit($form, &$form_state) {
  $new = clone($form_state['campaign']['context']);
  $new->name = $form_state['values']['name'];
  context_save($new);
  context_delete($form_state['campaign']['context']);
  $form_state['redirect'] = 'admin/config/campaigns';
}

function campaign_load_default_context() {
  $context = new stdClass();
  $context->disabled = FALSE;
  $context->api_version = 3;
  $context->name = 'default_campaign_context';
  $context->description = '';
  $context->tag = 'Campaign';
  $context->campaign = true;
  $context->conditions = array(
    'path' => array(
      'values' => array(),
    ),
  );
  $context->reactions = array(
    'campaign_context_reaction' => array(
      'campaign' => array(),
      'campaign_admin' => 1,
    ),
  );
  $context->condition_mode = 0;
  $context->weight = 0;

  return $context;
}
