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
        'text' => _utf8_4byte_to_3byte($status->text),
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

/**
 * Sanitize 4-byte UTF8 chars; no full utf8mb4 support in drupal7+mysql stack.
 * This solution runs in O(n) time BUT assumes that all incoming input is
 * strictly UTF8.
 *
 * @return
 *   the sanitized input
 */
function _utf8_4byte_to_3byte($input) {
  if (!empty($input)) {
    $utf8_2byte = 0xC0 /*1100 0000*/; $utf8_2byte_bmask = 0xE0 /*1110 0000*/;
    $utf8_3byte = 0xE0 /*1110 0000*/; $utf8_3byte_bmask = 0XF0 /*1111 0000*/;
    $utf8_4byte = 0xF0 /*1111 0000*/; $utf8_4byte_bmask = 0xF8 /*1111 1000*/;

    $sanitized = "";
    $len = strlen($input);
    for ($i = 0; $i < $len; ++$i) {
      $mb_char = $input[$i]; // Potentially a multibyte sequence
      $byte = ord($mb_char);
      if (($byte & $utf8_2byte_bmask) == $utf8_2byte) {
        $mb_char .= $input[++$i];
      }
      else if (($byte & $utf8_3byte_bmask) == $utf8_3byte) {
        $mb_char .= $input[++$i];
        $mb_char .= $input[++$i];
      }
      else if (($byte & $utf8_4byte_bmask) == $utf8_4byte) {
        // Replace with ? to avoid MySQL exception
        $mb_char = '?';
        $i += 3;
      }

      $sanitized .=  $mb_char;
    }

    $input= $sanitized;
  }

  return $input;
}
