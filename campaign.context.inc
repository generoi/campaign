<?php

class campaign_context_reaction extends context_reaction {

  function options_form($context) {
    $form = array();
    $form_state = array();
    // Load the previously saved settings.
    $data = $this->fetch_from_context($context);
    if (!isset($data['campaign'])) {
      $data['campaign'] = array();
    }
    // @TODO form_state wont work for AJAX add more fields.
    campaign_campaign_edit_form($form, $form_state, $context->name, $data['campaign']);

    // $form['campaign']['tokens']['#token_types'] = 'all';

    $form['campaign_admin'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show on campaign admin page'),
      '#weight' => -98,
      '#default_value' => isset($data['campaign_admin']) ? $data['campaign_admin'] : '',
    );
    return $form;
  }

  function execute() {
    $output = &drupal_static('campaign_context');
    if (!isset($output)) {
      $campaign = array();
      $output = array();
      $contexts = context_active_contexts();

      foreach ($contexts as $context) {
        if (campaign_get_from_context($context)) {
          // Allows multiple contexts to create a merged object. This should
          // be used with care.
          $output = drupal_array_merge_deep($output, campaign_get_from_context($context));
          $instance_names[] = $context->name;
        }
      }

      if (!empty($output) && !empty($instance_names)) {
        campaign_load_defaults($output);
        $output['contexts'] = $instance_names;
        $instance = 'context:' . implode(',', $instance_names);
        // Allow altering
        drupal_alter('campaign_view', $output, $instance);
      }
    }
  }
}
