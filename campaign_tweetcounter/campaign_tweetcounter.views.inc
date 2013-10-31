<?php

/**
 * Implements hook_views_data().
 */
function campaign_tweetcounter_views_data() {
  $data['campaign_tweetcounter_tweets']['table']['group'] = t('Tweets');

  // Advertise this as a possible base table
  $data['campaign_tweetcounter_tweets']['table']['base'] = array(
    'field' => 'id',
    'title' => t('Tweets'),
    'help' => t('Tweets'),
    'weight' => -10,
  );

  $numeric_base = array(
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $string_base = array(
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $date_base = array(
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['campaign_tweetcounter_tweets']['id'] = array(
    'title' => t('Tweet ID'),
    'help' => t('Unique identifier of the tweet'),
  ) + $numeric_base;

  $data['campaign_tweetcounter_tweets']['created'] = array(
    'title' => t('Created'),
    'help' => t('The time the tweet was created'),
  ) + $date_base;

  $data['campaign_tweetcounter_tweets']['user'] = array(
    'title' => t('User ID'),
    'help' => t('The user identifier'),
  ) + $numeric_base;

  $data['campaign_tweetcounter_tweets']['user'] = array(
    'title' => t('User ID'),
    'help' => t('The user identifier'),
  ) + $numeric_base;

  $data['campaign_tweetcounter_tweets']['text'] = array(
    'title' => t('Text'),
    'help' => t('The tweets text content'),
  ) + $string_base;

  $data['campaign_tweetcounter_tweets']['language'] = array(
    'title' => t('Language'),
    'help' => t('The language code'),
  ) + $string_base;

  $data['campaign_tweetcounter_tweets']['campaign'] = array(
    'title' => t('Campaign'),
    'help' => t('The Campaign namespace that stored this tweet.'),
  ) + $string_base;

  $data['campaign_tweetcounter_tweets']['hashtag'] = array(
    'title' => t('Hashtag'),
    'help' => t('The hashtag used to find this tweet'),
  ) + $string_base;

  return $data;
}

