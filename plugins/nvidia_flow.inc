<?php

/**
 * @file
 * State Flow implementation of the State Machine class
 */

class NvidiaFlow extends StateFlow {
	public function init() {
		// Initialize states
    $this->create_state('draft', array(
      'label' => t('Draft'),
    ));
    $this->create_state('needs review', array(
      'label' => t('Needs Review'),
    ));
    $this->create_state('published', array(
      'label' => t('Published'),
      'on_enter' => array($this, 'on_enter_published'),
      'on_exit' => array($this, 'on_exit_published'),
    ));
    $this->create_state('scheduled', array(
      'label' => t('Scheduled'),
      'on_exit' => array($this, 'on_exit_scheduled'),
    ));
    $this->create_state('unpublished', array(
      'label' => t('Unpublished'),
    ));

		// Initialize events
    $this->create_event('needs review', array(
      'label' => t('Queue for Review'),
      'origin' => 'draft',
      'target' => 'needs review',
      'after_transition' => 'after_transition_needs_review',
    ));
    $this->create_event('needs work', array(
      'label' => t('Needs Work'),
      'origin' => array('needs review', 'scheduled', 'unpublished'),
      'target' => 'draft',
      'after_transition' => 'after_transition_needs_work',
    ));
    $this->create_event('publish', array(
      'label' => t('Publish'),
      'origin' => array('draft', 'needs review', 'scheduled', 'unpublished'),
      'target' => 'published',
      'guard' => 'nvidia_flow_guard_publish',
      'after_transition' => 'after_transition_publish',
    ));
    $this->create_event('schedule', array(
      'label' => t('Schedule'),
      'origin' => 'needs review',
      'target' => 'scheduled',
      'guard' => 'state_flow_guard_schedule',
      'after_transition' => 'after_transition_scheduled',
    ));
    $this->create_event('unpublish', array(
      'label' => t('Unpublish'),
      'origin' => 'published',
      'target' => 'unpublished',
      'permission' => 'publish and unpublish content',
      'after_transition' => 'after_transition_unpublished',
    ));
	}

  public function get_event($key) {
    if (!array_key_exists($key, $this->events)) {
      return FALSE;
    }

    if (is_array($this->events[$key])) {
      $options = $this->events[$key];
      $this->events[$key] = new NvidiaFlow_Event($key, $this, $options);
    }

    return $this->events[$key];
  }
}

class NvidiaFlow_Event extends StateFlow_Event {
  /**
   * Allow the event to finish after the machine has changed state.
   */
  public function finish() {
    if (!empty($this->options['after_transition'])) {
      // Pass the object being operated on
      $machine = $this->get_machine();
      $object = $machine->get_object();

      // Call the defined after_transition callback if one has been specified
      call_user_func($this->options['after_transition'], $object);
    }
  }
}

/**
 * Guard callbacks
 */

function nvidia_flow_guard_publish($event, $permission) {
  return nvidia_flow_guard_permission($event, 'approve and reject content');
}

/**
 * Helper function for evaluating an access decision with either the global or
 * group-specific permission.
 */
function nvidia_flow_guard_permission($event, $permission) {
  // If the user has the global permission, then return TRUE
  if (user_access($permission)) {
    return TRUE;
  }

  // Otherwise return FALSE
  return FALSE;
}

/**
 * Transition callbacks
 */

function after_transition_needs_work($object) {
  nvidia_flow_activity_log($object, 'state_transition_needs_work');
}

function after_transition_needs_review($object) {
  nvidia_flow_activity_log($object, 'state_transition_needs_review');
}

function after_transition_publish($object) {
  nvidia_flow_activity_log($object, 'state_transition_published');
}

function after_transition_scheduled($object) {
  nvidia_flow_activity_log($object, 'state_transition_scheduled');
}

function after_transition_unpublished($object) {
  nvidia_flow_activity_log($object, 'state_transition_unpublished');
}
