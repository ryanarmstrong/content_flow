<?php

/**
 * @file
 * Page callbacks for NVIDIA Flow
 */

/**
 * Menu callback; presents the node editing form.
 */
function nvidia_flow_dashboard_page() {
  $build = '<p></p>';
  
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
  );
  $form = nvidia_flow_build_revisions_admin($variables);

  return $form;
}
