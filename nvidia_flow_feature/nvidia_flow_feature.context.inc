<?php
/**
 * @file
 * nvidia_flow_feature.context.inc
 */

/**
 * Implements hook_context_default_contexts().
 */
function nvidia_flow_feature_context_default_contexts() {
  $export = array();

  $context = new stdClass();
  $context->disabled = FALSE; /* Edit this to true to make a default context disabled initially */
  $context->api_version = 3;
  $context->name = 'nvidia_flow_dashboard';
  $context->description = '';
  $context->tag = 'nvidia_flow';
  $context->conditions = array(
    'path' => array(
      'values' => array(
        'admin/content' => 'admin/content',
      ),
    ),
  );
  $context->reactions = array(
    'block' => array(
      'blocks' => array(
        'nvidia_flow-my_latest_content' => array(
          'module' => 'nvidia_flow',
          'delta' => 'my_latest_content',
          'region' => 'content',
          'weight' => '-10',
        ),
        'nvidia_flow-needs_review' => array(
          'module' => 'nvidia_flow',
          'delta' => 'needs_review',
          'region' => 'content',
          'weight' => '-9',
        ),
        'nvidia_flow-activity_monitor' => array(
          'module' => 'nvidia_flow',
          'delta' => 'activity_monitor',
          'region' => 'content',
          'weight' => '-8',
        ),
      ),
    ),
  );
  $context->condition_mode = 0;

  // Translatables
  // Included for use with string extractors like potx.
  t('nvidia_flow');
  $export['nvidia_flow_dashboard'] = $context;

  return $export;
}
