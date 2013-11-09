<?php

/**
 * Implements hook_views_default_views().
 */
function campaign_counter_views_default_views() {
  $view = new view();
  $view->name = 'campaign_counters';
  $view->description = 'List the hits of all campaign counters used';
  $view->tag = 'campaign';
  $view->base_table = 'campaign_counter';
  $view->human_name = 'Campaign counters';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Campaign counters';
  $handler->display->display_options['use_more_always'] = FALSE;
  $handler->display->display_options['access']['type'] = 'perm';
  $handler->display->display_options['access']['perm'] = 'view campaign counters';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['exposed_form']['type'] = 'better_exposed_filters';
  $handler->display->display_options['exposed_form']['options']['reset_button_label'] = 'Palauta oletusarvoihin';
  $handler->display->display_options['exposed_form']['options']['exposed_sorts_label'] = 'Lajittele';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '50';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['quantity'] = '9';
  $handler->display->display_options['pager']['options']['tags']['first'] = '« ensimmäinen';
  $handler->display->display_options['pager']['options']['tags']['previous'] = '‹ edellinen';
  $handler->display->display_options['pager']['options']['tags']['next'] = 'seuraava ›';
  $handler->display->display_options['pager']['options']['tags']['last'] = 'viimeinen »';
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'id' => 'id',
    'campaign' => 'campaign',
    'type' => 'type',
    'ref' => 'ref',
    'hits' => 'hits',
    'updated' => 'updated',
  );
  $handler->display->display_options['style_options']['default'] = 'hits';
  $handler->display->display_options['style_options']['info'] = array(
    'id' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'campaign' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'type' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'ref' => array(
      'sortable' => 1,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'hits' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'updated' => array(
      'sortable' => 1,
      'default_sort_order' => 'desc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  /* Kenttä: Campaign counter: ID */
  $handler->display->display_options['fields']['id']['id'] = 'id';
  $handler->display->display_options['fields']['id']['table'] = 'campaign_counter';
  $handler->display->display_options['fields']['id']['field'] = 'id';
  /* Kenttä: Campaign counter: Campaign */
  $handler->display->display_options['fields']['campaign']['id'] = 'campaign';
  $handler->display->display_options['fields']['campaign']['table'] = 'campaign_counter';
  $handler->display->display_options['fields']['campaign']['field'] = 'campaign';
  $handler->display->display_options['fields']['campaign']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['campaign']['alter']['path'] = 'admin/config/campaigns/[campaign]';
  /* Kenttä: Campaign counter: Tyyppi */
  $handler->display->display_options['fields']['type']['id'] = 'type';
  $handler->display->display_options['fields']['type']['table'] = 'campaign_counter';
  $handler->display->display_options['fields']['type']['field'] = 'type';
  /* Kenttä: Campaign counter: Viittaaja */
  $handler->display->display_options['fields']['ref']['id'] = 'ref';
  $handler->display->display_options['fields']['ref']['table'] = 'campaign_counter';
  $handler->display->display_options['fields']['ref']['field'] = 'ref';
  $handler->display->display_options['fields']['ref']['alter']['make_link'] = TRUE;
  $handler->display->display_options['fields']['ref']['alter']['path'] = 'http://[ref]';
  $handler->display->display_options['fields']['ref']['alter']['external'] = TRUE;
  /* Kenttä: Campaign counter: Hit count */
  $handler->display->display_options['fields']['hits']['id'] = 'hits';
  $handler->display->display_options['fields']['hits']['table'] = 'campaign_counter';
  $handler->display->display_options['fields']['hits']['field'] = 'hits';
  /* Kenttä: Campaign counter: Päivitetty */
  $handler->display->display_options['fields']['updated']['id'] = 'updated';
  $handler->display->display_options['fields']['updated']['table'] = 'campaign_counter';
  $handler->display->display_options['fields']['updated']['field'] = 'updated';
  $handler->display->display_options['fields']['updated']['date_format'] = 'short';
  /* Filter criterion: Campaign counter: Campaign */
  $handler->display->display_options['filters']['campaign']['id'] = 'campaign';
  $handler->display->display_options['filters']['campaign']['table'] = 'campaign_counter';
  $handler->display->display_options['filters']['campaign']['field'] = 'campaign';
  $handler->display->display_options['filters']['campaign']['group'] = 1;
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
  /* Filter criterion: Campaign counter: Viittaaja */
  $handler->display->display_options['filters']['ref']['id'] = 'ref';
  $handler->display->display_options['filters']['ref']['table'] = 'campaign_counter';
  $handler->display->display_options['filters']['ref']['field'] = 'ref';
  $handler->display->display_options['filters']['ref']['group'] = 1;
  $handler->display->display_options['filters']['ref']['exposed'] = TRUE;
  $handler->display->display_options['filters']['ref']['expose']['operator_id'] = 'ref_op';
  $handler->display->display_options['filters']['ref']['expose']['label'] = 'Referrer';
  $handler->display->display_options['filters']['ref']['expose']['operator'] = 'ref_op';
  $handler->display->display_options['filters']['ref']['expose']['identifier'] = 'ref';
  $handler->display->display_options['filters']['ref']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    8 => 0,
    7 => 0,
    6 => 0,
    5 => 0,
    4 => 0,
    3 => 0,
  );
  /* Filter criterion: Campaign counter: Tyyppi */
  $handler->display->display_options['filters']['type']['id'] = 'type';
  $handler->display->display_options['filters']['type']['table'] = 'campaign_counter';
  $handler->display->display_options['filters']['type']['field'] = 'type';
  $handler->display->display_options['filters']['type']['group'] = 1;
  $handler->display->display_options['filters']['type']['exposed'] = TRUE;
  $handler->display->display_options['filters']['type']['expose']['operator_id'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['label'] = 'Type';
  $handler->display->display_options['filters']['type']['expose']['operator'] = 'type_op';
  $handler->display->display_options['filters']['type']['expose']['identifier'] = 'type';
  $handler->display->display_options['filters']['type']['expose']['remember_roles'] = array(
    2 => '2',
    1 => 0,
    8 => 0,
    7 => 0,
    6 => 0,
    5 => 0,
    4 => 0,
    3 => 0,
  );
  $handler->display->display_options['filters']['type']['group_info']['label'] = 'Type';
  $handler->display->display_options['filters']['type']['group_info']['identifier'] = 'type';
  $handler->display->display_options['filters']['type']['group_info']['group_items'] = array(
    1 => array(
      'title' => 'Blog Banner',
      'operator' => '=',
      'value' => 'campaign_banner',
    ),
    2 => array(
      'title' => 'Twitter Hashtag',
      'operator' => '=',
      'value' => 'campaign_twitter',
    ),
    3 => array(
      'title' => '',
      'operator' => '=',
      'value' => '',
    ),
  );

  /* Display: Page */
  $handler = $view->new_display('page', 'Page', 'page');
  $handler->display->display_options['path'] = 'admin/config/campaigns/campaign-counters';
  $handler->display->display_options['menu']['type'] = 'normal';
  $handler->display->display_options['menu']['title'] = 'Campaign counters';
  $handler->display->display_options['menu']['name'] = 'management';

  $views[$view->name] = $view;
  return $views;
}
