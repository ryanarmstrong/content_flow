<?php

/**
 * @file
 * NVIDIA Flow block callbacks
 */

/**
 * Block callback; builds the My Latest Content block
 */
function my_latest_content_block($form, $form_state) {
  global $user;
  $variables = array(
    'filters' => array(
      'author' => array('show' => FALSE, 'default' => $user->name),
      'language' => array('show' => FALSE, 'default' => isset($user->language) ? $user->language : NULL),
    ),
    'columns' => array(
      'author' => FALSE,
    ),
    'limit' => 5,
    'pager' => FALSE,
    'more' => 'admin/content/node/my-content',
    'caller' => 'my_latest_content_block',
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Block callback; builds the Needs Review block
 */
function needs_review_block($form, $form_state) {
  global $user;
  $variables = array(
    'filters' => array(
      'status' => array('show' => FALSE,  'default' => 'needs review'),
      'language' => array('show' => FALSE, 'default' => isset($user->language) ? $user->language : NULL),
    ),
    'columns' => array(
      'status' => FALSE,
    ),
    'limit' => 5,
    'pager' => FALSE,
    'more' => 'admin/content/node/review',
    'caller' => 'needs_review_block',
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Block callback; builds the Latest Activity block
 */
function latest_activity_block() {
  $header = array(
    'message' => array('data' => t('Message')),
    'created' => array('data' => t('Occurred on')),
  );

  $options = array(
    'range' => array(
      'start' => 0,
      'limit' => 10,
    ),
    'orderby' => array(
      'field' => 'timestamp',
      'sort' => 'DESC',
    ),
    'date_format' => 'ago',
  );

  $activities = nvidia_flow_get_activities($options);

  $rows = array();

  $block = theme('table', array(
    'header' => $header, 
    'rows' => $activities,
    'empty' => 'No activity has been reported. All quite on the front!',
  ));

  return $block;
}
