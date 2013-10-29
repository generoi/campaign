<?php
/**
 * @file
 * API documentation for the Campaign module.
 */

/**
 * Alter the admin summary table rows of a context.
 *
 * In this hook you can modify and add more rows to the summary.
 *
 * @see campaign_summary().
 */
function hook_campaign_summary_alter(&$rows, $context) {
  $rows[] = array('Custom attribute', $context->reactions['campaign_context_reaction']['campaign']['custom_attribute']);
}

/**
 * Alter the campaign edit form.
 *
 * The regular form hook won't edit contexts admin UI page, as this hook will.
 * The $campaign array consists of the forms structure populated with default
 * values.
 *
 * @see campaign_campaign_edit_form().
 */
function hook_campaign_edit_form_alter(&$form, &$form_state, $namespace, $campaign) {
  $form['campaign']['foo'] = array(
    '#type' => 'textfield',
    '#title' => t('Foo'),
    '#maxlength' => 255,
    '#default_value' => $campaign['foo'],
  );
}

/**
 * Alter campaign context before it's saved to the database.
 *
 * By using this hook you stil have access to the form data.
 *
 * @see campaign_context_edit_form_submit().
 */
function hook_campaign_save_alter($form, $form_state, &$context) {
  $context->reactions['campaign_context_reaction']['campaign']['foo'] = 'bar';
}

/**
 * Alter a triggered campaigns data.
 *
 * @see \campaign_context_reaction::options_form().
 */
function hook_campaign_view_alter($output, $instance) {
}


/**
 * Add new campaign config defaults. This is essentially used to skip isset
 * checks for #default_value in forms.
 *
 * @see campaign_campaign_config_info().
 */
function hook_campaign_config_info() {
  return array(
    'foo' => '',
  );
}

/**
 * Alter the config data of campaigns.
 *
 * @see campaign_config_list().
 */
function hook_campaign_config_info_alter(&$config) {
  $config['foo'] = 'default';
}

/**
 * Alter the data which gets stored in the graph cache.
 *
 * For example you could do more http requests, from eg. twitter.
 *
 * @see campaign_get_graph_app()
 */
function hook_campaign_graph_data_alter($app_id, &$response) {
  if (isset($response->data) && empty($response->data->error)) {
    $response->data->custom_attribute = 'Custom attribute';
  }
}
