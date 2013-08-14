<?php

/**
 * @file
 * Functions needed by migrate.
 */

/**
 * Implements hook_migrate_api().
 */
function migrate_yaml_migrate_api() {
  $api = array(
    'api' => 2,
  );

  return $api;
}
