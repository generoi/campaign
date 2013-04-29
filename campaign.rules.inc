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
        'optional' => TRUE,
      ),
      'link' => array(
        'type' => 'uri',
        'label' => t('Link path'),
        'description' => t('The link attached to this post.'),
      ),
      'name' => array(
        'type' => 'text',
        'label' => t('Name'),
        'description' => t('The name of the link attachment.'),
        'optional' => TRUE,
      ),
      'caption' => array(
        'type' => 'text',
        'label' => t('Caption'),
        'description' => t('The caption of the link (appears beneath the link name). If not specified, this field is automatically populated with the URL of the link.'),
        'optional' => TRUE,
      ),
      'description' => array(
        'type' => 'text',
        'label' => t('Description'),
        'description' => t('The description of the link (appears beneath the link caption). If not specified, this field is automatically populated by information scraped from the link, typically the title of the page.'),
        'optional' => TRUE,
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
      ),
      'link' => array(
        'type' => 'uri',
        'label' => t('Link path'),
        'description' => t('The link to send in the message.'),
      ),
      'name' => array(
        'type' => 'text',
        'label' => t('Name'),
        'description' => t('By default a title will be taken from the link specified. The name of the link, i.e. the text to display that the user will click on.'),
        'optional' => TRUE,
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
      ),
      'message' => array(
        'type' => 'text',
        'label' => t('Message'),
        'description' => t('The Request string the receiving user will see. It appears as a request posed by the sending user. The maximum length is 130 characters. The message value is displayed in Notifications and can also be viewed on the App Center Requests.'),
        'optional' => TRUE,
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
      ),
    ),
  );
  return $actions;
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
