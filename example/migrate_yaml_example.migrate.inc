<?php

/**
 * @file
 * Functions needed by migrate.
 */

/**
 * Implements hook_migrate_api().
 */
function migrate_yaml_example_migrate_api() {
  $api = array(
    'api' => 2,
    'migrations' => array(
      'YamlExampleTerm' => array(
        'class_name' => 'YamlExampleTermMigration',
        'listUrl' => DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml_example') . '/yaml/categories.yml',
        'vocabulary' => 'category',
      ),
      'YamlExampleMenuLinks' => array(
        'class_name' => 'YamlExampleMenuLinksMigration',
        'listUrl' => DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml_example') . '/yaml/menu_main-menu.yml',
        'menuName' => 'main-menu',
      ),
    ),
  );

  return $api;
}
