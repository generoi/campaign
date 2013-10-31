<?php

/**
 * Make a request to the twitter API.
 */
function campaign_tweetcounter_request($method, $settings, $field) {
  static $loaded = FALSE;
  if (!$loaded) {
    drupal_load('module', 'libraries');
    require_once libraries_get_path('twitter-api-php') . '/TwitterAPIExchange.php';
  }

  // https://dev.twitter.com/docs/api/1.1
  $endpoints = array(
    'tweets' => CAMPAIGN_TWEETCOUNTER_API_BASE . '/search/tweets.json',
  );

  $twitter = new TwitterAPIExchange($settings);

  switch ($method) {
    case 'tweets':
      $response = $twitter->setGetfield($field)
        ->buildOauth($endpoints[$method], 'GET')
        ->performRequest();

      try {
        $json = json_decode($response);
        if (isset($json->errors)) {
          $type = WATCHDOG_ERROR;
        } else {
          $type = WATCHDOG_INFO;
        }
      } catch(Exception $e) {
        $type = WATCHDOG_ERROR;
      }
      watchdog('campaign_tweetcounter', 'Request tweets with field %field: %response', array(
        '%field' => $field,
        '%response' => $response
      ), $type);

      if (isset($json)) {
        return $json;
      }
      break;
  }
}

/**
 * Get all tweets with mentioned hashtag since id.
 */
function campaign_tweetcounter_get_tweets($oauth, $hashtag, $since_id = FALSE) {
  $statuses = array();
  $hashtag = preg_replace('/^\#/', '', $hashtag);
  $params = array(
    'q' => '#' . $hashtag,
    'count' => '100',
    'result_type' => 'recent',
    'include_entities' => 'true',
  );
  if ($since_id) {
    $params['since_id'] = $since_id;
  }

  $query = http_build_query($params);
  $response = campaign_tweetcounter_request('tweets', $oauth, '?' . $query);
  if (!empty($response->statuses)) {
    $statuses = $response->statuses;
  }
  // @TODO handle errors
  return $statuses;
}

/**
 * Fetch all new tweets mentioning hashtag for the specified campaign and store
 * them in the database.
 */
function _campaign_tweetcounter_fetch_new($hashtag, $namespace) {
  $hashtag = preg_replace('/^\#/', '', $hashtag);
  $campaign = campaign_get($namespace);
  $oauth = array(
    'oauth_access_token' => $campaign['tweetcounter']['oauth_access_token'],
    'oauth_access_token_secret' => $campaign['tweetcounter']['oauth_access_token_secret'],
    'consumer_key' => $campaign['tweetcounter']['consumer_key'],
    'consumer_secret' => $campaign['tweetcounter']['consumer_secret'],
  );
  $all_statuses = array();
  if (count(array_diff($oauth, array(FALSE))) != count($oauth)) {
    watchdog('campaign_tweetcounter', 'Missing OAuth settings to fetch tweets.');
    return $all_statuses;
  }

  // Fetch the last tweet id we stored.
  $last_id = db_select('campaign_tweetcounter_tweets', 'c')
    ->fields('c', array('id'))
    ->condition('campaign', $namespace)
    ->condition('hashtag', $hashtag)
    ->orderBy('created', 'DESC')
    ->range(0, 1)
    ->execute()
    ->fetchField();

  while (true) {
    $statuses = campaign_tweetcounter_get_tweets($oauth, $hashtag, $last_id);
    $all_statuses = array_merge($all_statuses, $statuses);
    foreach ($statuses as $status) {
      $tweet = array(
        'id' => $status->id,
        'created' => strtotime($status->created_at),
        'user' => $status->user->id_str,
        'text' => $status->text,
        'language' => $status->metadata->iso_language_code,
        'hashtag' => $hashtag,
      );
      $q = db_insert('campaign_tweetcounter_tweets')
        ->fields(array(
          'campaign' => $namespace,
        ) + $tweet);
      $q->execute();
    }
    // Break out unless limit was reached.
    if (count($statuses) < 100) {
      break;
    }
  }

  return $all_statuses;
}
