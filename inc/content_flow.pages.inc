<?php

/**
 * @file
 * Page callbacks for Content Flow
 */

require_once 'content_flow.admin.inc';

/**
 * Page callback; presents the node editing form.
 */
function content_flow_dashboard_page() {
  $width_class = 'col-md-12';
  $build = '<div class="row">';

  // Build the 'Needs Review' section.
  if (user_access('approve and reject content')) {
    $needs_review_block = '<section id="needs-review" class="col-md-6">';
    $needs_review_block .= '<h2>Needs Review</h2>';
    $form = drupal_get_form('needs_review');
    $needs_review_block .= drupal_render($form);
    $needs_review_block .= '</section>';
    $width_class = 'col-md-6';
  }

  // Build the 'My Latest Content' section.
  $my_latest_content_block = '<section id="my-latest-content" class="' . $width_class . '">';
  $my_latest_content_block .= '<h2>My Latest Content</h2>';
  $form = drupal_get_form('my_latest_content');
  $my_latest_content_block .= drupal_render($form);
  $my_latest_content_block .= '</section>';

  $build .= $my_latest_content_block;
  $build .= $needs_review_block;

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
  $activities = content_flow_get_activities($options);

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
function content_flow_needs_review_page($form, $form_state) {
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
    'caller' => 'content_flow_needs_review_page',
    'table_id' => 'content-table',
    'pager' => TRUE,
    'content_view' => 'node',
  );
  $form = content_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Menu callback; presents all content on the site.
 */
function content_flow_all_content_page($form, $form_state) {
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
    'caller' => 'content_flow_all_content_page',
    'table_id' => 'content-table',
    'pager' => TRUE,
    'content_view' => 'node',
  );
  $form = content_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Menu callback; presents current users content on the site.
 */
function content_flow_my_content_page($form, $form_state) {
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
    'caller' => 'content_flow_my_content_page',
    'table_id' => 'content-table',
    'pager' => TRUE,
    'content_view' => 'node',
  );
  $form = content_flow_build_revisions_admin($variables);

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
    'content_view' => 'node',
  );
  $form = content_flow_build_revisions_admin($variables);

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
    'content_view' => 'node',
  );
  $form = content_flow_build_revisions_admin($variables);

  return $form;
}

/**
 * Page callback; redirects to the node create page.
 */
