<?php

/**
 * @file
 * Contains code pertaining to the NVIDIA Flow Activity log
 */

/**
 * Log an activity to the database
 */
function nvidia_flow_activity_log($object, $type, $variables = array()) {
  // Set variables if the activty relates to a node
  if ($type != 'new_user' || $type != 'updated_user_roles') {
    // Load some information for the node being acted upon
    $object_id = $object->nid;
    $object_type = $object->type;
    $object_type_name = node_type_get_name($object_type);
    $object_uid = $object->uid;

    // Load the user responsible for the activity so that their username can be used
    $activity_user = user_load($object->uid);
  }

  // Set variables if the activity relates to a user
  if ($type == 'new_user') {
    // Load variables for the new or updated user
    $object_id = $object['uid'];
    $object_type = 'user';
    $object_uid = '0';
  }

  // Set variables if the activity relates to updating a user role
  if ($type == 'updated_user_roles') {
    // Load variables for the new or updated user
    $object_id = $object->uid;
    $object_type = 'user';
    $object_uid = '0';
  }
  
  $message = '';

  if(!isset($object->path['alias'])){
    $object->path['alias'] = url('node/'. $object->nid);
  }

  // Build the message based on the type of activity
  switch ($type) {
    case 'new_content':
      $message = '%username created a new %nodetype titled <a href="!url">%title</a>.';
      $variables = array('%username' => $activity_user->name, '%nodetype' => $object_type_name, '!url' => 'node/' . $object->nid, '%title' => $object->title);
      break;
    case 'updated_content':
      $message = '%username has updated the %nodetype titled <a href="!url">%title</a>.';
      $variables = array('%username' => $activity_user->name, '%nodetype' => $object_type_name, '!url' => $object->path['alias'], '%title' => $object->title);
      break;
    case 'state_transition_needs_work':
      $message = '%username has moved the %nodetype titled <a href="!url">%title</a> back to draft.';
      $variables = array('%username' => $activity_user->name, '%nodetype' => $object_type_name, '!url' => $object->path['alias'], '%title' => $object->title);
      break;
    case 'state_transition_needs_review':
      $message = '%username has queued the %nodetype titled <a href="!url">%title</a> for review.';
      $variables = array('%username' => $activity_user->name, '%nodetype' => $object_type_name, '!url' => $object->path['alias'], '%title' => $object->title);
      break;
    case 'state_transition_published':
      $message = '%username has published the %nodetype titled <a href="!url">%title</a>.';
      $variables = array('%username' => $activity_user->name, '%nodetype' => $object_type_name, '!url' => $object->path['alias'], '%title' => $object->title);
      break;
    case 'state_transition_scheduled':
      $message = '%username has scheduled the %nodetype titled <a href="!url">%title</a> to be published on %publish_date.';
      $variables = array('%username' => $activity_user->name, '%nodetype' => $object_type_name, '!url' => $object->path['alias'], '%title' => $object->title);
      break;
    case 'state_transition_unpublished':
      $message = '%username has unpublished the %nodetype titled <a href="!url">%title</a>.';
      $variables = array('%username' => $activity_user->name, '%nodetype' => $object_type_name, '!url' => $object->path['alias'], '%title' => $object->title);
      break;
    case 'new_user':
      $message = 'A new account <a href="!url">%username</a> has been created.';
      $variables = array('%username' => $object['name'], '!url' => 'user/' . $object['uid']);
      break;
    case 'updated_user_roles':
      if (isset($variables['added_role'])) {
        $message = '%username has been granted the role of %role by %grantor.';
        $variables = array('%username' => $object->name, '%role' => $variables['added_role'], '%grantor' => 'rarmstrong');
      }
      if (isset($variables['removed_role'])) {
        $message = '%username has been revolked the role of %role by %grantor.';
        $variables = array('%username' => $object->name, '%role' => $variables['removed_role'], '%grantor' => 'rarmstrong');
      }
      break;

    default:
      drupal_set_message(t('Atempted to create an acivity log with an improper activity type.'), 'error');
      break;
  }

  // Create the activity log
  Database::getConnection('default', 'default')->insert('nvidia_flow_activity')
    ->fields(array(
      'uid' => $object_uid,
      'oid' => $object_id,
      'object_type' => $object_type,
      'activity_type' => $type,
      'message' => $message,
      'variables' => serialize($variables),
      'timestamp' => time(),
    ))
    ->execute();
}

/**
 * Returns an array of activities
 *
 * @param $options array
 *   An array of options that will be passed into query
 *   - range
 *     - start: Where to start
 *     - limit: How many items to limit the query to
 *   - orderby
 *     - field: Which field to order by
 *     - sort: Sort it by ASC or DESC
 *   - date_format: 'ago' for time ago, otherwise give a string for date()
 */             
function nvidia_flow_get_activities($options) {
  $query = db_select('nvidia_flow_activity', 'a');
  $query->fields('a', array('aid', 'activity_type', 'message', 'variables', 'timestamp'));

  if (isset($options['range'])) {
    $query->range($options['range']['start'], $options['range']['limit']);
  }

  if (isset($options['orderby'])) {
    $query->orderBy($options['orderby']['field'], $options['orderby']['sort']);
  }

  $result = $query->execute();
  $activities = array();
  foreach ($result as $record) {
    $activities[$record->aid]['message'] = t($record->message, unserialize($record->variables));
    if (isset($options['date_format'])) {
      if ($options['date_format'] == 'ago') {
        $activities[$record->aid]['created'] = format_interval((time() - $record->timestamp) , 2) . t(' ago');
      } else {
        $activities[$record->aid]['created'] = date($options['date_format'], $record->timestamp);
      }
    } else {
      $activities[$record->aid]['created'] = $record->timestamp;
    }
  }

  return $activities;
}
