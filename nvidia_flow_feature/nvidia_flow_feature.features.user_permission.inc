<?php
/**
 * @file
 * nvidia_flow_feature.features.user_permission.inc
 */

/**
 * Implements hook_user_default_permissions().
 */
function nvidia_flow_feature_user_default_permissions() {
  $permissions = array();

  // Exported permission: 'access content dashboard'.
  $permissions['access content dashboard'] = array(
    'name' => 'access content dashboard',
    'roles' => array(
      'administrator' => 'administrator',
      'authenticated user' => 'authenticated user',
      'editor' => 'editor',
      'writer' => 'writer',
    ),
    'module' => 'nvidia_flow',
  );

  // Exported permission: 'access navbar'.
  $permissions['access navbar'] = array(
    'name' => 'access navbar',
    'roles' => array(
      'administrator' => 'administrator',
      'editor' => 'editor',
      'writer' => 'writer',
    ),
    'module' => 'navbar',
  );

  // Exported permission: 'administer content revisions'.
  $permissions['administer content revisions'] = array(
    'name' => 'administer content revisions',
    'roles' => array(
      'administrator' => 'administrator',
    ),
    'module' => 'state_flow',
  );

  // Exported permission: 'approve and reject content'.
  $permissions['approve and reject content'] = array(
    'name' => 'approve and reject content',
    'roles' => array(
      'administrator' => 'administrator',
      'editor' => 'editor',
    ),
    'module' => 'nvidia_flow',
  );

  // Exported permission: 'manage content workflow'.
  $permissions['manage content workflow'] = array(
    'name' => 'manage content workflow',
    'roles' => array(
      'administrator' => 'administrator',
      'editor' => 'editor',
      'writer' => 'writer',
    ),
    'module' => 'state_flow',
  );

  // Exported permission: 'schedule content workflow'.
  $permissions['schedule content workflow'] = array(
    'name' => 'schedule content workflow',
    'roles' => array(
      'administrator' => 'administrator',
      'editor' => 'editor',
    ),
    'module' => 'state_flow_schedule',
  );

  // Exported permission: 'view any unpublished content'.
  $permissions['view any unpublished content'] = array(
    'name' => 'view any unpublished content',
    'roles' => array(
      'administrator' => 'administrator',
      'authenticated user' => 'authenticated user',
    ),
    'module' => 'view_unpublished',
  );

  return $permissions;
}
