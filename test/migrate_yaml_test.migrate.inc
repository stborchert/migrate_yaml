<?php

/**
 * @file
 * Functions needed by migrate.
 */

/**
 * Implements hook_migrate_api().
 */
function migrate_yaml_test_migrate_api() {
  $api = array(
    'api' => 2,
    'migrations' => array(
      'YamlTerm' => array(
        'class_name' => 'YamlTermMigration',
        'listUrl' => DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml_test') . '/yaml/categories.yml',
        'vocabulary' => 'category',
      ),
    ),
  );

  return $api;
}
