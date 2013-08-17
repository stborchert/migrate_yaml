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
      'YamlTestTerm' => array(
        'class_name' => 'YamlTestTermMigration',
        'listUrl' => DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml_test') . '/yaml/categories.yml',
        'vocabulary' => 'category',
      ),
      'YamlTestMenuLinks' => array(
        'class_name' => 'YamlTestMenuLinksMigration',
        'listUrl' => DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml_test') . '/yaml/menu_main-menu.yml',
        'menuName' => 'main-menu',
      ),
    ),
  );

  return $api;
}
