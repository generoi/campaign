<?php

/**
 * Implements hook_views_data().
 */
function campaign_counter_views_data() {
  $data['campaign_counter']['table']['group'] = t('Campaign counter');

  // Advertise this as a possible base table
  $data['campaign_counter']['table']['base'] = array(
    'field' => 'id',
    'title' => t('Campaign counter'),
    'help' => t('Campaign counter'),
    'weight' => -10,
  );

  $data['campaign_counter']['id'] = array(
    'title' => t('ID'),
    'help' => t('Unique identifier'),
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

  $data['campaign_counter']['hits'] = array(
    'title' => t('Hit count'),
    'help' => t('The number of hits for this reference'),
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

  $data['campaign_counter']['ref'] = array(
    'title' => t('Referrer'),
    'help' => t('The referrer'),
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
  $data['campaign_counter']['campaign'] = array(
    'title' => t('Campaign'),
    'help' => t('The campaign namespace'),
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
  $data['campaign_counter']['type'] = array(
    'title' => t('Type'),
    'help' => t('The counter type'),
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
  $data['campaign_counter']['updated'] = array(
    'title' => t('Updated'),
    'help' => t('The time of the last counter increment'),
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

  return $data;
}
