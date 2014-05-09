<?php

/**
 * @file
 * Page callbacks for NVIDIA Flow
 */

/**
 * Menu callback; presents the node editing form.
 */
function nvidia_flow_dashboard_page() {
  // Build the 'My Latest Content' section.
  $build = drupal_render(drupal_get_form('my_latest_content_block'));
  // Build the 'Needs Review' section.
  $build .= drupal_render(drupal_get_form('needs_review_block'));
  // Build the activities section.
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
  $build .= theme('table', array(
    'header' => $header,
    'rows' => $activities,
    'empty' => 'No activity has been reported. All quite on the front!',
  ));

  return $build;
}

/**
 * Menu callback; presents the node editing form.
 */
function nvidia_flow_needs_review_page($form, $form_state) {
  global $user;
  $variables = array(
    'filters' => array(
      'status' => array('show' => FALSE,  'default' => 'needs review'),
      'type' => array('show' => TRUE),
      'author' => array('show' => TRUE),
      'title' => array('show' => TRUE),
      'language' => array('show' => TRUE, 'default' => isset($user->language) ? $user->language : NULL),
    ),
    'update_options' => TRUE,
    'columns' => array(
      'status' => FALSE,
    ),
    'caller' => 'nvidia_flow_needs_review_page',
    'table_id' => 'content-table',
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Menu callback; presents all content on the site.
 */
function nvidia_flow_all_content_page($form, $form_state) {
  global $user;
  $variables = array(
    'filters' => array(
      'status' => array('show' => TRUE),
      'type' => array('show' => TRUE),
      'author' => array('show' => TRUE),
      'title' => array('show' => TRUE),
      'language' => array('show' => TRUE, 'default' => isset($user->language) ? $user->language : NULL),
    ),
    'update_options' => TRUE,
    'content_view' => 'node',
    'caller' => 'nvidia_flow_all_content_page',
    'table_id' => 'content-table',
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Menu callback; presents current users content on the site.
 */
function nvidia_flow_my_content_page($form, $form_state) {
  global $user;
  $variables = array(
    'filters' => array(
      'status' => array('show' => TRUE),
      'type' => array('show' => TRUE),
      'author' => array('show' => FALSE, 'default' => $user->name),
      'title' => array('show' => TRUE),
      'language' => array('show' => TRUE, 'default' => isset($user->language) ? $user->language : NULL),
    ),
    'update_options' => TRUE,
    'columns' => array(
      'author' => FALSE,
    ),
    'caller' => 'nvidia_flow_my_content_page',
    'table_id' => 'content-table',
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Section; builds the My Latest Content section.
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
    'table_id' => 'my-latest-content-block-table',
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Section; builds the Needs Review section.
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
    'table_id' => 'needs-review-block-table',
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}
