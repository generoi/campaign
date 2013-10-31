<?php

/**
 * Implements hook_views_default_view().
 */
function campaign_tweetcounter_views_default_view() {
  $view = new view();
  $view->name = 'campaign_tweets';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'campaign_tweetcounter_tweets';
  $view->human_name = 'Campaign tweets';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Campaign tweets';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'access tweetcounter';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'campaign_1' => 'campaign_1',
    'hashtag' => 'hashtag',
    'text' => 'text',
    'campaign' => 'campaign',
    'created' => 'created',
    'language' => 'language',
    'user' => 'user',
  );
  $handler->display->display_options['style_options']['default'] = 'created';
  $handler->display->display_options['style_options']['info'] = array(
    'campaign_1' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'hashtag' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'text' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'campaign' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'created' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'language' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'user' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Field: Tweets: Campaign */
  $handler->display->display_options['fields']['campaign_1']['id'] = 'campaign_1';
  $handler->display->display_options['fields']['campaign_1']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['fields']['campaign_1']['field'] = 'campaign';
  $handler->display->display_options['fields']['campaign_1']['label'] = '';
  $handler->display->display_options['fields']['campaign_1']['exclude'] = TRUE;
  $handler->display->display_options['fields']['campaign_1']['element_label_colon'] = FALSE;
  /* Field: Tweets: Hashtag */
  $handler->display->display_options['fields']['hashtag']['id'] = 'hashtag';
  $handler->display->display_options['fields']['hashtag']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['fields']['hashtag']['field'] = 'hashtag';
  $handler->display->display_options['fields']['hashtag']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['hashtag']['alter']['path'] = 'admin/config/campaigns/campaign-counters?campaign=[campaign_1]&type=tweetcounter&ref=[hashtag]';
  /* Field: Tweets: Text */
  $handler->display->display_options['fields']['text']['id'] = 'text';
  $handler->display->display_options['fields']['text']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['fields']['text']['field'] = 'text';
  /* Field: Tweets: Campaign */
  $handler->display->display_options['fields']['campaign']['id'] = 'campaign';
  $handler->display->display_options['fields']['campaign']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['fields']['campaign']['field'] = 'campaign';
  $handler->display->display_options['fields']['campaign']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['campaign']['alter']['path'] = 'admin/config/campaigns/[campaign]';
  /* Field: Tweets: Created */
  $handler->display->display_options['fields']['created']['id'] = 'created';
  $handler->display->display_options['fields']['created']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['fields']['created']['field'] = 'created';
  $handler->display->display_options['fields']['created']['date_format'] = 'short';
  /* Field: Tweets: Language */
  $handler->display->display_options['fields']['language']['id'] = 'language';
  $handler->display->display_options['fields']['language']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['fields']['language']['field'] = 'language';
  /* Field: Tweets: User ID */
  $handler->display->display_options['fields']['user']['id'] = 'user';
  $handler->display->display_options['fields']['user']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['fields']['user']['field'] = 'user';
  $handler->display->display_options['fields']['user']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['user']['alter']['path'] = 'https://twitter.com/account/redirect_by_id/[user]';
  $handler->display->display_options['fields']['user']['alter']['external'] = TRUE;
  $handler->display->display_options['fields']['user']['separator'] = '';
  /* Filter criterion: Tweets: Campaign */
  $handler->display->display_options['filters']['campaign']['id'] = 'campaign';
  $handler->display->display_options['filters']['campaign']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['filters']['campaign']['field'] = 'campaign';
  $handler->display->display_options['filters']['campaign']['exposed'] = TRUE;
  $handler->display->display_options['filters']['campaign']['expose']['operator_id'] = 'campaign_op';
  $handler->display->display_options['filters']['campaign']['expose']['label'] = 'Campaign';
  $handler->display->display_options['filters']['campaign']['expose']['operator'] = 'campaign_op';
  $handler->display->display_options['filters']['campaign']['expose']['identifier'] = 'campaign';
  $handler->display->display_options['filters']['campaign']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    8 => 0,
    7 => 0,
    6 => 0,
    5 => 0,
    4 => 0,
    3 => 0,
  );
  /* Filter criterion: Tweets: Hashtag */
  $handler->display->display_options['filters']['hashtag']['id'] = 'hashtag';
  $handler->display->display_options['filters']['hashtag']['table'] = 'campaign_tweetcounter_tweets';
  $handler->display->display_options['filters']['hashtag']['field'] = 'hashtag';
  $handler->display->display_options['filters']['hashtag']['exposed'] = TRUE;
  $handler->display->display_options['filters']['hashtag']['expose']['operator_id'] = 'hashtag_op';
  $handler->display->display_options['filters']['hashtag']['expose']['label'] = 'Hashtag';
  $handler->display->display_options['filters']['hashtag']['expose']['operator'] = 'hashtag_op';
  $handler->display->display_options['filters']['hashtag']['expose']['identifier'] = 'hashtag';
  $handler->display->display_options['filters']['hashtag']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    8 => 0,
    7 => 0,
    6 => 0,
    5 => 0,
    4 => 0,
    3 => 0,
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/config/campaigns/tweets';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Tweets';
  $handler->display->display_options['menu']['name'] = 'management';

  $views[$view->name] = $view;
  return $views;
}
