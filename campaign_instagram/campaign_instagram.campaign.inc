<?php

/**
 * Implements hook_campaign_config_info().
 */
function campaign_instagram_campaign_config_info() {
  return array(
    'instagram' => array(
      'client_id' => NULL,
      'tags' => array(),
    ),
  );
}

/**
 * Implements hook_campaign_edit_form_alter().
 */
function campaign_instagram_campaign_edit_form_alter(&$form, &$form_state, $namespace, $campaign) {
  // Figure out how many hashtag fields should be generated.
  if (empty($form_state['num_instagram_counters'])) {
    // Add an extra field below existing hashtags.
    if (!empty($campaign['instagram']['tags'])) {
      $form_state['num_instagram_counters'] = count($campaign['instagram']['tags']) + 1;
    }
    else {
      $form_state['num_instagram_counters'] = 1;
    }
  }
  $instagram_app_link = l(t('Instagram app'), 'http://instagram.com/developer/', array('external' => TRUE));

  $form['campaign']['instagram'] = array(
    '#type' => 'fieldset',
    '#title' => t('Instagram'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#access' => user_access('administer campaigns'),
    '#weight' => 55,
    '#group' => 'campaign',
  );
  $form['campaign']['instagram']['help'] = array(
    '#markup' => '<p>' . t('Count the media mentions on instagram with a specified tag.') . '</p>'
      . '<p>' . t('To retrieve the amount of images there is a JSON endpoint as well as a JavaScript function which can be used. If the campaign is inactivated, the endpoint will be closed as well.') . '</p>',
  );

  $form['campaign']['instagram']['client_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#description' => t('The Client ID for the !link.', array('!link' => $instagram_app_link)),
    '#maxlength' => 255,
    '#default_value' => $campaign['instagram']['client_id'] ? $campaign['instagram']['client_id'] : variable_get('campaign_instagram_client_id', ''),
  );
  $form['campaign']['instagram']['hashtag_help'] = array(
    '#markup' => '<h3>' . t('Tags') . '</h3>'
      . '<p>' . t('Add each tag you want the campaign to track. These will not be stored but rather used for querying the total count of images mentioning a tag using the Instagram API.') . '</p>',
  );

  for ($i = 1; $i <= $form_state['num_instagram_counters']; $i++) {
    $idx = $i - 1;
    $form['campaign']['instagram']['tags'][$idx]['tag'] = array(
      '#type' => 'textfield',
      '#title' => t('Tag'),
      '#description' => t('Eg. %tag', array('%tag' => '#mushjouluiloa2013')),
      '#maxlength' => 255,
      '#default_value' => isset($campaign['instagram']['tags'][$idx]['tag']) ? '#' . $campaign['instagram']['tags'][$idx]['tag'] : '',
    );
  }

  $form['campaign']['instagram']['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('Add another tag to track'),
    '#submit' => array('campaign_instagram_add_one'),
    '#ajax' => array(
      'callback' => 'campaign_add_more_callback',
      'wrapper' => 'campaign-wrapper',
    ),
    '#prefix' => '<div class="campaign-edit-form-add_more">',
    '#suffix' => '</div>',
  );
}

/**
 * Implements hook_campaign_summary_alter().
 */
function campaign_instagram_campaign_summary_alter(&$summary, $context) {
  $campaign = campaign_get_from_context($context);
  $tags = array();
  if (!isset($campaign['instagram'])) {
    return;
  }
  foreach ($campaign['instagram']['tags'] as $tag) {
    $tags[] = $tag['tag'];
  }
  $active = t('!tags', array(
    '!tags' => implode('<br />', $tags),
  ));
  $summary[] = array(t('Instagram Counter'), $active);
}

/**
 * Implements hook_campaign_save_alter().
 */
function campaign_instagram_campaign_save_alter($form, $form_state, &$context) {
  $campaign = &$context->reactions['campaign_context_reaction']['campaign'];
  // Remove submit buttons value so that it's not save in the context.
  unset($campaign['instagram']['add_more']);
  // Store tags without the # symbol.
  foreach ($campaign['instagram']['tags'] as $idx => $tag) {
    $normalized_tag = trim(preg_replace('/^\#/', '', $tag['tag']));
    $campaign['instagram']['tags'][$idx]['tag'] = $normalized_tag;
    if (empty($normalized_tag)) {
      unset($campaign['instagram']['tags'][$idx]);
    }
  }
}