function content_flow_create_content() {
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
function content_flow_node_edit_page_override($node) {
  $node = content_flow_node_latest_revision($node);
  // Ensure we have the editing code.
  module_load_include('inc', 'node', 'node.pages');
  return node_page_edit($node);
}

/**
 * Displays the current draft the node, if it is not published.
 *
 * @param $node
 *   The node being acted upon.
 *
 * @return
 *   A fully themed node page.
 */
function content_flow_node_view_draft($node) {
  $node = content_flow_node_latest_revision($node);
  $router_item = menu_get_item('node/' . $node->nid);
  if ($router_item['include_file']) {
    require_once DRUPAL_ROOT . '/' . $router_item['include_file'];
  }

  // Call whatever function is assigned to the main node path but pass the
  // current node as an argument. This approach allows for the reuse of of Panel
  // definition acting on node/%node.
  return $router_item['page_callback']($node);
}

/**
 * Page callback for a node's Workflow page.
 */
function content_flow_events($node) {
  // Gather information on the current published revision
  $state_flow = state_flow_load_state_machine($node);
  $events = $state_flow->get_available_events();
  $current_revision_actions = array();
  foreach ($events as $event_machine_name => $event) {
    if (state_flow_access($node, $event_machine_name)) {
      $current_revision_actions[] = l($event->get_option('label'), 'node/' . $node->nid . '/revisions/' . $node->vid . '/workflow/' . $event_machine_name);
    }
  }
  $current_revision_actions_str = implode(' | ', $current_revision_actions);

  // Build table for the active draft revisions and other revisions of this node
  $drafts_table = array(
    'header' => array(
      t('Revision ID'),
      t('Title'),
      t('Status'),
      t('Last Updated'),
      t('Author'),
      t('Actions'),
    ),
    'rows' => array(),
    'empty' => t('There isn\'t a working draft.'),
  );
  $others_table = array(
    'header' => array(
      t('Revision ID'),
      t('Title'),
      t('Last Updated'),
      t('Author'),
    ),
    'rows' => array(),
    'empty' => t('There aren\'t any archived drafts.'),
  );
  $states = state_flow_get_revisions($node->nid);
  foreach ($states as $state) {
    if (empty($state->state)) {
      continue;
    }
    $vid = intval($state->vid);
    $path_view = ($vid == $node->vid) ? 'node/' . $node->nid : 'node/' . $node->nid . '/revisions/' . $vid . '/view';
    $state_date_str = format_date($state->timestamp, 'short');
    if (($state->status && !($state->state === 'published' && $vid == $node->vid)) || (!$state->status && $vid != $node->vid)) {
      $state_node = node_load($node->nid, $vid);
      $rev_author = user_load($state_node->revision_uid);
      $state_state_flow = state_flow_load_state_machine($state_node);
      $state_events = $state_state_flow->get_available_events();
      $state_actions = array();
      if (node_access('update', $state_node)) {
        $state_actions[] = l(t('Edit Draft'), 'node/' . $node->nid . '/edit');
      }
      if (_node_revision_access($state_node, 'delete')) {
        $state_actions[] = l(t('Delete'), 'node/' . $node->nid . '/revisions/' . $vid . '/delete');
      }
      foreach ($state_events as $event_machine_name => $event) {
        if (state_flow_access($state_node, $event_machine_name)) {
          $state_actions[] = l($event->get_option('label'), 'node/' . $node->nid . '/revisions/' . $vid . '/workflow/' . $event_machine_name) . ' ';
        }
      }
      $state_actions_str = implode(' | ', $state_actions);
      $item = array(
        l($vid, $path_view),
        l(check_plain($state_node->title), $path_view),
        '<span class="label label-default " title="' . ucwords($state->state) . '" alt="' . ucwords($state->state) . '">' . ucwords($state->state) . '</span>',
        $state_date_str,
        theme('username', array('account' => $rev_author)),
        $state_actions_str,
      );
      if ($state->status) {
        $drafts_table['rows'][] = $item;
      }
      else {
        $item = array(
          l($vid, $path_view),
          l(check_plain($state_node->title), $path_view),
          $state_date_str,
          theme('username', array('account' => $rev_author)),
        );
        $others_table['rows'][] = $item;
      }
    }
  }

  // Build a table for the history of this node
  $history_table = array(
    'header' => array(
      t('Date'),
      t('Message'),
    ),
    'rows' => array(),
  );
  $history = state_flow_get_history($node->nid);
  foreach ($history as $state) {
    $vid = intval($state->vid);
    $path_view = ($vid == $node->vid) ? 'node/' . $node->nid : 'node/' . $node->nid . '/revisions/' . $vid . '/view';
    $state_date_str = format_date($state->timestamp, 'short');
    $history_str = '';
    $user_name = format_username((object) array('uid' => (int) $state->uid, 'name' => $state->user_name));
    $history_str = t('!user transitioned revision !vid-link to %state.',
      array(
        '!user' => l(check_plain($user_name), 'user/' . $state->uid),
        '!vid-link' => l($vid,  $path_view),
        '%state' => $state->state
      )
    );
    if (!empty($state->log)) {
      $history_str .= '<br/>' . t('@user said: @log', array('@user' => $user_name, '@log' => $state->log));
    }
    $history_table['rows'][] = array(
      $state_date_str,
      $history_str,
    );
  }

  // Set the title for this page
  drupal_set_title(t('Workflow for @title', array('@title' => $node->title)), PASS_THROUGH);

  // Build the render array
  $output = array(
    'current_revision' => array(
      '#type' => 'fieldset',
      '#title' => t('Current Revision'),
      'current_revision_status' => array(
        '#type' => 'item',
        '#title' => t('Status'),
        '#markup' => '<span class="label label-default " title="' . ucwords(check_plain($state_flow->get_current_state())) . '" alt="' . ucwords(check_plain($state_flow->get_current_state())) . '">' . ucwords(check_plain($state_flow->get_current_state())) . '</span>',
      ),
      'current_revision_vid' => array(
        '#type' => 'item',
        '#title' => t('Revision ID'),
        '#markup' => l(check_plain($node->vid), 'node/' . $node->nid),
      ),
    ),
    '#sorted' => TRUE,
    '#attached' => array(
      'js' => array(
        'misc/form.js',
        'misc/collapse.js',
      ),
    ),
  );
  if (!empty($current_revision_actions_str)) {
    $output['content']['current_revision']['current_revision_actions'] = array(
      '#type' => 'item',
      '#title' => t('Actions'),
      '#markup' => $current_revision_actions_str,
    );
  }
  $output['content']['draft_revisions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Working Draft'),
    'draft_revisions_table' => array(
      '#markup' => theme('table', $drafts_table),
    ),
  );
  $output['content']['other_revisions'] = array(
    '#type' => 'fieldset',
    '#title' => t('Archived Drafts'),
    'other_revisions_table' => array(
      '#markup' => theme('table', $others_table),
    ),
    '#attributes' => array(
      'class' => array('collapsible', 'collapsed'),
    ),
  );
  $output['content']['history'] = array(
    '#type' => 'fieldset',
    '#title' => t('History'),
    'history_table' => array(
      '#markup' => theme('table', $history_table),
    ),
    '#attributes' => array(
      'class' => array('collapsible', 'collapsed'),
    ),
  );

  return $output;
}
