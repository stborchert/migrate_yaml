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
      'YamlExampleVocabulary' => array(
        'class_name' => 'YamlExampleVocabularyMigration',
        'description' => t('Example migration for vocabularies defined as YAML markup.'),
        'listUrl' => DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml_example') . '/yaml/vocabularies.yml',
      ),
      'YamlExampleTerm' => array(
        'class_name' => 'YamlExampleTermMigration',
        'description' => t('Example migration for taxonomy terms defined as YAML markup.'),
        'listUrl' => DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml_example') . '/yaml/categories.yml',
        'vocabulary' => 'category',
        'dependencies' => array('YamlExampleVocabulary'),
      ),
      'YamlExampleMenuLinks' => array(
        'class_name' => 'YamlExampleMenuLinksMigration',
        'description' => t('Example migration for menu links defined as YAML markup.'),
        'listUrl' => DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml_example') . '/yaml/menu_main-menu.yml',
        'menuName' => 'main-menu',
      ),
    ),
  );

  return $api;
}
