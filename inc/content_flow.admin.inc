<?php

/**
 * @file
 * Admin functions for editing Content Flow revisions
 */

function content_flow_build_revisions_admin($variables) {
  // Include filters if requested.
  if (isset($variables['filters'])) {
    $form['filter'] = content_flow_filter_form($variables);
    $form['#submit'] = array('content_flow_filter_form_submit');
  }

  // Build the main form.
  $form['admin'] = content_flow_admin_nodes($variables);

  return $form;
}

/**
 * List node administration filters that can be applied.
 */
function content_flow_revision_filters($variables) {
  $filters = array();

  // Build the 'Status' filter.
  $options = array();

  if ($states = state_flow_get_all_states()) {
    $options = array_combine(array_keys($states), array_keys($states));
  }

  array_unshift($options, 'any');
  $filters['status'] = array(
    'form' => array(
      '#type' => 'select',
      '#title' => t('Status'),
      '#options' => $options,
      '#access' => isset($variables['filters']['status']['show']) ? $variables['filters']['status']['show'] : TRUE,
    ),
  );

  // Build the 'Type' filter.
  $options = array();

  if ($types = node_type_get_types()) {
    foreach ($types as $machine_name => $label) {
      $options[$machine_name] = $label->name;
    }
  }

  array_unshift($options, 'any');
  $filters['type'] = array(
    'form' => array(
      '#type' => 'select',
      '#title' => t('Type'),
      '#options' => $options,
      '#access' => isset($variables['filters']['type']['show']) ? $variables['filters']['type']['show'] : TRUE,
    ),
  );

  // Build an 'Author' filter.
  $filters['author'] = array(
    'form' => array(
      '#type' => 'textfield',
      '#title' => t('Author'),
      '#default_value' => '',
      '#access' => isset($variables['filters']['author']['show']) ? $variables['filters']['author']['show'] : TRUE,
    ),
  );

  // Build an 'Title' filter.
  $filters['title'] = array(
    'form' => array(
      '#type' => 'textfield',
      '#title' => t('Title'),
      '#default_value' => '',
      '#access' => isset($variables['filters']['title']['show']) ? $variables['filters']['title']['show'] : TRUE,
    ),
  );

  // Build the 'Language' filter.
  $options = array();

  if ($languages = language_list()) {
    foreach ($languages as $language) {
      $options[$language->language] = $language->name;
    }
  }

  array_unshift($options, 'any');
  $filters['language'] = array(
    'form' => array(
      '#type' => 'select',
      '#title' => t('Language'),
      '#options' => $options,
      '#access' => isset($variables['filters']['language']['show']) ? $variables['filters']['language']['show'] : TRUE,
    ),
  );

  return $filters;
}

/**
 * Return form for node administration filters.
 */
function content_flow_filter_form($variables) {
  // Create a unqiue ID for this form.
  $form_id = $variables['caller'] . '_node_revision_filter';
  $form['form_id'] = array(
    '#type' => 'value',
    '#value' => $form_id,
  );

  // Check to see if any of the filters are set to be visible.
  $visible_filters = FALSE;
  foreach ($variables['filters'] as $filter) {
    $visible_filters = $filter['show'];
  }

  // Set default filters, if given.
  foreach ($variables['filters'] as $filter_key => $filter) {
    if (isset($filter['default'])) {
      $_SESSION[$form_id][$filter_key] = isset($_SESSION[$form_id][$filter_key]) ? $_SESSION[$form_id][$filter_key] : $filter['default'];
    }
  }

  $session = isset($_SESSION[$form_id]) ? $_SESSION[$form_id] : array();
  $filters = content_flow_revision_filters($variables);

  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only items where') . '<span class="caret"></span>',
    '#theme' => 'exposed_filters__node',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#attributes' => array('class' => array('bs-callout bs-callout-info')),
    '#access' => $visible_filters,
  );
  foreach ($session as $filter_key => $filter) {
    $filters[$filter_key]['form']['#default_value'] = $filter;
  }

  $form['filters']['status']['filters'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('filters')),
  );
  foreach ($filters as $key => $filter) {
    $form['filters']['status']['filters'][$key] = $filter['form'];
  }

  $form['filters']['status']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['filters']['status']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => count($session) ? t('Refine') : t('Filter'),
  );
  if (count($session)) {
    $form['filters']['status']['actions']['undo'] = array('#type' => 'submit', '#value' => t('Undo'));
    $form['filters']['status']['actions']['reset'] = array('#type' => 'submit', '#value' => t('Reset'));
  }

  drupal_add_js('misc/form.js');

  return $form;
}

/**
 * Process result from node administration filter form.
 */
function content_flow_filter_form_submit($form, &$form_state) {
  // Set ID.
  $form_id = $form_state['values']['form_id'];

  $filters = content_flow_revision_filters();
  switch ($form_state['values']['op']) {
    case t('Filter'):
    case t('Refine'):
      // Apply every filter that has a choice selected other than 'any'.
      foreach ($filters as $filter_key => $filter) {
        if (isset($form_state['values'][$filter_key]) && $form_state['values'][$filter_key] != '[any]') {
          $_SESSION[$form_id][$filter_key] = $form_state['values'][$filter_key];
        }
      }
      break;

    case t('Undo'):
      array_pop($_SESSION[$form_id]);
      break;

    case t('Reset'):
      $_SESSION[$form_id] = array();
      break;
  }
}

