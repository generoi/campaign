<?php

/**
 * Implements hook_rules_action_info().
 */
function campaign_rules_action_info() {
  // https://developers.facebook.com/docs/reference/dialogs/feed/
  $actions['campaign_feed_dialog'] = array(
    'label' => t('Display a feed dialog'),
    'group' => t('Campaign'),
    'base' => 'campaign_action_feed_dialog',
    'named parameter' => TRUE,
    'parameter' => array(
      'picture' => array(
        'type' => 'uri',
        'label' => t('Picture'),
        'description' => t('The URL of a picture attached to this post. The picture must be at least 50px by 50px (though minimum 200px by 200px is preferred) and have a maximum aspect ratio of 3:1'),
        'translatable' => TRUE,
        'optional' => TRUE,
      ),
      'link' => array(
        'type' => 'uri',
        'label' => t('Link path'),
        'description' => t('The link attached to this post.'),
        'translatable' => TRUE,
      ),
      'name' => array(
        'type' => 'text',
        'label' => t('Name'),
        'description' => t('The name of the link attachment.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
      'caption' => array(
        'type' => 'text',
        'label' => t('Caption'),
        'description' => t('The caption of the link (appears beneath the link name). If not specified, this field is automatically populated with the URL of the link.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
      'description' => array(
        'type' => 'text',
        'label' => t('Description'),
        'description' => t('The description of the link (appears beneath the link caption). If not specified, this field is automatically populated by information scraped from the link, typically the title of the page.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
      // @TODO actions, properties, ref, source, from, to, display, app_id, redirect_uri
    ),
  );

  // http://developers.facebook.com/docs/reference/dialogs/send/
  $actions['campaign_send_dialog'] = array(
    'label' => t('Display a send dialog'),
    'group' => t('Campaign'),
    'base' => 'campaign_action_send_dialog',
    'named parameter' => TRUE,
    'parameter' => array(
      'picture' => array(
        'type' => 'uri',
        'label' => t('Picture'),
        'description' => t('By default a picture will be taken from the link specified. The URL of a picture to include in the message. The picture will be shown next to the link.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
      'link' => array(
        'type' => 'uri',
        'label' => t('Link path'),
        'description' => t('The link to send in the message.'),
        'translatable' => TRUE,
      ),
      'name' => array(
        'type' => 'text',
        'label' => t('Name'),
        'description' => t('By default a title will be taken from the link specified. The name of the link, i.e. the text to display that the user will click on.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
      'to' => array(
        'type' => 'text',
        'label' => t('To'),
        'description' => t('A user ID or username to which to send the message. Once the dialog comes up, the user can specify additional users, Facebook groups, and email addresses to which to send the message. Sending content to a Facebook group will post it to the group\'s wall.'),
        'optional' => TRUE,
      ),
      'description' => array(
        'type' => 'text',
        'label' => t('Description'),
        'description' => t('By default a description will be taken from the link specified. Descriptive text to show below the link.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
    ),
  );

  // http://developers.facebook.com/docs/reference/dialogs/requests/
  $actions['campaign_requests_dialog'] = array(
    'label' => t('Display a requests dialog'),
    'group' => t('Campaign'),
    'base' => 'campaign_action_requests_dialog',
    'named parameter' => TRUE,
    'parameter' => array(
      'title' => array(
        'type' => 'text',
        'label' => t('Title'),
        'description' => t('Optional, the title for the Dialog. Maximum length is 50 characters.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
      'message' => array(
        'type' => 'text',
        'label' => t('Message'),
        'description' => t('The Request string the receiving user will see. It appears as a request posed by the sending user. The maximum length is 130 characters. The message value is displayed in Notifications and can also be viewed on the App Center Requests.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
      'to' => array(
        'type' => 'text',
        'label' => t('To'),
        'description' => t('A user ID or username. This may or may not be a friend of the user. If this is specified, the user will not have a choice of recipients. If this is omitted, the user will see a Multi Friend Selector and will be able to select a maximum of 50 recipients.'),
        'optional' => TRUE,
      ),
      'max_recipients' => array(
        'type' => 'integer',
        'label' => t('Max recipients'),
        'description' => t('An integer that specifies the maximum number of friends that can be chosen by the user in the friend selector. This parameter is not supported on mobile devices.'),
        'optional' => TRUE,
      ),
      'data' => array(
        'type' => 'text',
        'label' => t('Data'),
        'description' => t('Optional, additional data you may pass for tracking. This will be stored as part of the request objects created. The maximum length is 255 characters.'),
        'optional' => TRUE,
        'translatable' => TRUE,
      ),
    ),
  );
  return $actions;
}

/**
 * Implements hook_rules_condition_info().
 */
function campaign_rules_condition_info() {
  $conditions['campaign_webform_field_comparison'] = array(
    'label' => t('Webform question comparison'),
    'parameter' => array(
      'data' => array(
        'type' => '*',
        'label' => t('Data to compare'),
        'description' => t('The data to compare, you should select the "data" choice 99% of the time.'),
        'allow null' => TRUE,
      ),
      'question' => array(
        'label' => t('Question field key'),
        'help' => t('Enter the machine readable field key of the question component. The raw value (i.e. the option key instead of the option value) will be used for comparisons.'),
        'type' => 'text',
      ),
      'solution' => array(
        'label' => t('Solution field key'),
        'help' => t('Enter the machine readable field key of the solution component. The raw value will be used for comparisons.'),
        'type' => 'text',
      ),
    ),
    'group' => t('Webform'),
  );
  return $conditions;
}

/**
 * Implements hook_webform_field_comparison().
 *
 * Compare webform component values.
 */
function campaign_webform_field_comparison($data, $question_key, $solution_key) {
  // Make sure the components exist.
  if (!isset($data['components'][$question_key])) {
    watchdog('campaign', 'Webform component %comp does not exist', array('%comp' => $question_key));
    return FALSE;
  }
  if (!isset($data['components'][$solution_key])) {
    watchdog('campaign', 'Webform component %comp does not exist', array('%comp' => $solution_key));
    return FALSE;
  }
  $question = intval($data['components'][$question_key]['value'][0]);
  $solution = intval($data['components'][$solution_key]['value'][0]);

  return $question === $solution;
}

/**
 * Display a feed dialog.
 */
function campaign_action_feed_dialog($parameters) {
  $json = array('method' => 'feed');
  foreach (array('picture', 'link', 'name', 'caption', 'description') as $key) {
    if (!empty($parameters[$key])) {
      $json[$key] = $parameters[$key];
    }
  }
  campaign_add_dialog($json);
}

/**
 * Display a send dialog.
 */
function campaign_action_send_dialog($parameters) {
  $json = array('method' => 'send');
  foreach (array('picture', 'link', 'name', 'to', 'description') as $key) {
    if (!empty($parameters[$key])) {
      $json[$key] = $parameters[$key];
    }
  }
  campaign_add_dialog($json);
}

/**
 * Display a app requests dialog.
 */
function campaign_action_requests_dialog($parameters) {
  $json = array('method' => 'apprequests');
  foreach (array('message', 'to', 'max_recipients', 'data', 'title') as $key) {
    if (!empty($parameters[$key])) {
      $json[$key] = $parameters[$key];
    }
  }
  campaign_add_dialog($json);
}
