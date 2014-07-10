<?php

/**
 * @file
 * Admin functions for editing NVIDIA Flow revisions
 */

function nvidia_flow_build_revisions_admin($variables) {
  // Include filters if requested.
  if (isset($variables['filters'])) {
    $form['filter'] = nvidia_flow_filter_form($variables);
    $form['#submit'] = array('nvidia_flow_filter_form_submit');
  }

  // Build the main form.
  $form['admin'] = nvidia_flow_admin_nodes($variables);

  return $form;
}

/**
 * List node administration filters that can be applied.
 */
function nvidia_flow_revision_filters($variables) {
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
function nvidia_flow_filter_form($variables) {
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
  $filters = nvidia_flow_revision_filters($variables);

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
function nvidia_flow_filter_form_submit($form, &$form_state) {
  // Set ID.
  $form_id = $form_state['values']['form_id'];

  $filters = nvidia_flow_revision_filters();
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
function nvidia_flow_admin_nodes($variables) {
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
    drupal_alter('nvidia_flow_node_revision_operations', $operations);
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
      '#validate' => array('nvidia_flow_admin_nodes_validate'),
      '#submit' => array('nvidia_flow_admin_nodes_submit'),
    );
  }

  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'n.title'),
    'type' => array('data' => t('Type'), 'field' => 'n.type', 'class' => 'hidden-xs'),
    'changed' => array('data' => t('Updated'), 'field' => 'nrs.timestamp', 'sort' => 'desc'),
    'author' => array('data' => t('Author'), 'class' => 'hidden-xs'),
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

  // Toggle whether to view this from a node or revision perspective.
  if (!empty($variables['content_view']) && $variables['content_view'] == 'node') {
    $query->leftJoin('node_revision_states', 'nrs', 'n.vid = nrs.vid');
    $query->leftJoin('users', 'u', 'n.uid = u.uid');
  } else {
    $query->leftJoin('node_revision_states', 'nrs', 'n.nid = nrs.nid');
    $query->leftJoin('node_revision', 'nr', 'nrs.vid = nr.vid');
    $query->leftJoin('users', 'u', 'nr.uid = u.uid');
  }

  $filters = isset($_SESSION[$form_id]) ? $_SESSION[$form_id] : array();
  $query
    ->fields('n', array('nid', 'title', 'type', 'changed', 'uid', 'vid'))
    ->fields('u', array('name'))
    ->fields('nrs', array('vid', 'state'))
    ->orderByHeader($header)
    ->addTag('node_revision')
    ->addMetaData('filters', $filters);

  // Set the range if provided.
  if (isset($variables['limit'])) {
    $query->range(0, $variables['limit']);
  }

  // Set the order if provided.
  if (isset($variables['orderby'])) {
    $query->orderBy($variables['orderby']['field'], $variables['orderby']['sort']);
  }

  $rows = $query->execute();

  $options = array();
  if ($rows) {
    // Prepare the list of nodes.
    $destination = drupal_get_destination();

    foreach ($rows as $row) {
      // Set the default presentation of the status column.
      $status = array(
        'data' => '<span class="label label-default " title="' . ucwords($row->state) . '" alt="' . ucwords($row->state) . '">' . strtoupper(substr($row->state, 0, 1)) . '</span>',
        'class' => 'hidden-xs',
      );

      // Set the default vid of the row.
      $latest_revision_vid = $row->nrs_vid;

      // Generate key based on whether we are viewing a node or revision.
      if (!empty($variables['content_view']) && $variables['content_view'] == 'node') {
        $key = $row->nid;
        $node = node_load($row->nid, $row->nrs_vid);
        if ($row->state == 'published') {
          $states = state_flow_get_revisions($node->nid);
          foreach ($states as $state) {
            if ($state->status == 1 && $state->state != 'published') {
              $latest_revision_vid = $state->vid;
              if ($state->state == 'needs review') {
                $status = array(
                  'data' => '<span class="label label-default " title="' . ucwords($row->state) . '" alt="' . ucwords($row->state) . '">' . strtoupper(substr($row->state, 0, 1)) . '</span>' . ' <span class="label label-primary" title="Needs Review" alt="Needs Review">NR</span>',
                  'class' => 'hidden-xs',
                );
              } else {
                $status = array(
                  'data' => '<span class="label label-default" title="' . ucwords($row->state) . '" alt="' . ucwords($row->state) . '">' . strtoupper(substr($row->state, 0, 1)) . '</span>' . ' <span class="label label-info" title="Working Draft" alt="Working Draft">WD</span>',
                  'class' => 'hidden-xs',
                );
              }
            }
          }
        }
      } else {
        $key = $row->nid . $row->nrs_vid;
      }

      $options[$key] = array(
        'title' => array(
          'data' => l($row->title, 'node/' . $row->nid),
        ),
        'type' => array(
          'data' => check_plain(node_type_get_name($row->type)),
          'class' => 'hidden-xs',
        ),
        'changed' => t('!time ago', array('!time' => format_interval((time() - $row->changed), 2))),
        'author' => array(
          'data' => array(
            '#type' => 'link',
            '#title' => $row->name,
            '#href' => 'user/' . $row->name,
          ),
          'class' => 'hidden-xs',
        ),
        'status' => $status,
      );

      // Conditionally remove unneeded columns.
      if (isset($variables['columns'])) {
        foreach ($variables['columns'] as $column => $status) {
          if ($status === FALSE) {
            unset($options[$key][$column]);
          }
        }
      }

      // Build a list of all the accessible operations for the current node.
      $operations = array();
      if (node_access('update', $node)) {
        $edit_title = 'new draft';
        $states = state_flow_get_revisions($row->nid);
        foreach ($states as $state) {
          if ($state->vid == $latest_revision_vid && ($state->state == 'draft' || $state->state == 'needs review')) {
            $edit_title = 'edit draft';
          }
        }
        $operations['edit'] = array(
          'title' => t($edit_title),
          'href' => 'node/' . $row->nid . '/edit',
          'query' => $destination,
        );
      }
      if (user_access('manage content workflow')) {
        $operations['workflow'] = array(
          'title' => t('workflow'),
          'href' => 'node/' . $row->nid . '/workflow',
          'query' => $destination,
        );
      }
      if (user_access('administer nodes')) {
        $operations['delete'] = array(
          'title' => t('delete'),
          'href' => 'node/' . $row->nid . '/delete',
          'query' => $destination,
        );
      }

      $options[$key]['operations'] = array();
      if (count($operations) > 1) {
        // Render an unordered list of operations links.
        $options[$key]['operations'] = array(
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
        $options[$key]['operations'] = array(
          'data' => array(
            '#type' => 'link',
            '#title' => $link['title'],
            '#href' => $link['href'],
            '#options' => array('query' => $link['query']),
          ),
        );
      }
    }
  }

  $form['nodes'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No content available.'),
    '#attributes' => array('id' => $variables['table_id']),
  );

  $form['pager'] = array('#markup' => theme('pager'));

  if (!empty($variables['more'])) {
    $form['more'] = array(
      '#type' => 'link',
      '#title' => t('More'),
      '#href' => $variables['more'],
      '#attributes' => array('class' => array('button btn btn-primary btn-xs'), 'type' => array('button')),
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
function nvidia_flow_admin_nodes_validate($form, &$form_state) {
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
function nvidia_flow_admin_nodes_submit($form, &$form_state) {
  $operations = module_invoke_all('node_revision_operations');
  drupal_alter('nvidia_flow_node_revision_operations', $operations);
  $operation = $operations[$form_state['values']['operation']];

  // Filter out unchecked nodes.
  $keys = array_filter($form_state['values']['nodes']);
  $keys = array_map('_nvidia_flow_split_keys', $keys);

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
 * Helper function to to split the keys.
 */
function _nvidia_flow_split_keys($key) {
  list($nid, $vid) = explode(':::', $key);
  return array(
    'vid' => $vid,
    'nid' => $nid,
  );
}