/**
 * Form builder: Builds the node administration overview.
 */
function content_flow_admin_nodes($variables) {
  // Set the ID.
  $form_id = $variables['caller'] . '_node_revision_filter';

  $admin_access = user_access('administer content revisions');

  // Build the 'Update options' form if requested.
  if (!empty($variables['update_options'])) {
    $form['options'] = array(
      '#type' => 'fieldset',
      '#title' => t('Update options') . '<span class="caret"></span>',
      '#access' => $admin_access,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#attributes' => array('class' => array('bs-callout bs-callout-info')),
    );
    $options = array();
    $operations = module_invoke_all('node_revision_operations');
    drupal_alter('content_flow_node_revision_operations', $operations);
    foreach ($operations as $operation => $array) {
      $options[$operation] = $array['label'];
    }

    $form['options']['operation'] = array(
      '#type' => 'select',
      '#title' => t('Operation'),
      '#title_display' => 'invisible',
      '#options' => $options,
      '#default_value' => 'approve',
    );
    $form['options']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#validate' => array('content_flow_admin_nodes_validate'),
      '#submit' => array('content_flow_admin_nodes_submit'),
    );
  }

  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'n.title'),
    'type' => array('data' => t('Type'), 'field' => 'n.type', 'class' => 'hidden-xs'),
    'changed' => array('data' => t('Updated'), 'field' => 'n.changed', 'sort' => 'desc'),
    'author' => array('data' => t('Author'), 'field' => 'u.name', 'class' => 'hidden-xs'),
    'status' => array('data' => t('Status'), 'class' => 'hidden-xs'),
  );

  // Conditionally remove unneeded columns.
  if (isset($variables['columns'])) {
    foreach ($variables['columns'] as $column => $status) {
      if ($status === FALSE) {
        unset($header[$column]);
      }
    }
  }

  $header['operations'] = array('data' => t('Operations'));

  $query = db_select('node', 'n')->extend('TableSort');
  if (!empty($variables['pager'])) {
    $query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
    $query->limit(50);
  }
  $query->leftJoin('users', 'u', 'n.uid = u.uid');

  $filters = isset($_SESSION[$form_id]) ? $_SESSION[$form_id] : array();
  $query
    ->fields('n', array('title', 'type', 'uid', 'status', 'vid', 'nid', 'changed'))
    ->fields('u', array('name'))
    ->orderByHeader($header)
    ->addMetaData('filters', $filters);

  // Set the range if provided.
  if (isset($variables['limit'])) {
    $query->range(0, $variables['limit']);
  }

  // Set the order if provided.
  if (isset($variables['orderby'])) {
    $query->orderBy($variables['orderby']['field'], $variables['orderby']['sort']);
  }

  $results = $query->execute();

  // Build the options.
  $options = array();
  foreach ($results as $node) {
    // Prepare the list of nodes.
    $destination = drupal_get_destination();

    // Prepare status lables.
    $statuses = ($node->status) ? content_flow_generate_status_label('published') : content_flow_generate_status_label('unpublished');
    $managed_content_type = _content_flow_managed_type($node->type);
    // Check for working drafts.
    if ($managed_content_type) {
      $working_draft = FALSE;
      $revisions = state_flow_get_revisions($node->nid);
      foreach ($revisions as $revision) {
        if ($revision->status == 1 && $revision->state != 'published') {
          $statuses .= ' ' . content_flow_generate_status_label($revision->state, 'info');
          $working_draft = TRUE;
        }
      }
    }

    // Bug: For some reason l(), url(), drupal_get_path_alias() etc are returning
    //      an extra '/' for nodes that are published and have a draft. Silly
    //      workaround.
    $node_path = str_replace('//node', '/node', url('node/' . $node->nid));
    $options[$node->nid] = array(
      'title' => array(
        'data' => '<a href="' . $node_path . '">' . $node->title . '</a>',
      ),
      'type' => array(
        'data' => check_plain(node_type_get_name($node->type)),
        'class' => 'hidden-xs',
      ),
      'changed' => t('!time ago', array('!time' => format_interval((time() - $node->changed), 2))),
      'author' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $node->name,
          '#href' => 'user/' . $node->name,
        ),
        'class' => 'hidden-xs',
      ),
      'status' => array(
        'data' => $statuses,
        'class' => 'hidden-xs',
      ),
    );

    // Conditionally remove unneeded columns.
    if (isset($variables['columns'])) {
      foreach ($variables['columns'] as $column => $enabled) {
        if ($enabled === FALSE) {
          unset($options[$node->nid][$column]);
        }
      }
    }

    // Build a list of all the accessible operations for the current node.
    $operations = array();
    if (node_access('update', node_load($node->nid))) {
      $edit_title = 'edit content';
      if ($managed_content_type) {
        $edit_title = 'new draft';
        if ($working_draft) {
          $edit_title = 'edit draft';
        }
      }
      $operations['edit'] = array(
        'title' => t($edit_title),
        'href' => 'node/' . $node->nid . '/edit',
        'query' => $destination,
      );
    }
    if (user_access('manage content workflow') && $managed_content_type) {
      $operations['workflow'] = array(
        'title' => t('workflow'),
        'href' => 'node/' . $node->nid . '/workflow',
        'query' => $destination,
      );
    }
    if (user_access('administer nodes')) {
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => 'node/' . $node->nid . '/delete',
        'query' => $destination,
      );
    }

    $options[$node->nid]['operations'] = array();
    if (count($operations) > 1) {
      // Render an unordered list of operations links.
      $options[$node->nid]['operations'] = array(
        'data' => array(
          '#theme' => 'bootstrap_btn_dropdown',
          '#links' => $operations,
          '#split' => TRUE,
          '#type' => 'default',
          '#size' => 'sm',
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      );
    }
    elseif (!empty($operations)) {
      // Render the first and only operation as a link.
      $link = reset($operations);
      $options[$node->nid]['operations'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => $link['title'],
          '#href' => $link['href'],
          '#options' => array('query' => $link['query']),
        ),
      );
    }
  }

  $form['nodes'] = array(
    '#type' => 'markup',
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $options,
    '#empty' => t('No content available.'),
    '#attributes' => array('id' => $variables['table_id']),
  );
  $form['pager'] = array('#markup' => theme('pager'));
  if (!empty($variables['type']) && $variables['type'] == 'tableselect') {
    $form['nodes']['#type'] = 'tableselect';
    $form['nodes']['#options'] = $options;
    unset($form['nodes']['#rows']);
    unset($form['nodes']['#theme']);
  }

  if (!empty($variables['more'])) {
    $form['more'] = array(
      '#type' => 'link',
      '#title' => t('More'),
      '#href' => $variables['more'],
      '#attributes' => array('class' => array('button btn btn-primary btn-xs more-button'), 'type' => array('button')),
    );
  }

  return $form;
}

