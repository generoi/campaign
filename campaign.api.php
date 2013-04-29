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
 * @see campaign_summary()
 */
function hook_campaign_summary_alter(&$rows, $context) {
  $rows[] = array('Custom attribute', $context->reactions['campaign_context_reaction']['campaign']['custom_attribute']);
}

/**
 * Alter the data which gets stored in the graph cache.
 *
 * For example you could do more http requests, from eg. twitter.
 *
 * @see campaign_get_graph_app()
 */
function hook_campaign_graph_data($app_id, &$response) {
  if (isset($response->data) && empty($response->data->error)) {
    $response->data->custom_attribute = 'Custom attribute';
  }
}
