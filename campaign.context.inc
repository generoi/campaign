<?php

class campaign_context_reaction extends context_reaction {

  function options_form($context) {
    $form = array();
    // Load the previously saved settings.
    $data = $this->fetch_from_context($context);
    if (!isset($data['campaign'])) {
      $data['campaign'] = array();
    }
    campaign_campaign_form($form, $context->name, $data['campaign']);

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
        if (!empty($context->reactions['campaign_context_reaction']['campaign'])) {
          // Allows multiple contexts to create a merged object. This should
          // be used with care.
          $output = drupal_array_merge_deep($output, $context->reactions['campaign_context_reaction']['campaign']);
          $instance_names[] = $context->name;
        }
      }

      if (!empty($output)) {
        $instance = 'context:' . implode(',', $instance_names);
        // Allow altering
        drupal_alter('campaign_campaign_view', $output, $instance);
      }
    }
  }
}
