<?php

/**
 * @file
 * Page callbacks for NVIDIA Flow
 */

require_once 'nvidia_flow.admin.inc';

/**
 * Page callback; presents the node editing form.
 */
function nvidia_flow_dashboard_page() {
  // Build the 'My Latest Content' section.
  $build = '<div class="row">';
  $build .= '<section id="my-latest-content" class="col-md-6">';
  $build .= '<h2>My Latest Content</h2>';
  $form = drupal_get_form('my_latest_content');
  $build .= drupal_render($form);
  $build .= '</section>';

  // Build the 'Needs Review' section.
  $build .= '<section id="needs-review" class="col-md-6">';
  $build .= '<h2>Needs Review</h2>';
  $form = drupal_get_form('needs_review');
  $build .= drupal_render($form);
  $build .= '</section>';
  $build .= '</div>';

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

  $build .= '<div class="row">';
  $build .= '<section id="latest-activity" class="col-lg-12">';
  $build .= '<h2>Recent Activity</h2>';
  $build .= theme('table', array(
    'header' => $header,
    'rows' => $activities,
    'empty' => 'No activity has been reported. All quite on the front!',
  ));
  $build .= '</section>';
  $build .= '</div>';

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
    'pager' => TRUE,
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
    'pager' => TRUE,
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
    'pager' => TRUE,
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Section; builds the My Latest Content section.
 */
function my_latest_content($form, $form_state) {
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
function needs_review($form, $form_state) {
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
    'more' => 'admin/content/node/review',
    'caller' => 'needs_review_block',
    'table_id' => 'needs-review-block-table',
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Page callback; redirects to the node create page.
 */
function nvidia_flow_create_content() {
  drupal_goto('node/add');
}

/**
 * Overrides the node/%/edit page to ensure the proper revision is shown.
 *
 * @param $node
 *   The node being acted upon.
 * @return
 *   A node editing form.
 */
function nvidia_flow_node_edit_page_override($node) {
  $node = nvidia_flow_node_latest_revision($node);
  // Ensure we have the editing code.
  module_load_include('inc', 'node', 'node.pages');
  return node_page_edit($node);
}