/**
 * Validate node_admin_nodes form submissions.
 *
 * Check if any nodes have been selected to perform the chosen
 * 'Update option' on.
 */
function content_flow_admin_nodes_validate($form, &$form_state) {
  // Error if there are no items to select.
  if (!is_array($form_state['values']['nodes']) || !count(array_filter($form_state['values']['nodes']))) {
    form_set_error('', t('No items selected.'));
  }
}

/**
 * Process node_admin_nodes form submissions.
 *
 * Execute the chosen 'Update option' on the selected nodes.
 */
function content_flow_admin_nodes_submit($form, &$form_state) {
  $operations = module_invoke_all('node_revision_operations');
  drupal_alter('content_flow_node_revision_operations', $operations);
  $operation = $operations[$form_state['values']['operation']];

  // Filter out unchecked nodes.
  $keys = array_filter($form_state['values']['nodes']);
  $keys = array_map('_content_flow_split_keys', $keys);

  if ($function = $operation['callback']) {
    // Add in callback arguments if present.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($keys), $operation['callback arguments']);
    }
    else {
      $args = array($keys);
    }
    call_user_func_array($function, $args);

    cache_clear_all();
  }
  else {
    // We need to rebuild the form to go to a second step. For example, to
    // show the confirmation form for the deletion of nodes.
    $form_state['rebuild'] = TRUE;
  }
}

/**
 * Generates a status label.
 */
function content_flow_generate_status_label($status_name, $variation = 'default') {
  return '<span class="label label-' . $variation . '" title="' . $status_name . '" alt="' . $status_name . '">' . strtoupper(substr($status_name, 0, 1)) . '</span>';
}

/**
 * Helper function to to split the keys.
 */
function _content_flow_split_keys($key) {
  list($nid, $vid) = explode(':::', $key);
  return array(
    'vid' => $vid,
    'nid' => $nid,
  );
}

function content_flow_admin_settings($form, $form_state) {
  $form['content_flow_content_types'] = array(
    '#type' => 'fieldset',
    '#title' => t('Content Types'),
    '#description' => t('Select which content types should be managed by Content Flow.'),
  );
  foreach (node_type_get_types() as $content_type) {
    $form['content_flow_content_types']['content_flow_active_' . $content_type->type] = array(
      '#type' => 'checkbox',
      '#title' => $content_type->name,
      '#default_value' => variable_get('content_flow_active_' . $content_type->type, 0),
    );
  }

  return system_settings_form($form);
}

function content_flow_admin_update_content($form, $form_state) {
  drupal_set_message(t('IMPORTANT: Updating content will DROP all data from the tables \'node_revision_states\' and \'node_revision_states_history\'.'), 'warning');
  $form['note'] = array(
    '#type' => 'markup',
    '#markup' => '<p>Update content to have Content Flow states and history. Do this if you have installed Content Flow on a site with existing content.</p>',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Content'),
  );

  return $form;
}

function content_flow_admin_update_content_submit($form, $form_state) {
  content_flow_update_content(TRUE);
}
