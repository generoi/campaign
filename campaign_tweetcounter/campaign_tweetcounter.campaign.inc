<?php

/**
 * Implements hook_campaign_config_info().
 */
function campaign_tweetcounter_campaign_config_info() {
  return array(
    'tweetcounter' => array(
      'active' => FALSE,
      'oauth_access_token' => NULL,
      'oauth_access_token_secret' => NULL,
      'consumer_key' => NULL,
      'consumer_secret' => NULL,
      'hashtags' => array(),
    ),
  );
}

/**
 * Implements hook_campaign_edit_form_alter().
 */
function campaign_tweetcounter_campaign_edit_form_alter(&$form, &$form_state, $namespace, $campaign) {
  // Figure out how many hashtag fields should be generated.
  if (empty($form_state['num_tweet_counters'])) {
    // Add an extra field below existing hashtags.
    if (!empty($campaign['tweetcounter']['hashtags'])) {
      $form_state['num_tweet_counters'] = count($campaign['tweetcounter']['hashtags']) + 1;
    }
    else {
      $form_state['num_tweet_counters'] = 1;
    }
  }
  $twitter_app_link = l(t('Twitter app'), 'https://dev.twitter.com/apps', array('external' => TRUE));

  $form['campaign']['tweetcounter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tweet counter'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
    '#access' => user_access('administer campaigns'),
    '#weight' => 55,
    '#group' => 'campaign',
  );
  $form['campaign']['tweetcounter']['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Activate Tweet Counters'),
    '#description' => t('If inactive, tweets will not be synchronized or counted (same if the campaign itself is inactive).'),
    '#default_value' => $campaign['tweetcounter']['active'],
  );
  $form['campaign']['tweetcounter']['help'] = array(
    '#markup' => '<p>' . t('Track and store all tweets mentioning a hashtag as well as keep a counter on the ones mentioning a specified URL.') . '</p>',
  );

  $form['campaign']['tweetcounter']['oauth_access_token'] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth access token'),
    '#description' => t('The OAuth access token for the !link.', array('!link' => $twitter_app_link)),
    '#maxlength' => 255,
    '#default_value' => $campaign['tweetcounter']['oauth_access_token'] ? $campaign['tweetcounter']['oauth_access_token'] : variable_get('campaign_tweetcounter_oauth_access_token', ''),
  );
  $form['campaign']['tweetcounter']['oauth_access_token_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth access token secret'),
    '#description' => t('The OAuth access token secret for the !link.', array('!link' => $twitter_app_link)),
    '#maxlength' => 255,
    '#default_value' => $campaign['tweetcounter']['oauth_access_token_secret'] ? $campaign['tweetcounter']['oauth_access_token_secret'] : variable_get('campaign_tweetcounter_oauth_access_token_secret', ''),
  );
  $form['campaign']['tweetcounter']['consumer_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Consumer key'),
    '#description' => t('Consumer key for the !link.', array('!link' => $twitter_app_link)),
    '#maxlength' => 255,
    '#default_value' => $campaign['tweetcounter']['consumer_key'] ? $campaign['tweetcounter']['consumer_key'] : variable_get('consumer_key', ''),
  );
  $form['campaign']['tweetcounter']['consumer_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('Consumer secret'),
    '#description' => t('Consumer secret for the !link.', array('!link' => $twitter_app_link)),
    '#maxlength' => 255,
    '#default_value' => $campaign['tweetcounter']['consumer_secret'] ? $campaign['tweetcounter']['consumer_secret'] : variable_get('consumer_secret', ''),
  );

  $form['campaign']['tweetcounter']['hashtag_help'] = array(
    '#markup' => '<h3>' . t('Hashtags') . '</h3>'
      . '<p>' . t('Add each hashtag you want the campaign to track. Optionally enter a space separated list of URLs, one of which has to be mentioned to count as a participation. If no URL is specified all tweets will be counted.') . '</p>',
  );

  for ($i = 1; $i <= $form_state['num_tweet_counters']; $i++) {
    $idx = $i - 1;
    $form['campaign']['tweetcounter']['hashtags'][$idx]['hashtag'] = array(
      '#type' => 'textfield',
      '#title' => t('Hashtag'),
      '#description' => t('Eg. %hashtag', array('%hashtag' => '#mushjouluiloa2013')),
      '#maxlength' => 255,
      '#default_value' => isset($campaign['tweetcounter']['hashtags'][$idx]['hashtag']) ? '#' . $campaign['tweetcounter']['hashtags'][$idx]['hashtag'] : '',
      '#prefix' => '<div class="campaign-edit-form-column">',
      '#suffix' => '</div>',
    );
    $form['campaign']['tweetcounter']['hashtags'][$idx]['url'] = array(
      '#type' => 'textfield',
      '#title' => t('URL'),
      '#description' => t('Eg. %url', array('%url' => 'http://www.mushbarf.com')),
      '#maxlength' => 255,
      '#default_value' => isset($campaign['tweetcounter']['hashtags'][$idx]['url']) ? $campaign['tweetcounter']['hashtags'][$idx]['url'] : '',
    );
  }

  $form['campaign']['tweetcounter']['add_more'] = array(
    '#type' => 'submit',
    '#value' => t('Add another hashtag to track'),
    '#submit' => array('campaign_tweetcounter_add_one'),
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
function campaign_tweetcounter_campaign_summary_alter(&$summary, $context) {
  $campaign = campaign_get_from_context($context);
  $hashtag_links = array();
  if (!isset($campaign['tweetcounter'])) {
    return;
  }
  foreach ($campaign['tweetcounter']['hashtags'] as $hashtag) {
    $hashtag_links[] = format_string('!hashtag %url', array(
      '!hashtag' => l('#' . $hashtag['hashtag'], 'admin/config/campaigns/campaign-counters', array(
        'query' => array(
          'campaign' => $context->name,
          'type' => CAMPAIGN_TWEETCOUNTER_ID,
          'ref' => $hashtag['hashtag'],
        )
      )),
      '%url' => $hashtag['url'] ? '(' . $hashtag['url'] . ')' : '',
    ));
  }
  $active = t('Active: !tweets | !sync !hashtags', array(
    '!tweets' => l(t('Tweets'), 'admin/config/campaigns/tweets', array('query' => array('campaign' => $context->name))),
    '!sync' => l(t('Trigger sync now'), 'admin/config/campaigns/tweetcounter/sync/' . $context->name),
    '!hashtags' => '<br />' . implode('<br />', $hashtag_links),
  ));
  $summary[] = array(t('Tweet Counter'), $campaign['tweetcounter']['active'] ? $active : t('Inactive'));
}

/**
 * Implements hook_campaign_save_alter().
 */
function campaign_tweetcounter_campaign_save_alter($form, $form_state, &$context) {
  $campaign = &$context->reactions['campaign_context_reaction']['campaign'];
  // Remove submit buttons value so that it's not save in the context.
  unset($campaign['tweetcounter']['add_more']);
  // Store hashtags without the # symbol.
  foreach ($campaign['tweetcounter']['hashtags'] as $idx => $hashtag) {
    $normalized_hashtag = trim(preg_replace('/^\#/', '', $hashtag['hashtag']));
    $campaign['tweetcounter']['hashtags'][$idx]['hashtag'] = $normalized_hashtag;
    if (empty($normalized_hashtag)) {
      unset($campaign['tweetcounter']['hashtags'][$idx]);
    }
  }
}

/**
 * Hook into campaign renames and change the campaign for each hashtag
 */
function campaign_tweetcounter_rename_submit($form, &$form_state) {
  $namespace = $form_state['values']['name'];
  $old_namespace = $form_state['values']['original_name'];

  // @TODO queue api should be used.
  db_update('campaign_tweetcounter_tweets')
    ->fields(array(
      'campaign' => $namespace,
    ))
    ->condition('campaign', $old_namespace)
    ->execute();
}
