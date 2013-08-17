<?php

/**
 * @file
 * Import menu links from YAML source.
 */

/**
 * Basic migration class to import menu links from YAML files.
 */
class YamlMenuLinksMigration extends DynamicMigration {

  protected $listUrl;
  protected $menuName;

  /**
   * Default constructor.
   *
   * @param string $arguments
   *   Associative array containing several arguments needed for the migration.
   *   - listURL: Path to YAML source file.
   *   - menuName: Name of menu to import the menu items to.
   */
  public function __construct($arguments) {
    // Always call the parent constructor first for basic setup.
    parent::__construct();
    if (empty($arguments['listUrl'])) {
      throw new Exception(t('listUrl" is a required parameter for this migration.'));
    }
    $this->listUrl = $arguments['listUrl'];
    if (empty($arguments['menuName'])) {
      throw new Exception(t('menuName" is a required parameter for this migration.'));
    }
    $this->menuName = $arguments['menuName'];

    // Add a description.
    $this->description = t('Migrate data from YAML to menu links.');

    $item_url = $this->listUrl;

    $this->map = new MigrateSQLMap($this->machineName, array(
        'mlid' => array(
          'type' => 'int',
          'unsigned' => TRUE,
          'not null' => TRUE,
          'description' => 'ID of destination link',
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    // Define basic fields.
    $fields = array(
      'module' => 'The name of the module that generated this link.',
      'menu_name' => 'Menu name of the menu link',
      'link_path' => 'The Drupal path or external path this link points to',
      'link_title' => 'The text displayed for the link',
      'expanded' => 'Flag for whether this link should be rendered as expanded in menus',
      'hidden' => 'A flag for whether the link should be rendered in menus. (1 = disabled menu item, -1 = menu callback, 0 = normal, visible link)',
      'options' => 'Array of options, such as a query string or HTML attributes',
      'weight' => 'Link weight among links in the same menu at the same depth',
      'parent' => 'Unique ID of parent menu link',
    );

    // Create migration source.
    $this->source = new MigrateSourceList(new MigrateListYAML($this->listUrl), new MigrateItemYAML($item_url), $fields);

    // Define destination.
    $this->destination = new MigrateDestinationMenuLinks();

    // Add mappings.
    $this->addFieldMapping('module', 'module')
            ->defaultValue('migrate_yaml');
    $this->addFieldMapping('menu_name', 'menu_name')
            ->defaultValue($this->menuName);
    $this->addFieldMapping('link_path', 'link_path');
    $this->addFieldMapping('link_title', 'link_title');
    $this->addFieldMapping('expanded', 'expanded')
            ->defaultValue(0);
    $this->addFieldMapping('hidden', 'hidden')
            ->defaultValue(0);
    $this->addFieldMapping('options', 'options')
            ->defaultValue(array());
    $this->addFieldMapping('weight', 'weight')
            ->defaultValue(0);
    $this->addFieldMapping('plid', 'parent')
            ->defaultValue(0)
            ->sourceMigration(array($this->getMachineName()));
  }

  /**
   * Creates a stub menu link, for when a child is imported before its parent.
   *
   * @param object $migration
   *   The implementing migration.
   * @param array $source_id
   *   List of menu link IDs to look for.
   *
   * @return (array|bool)
   *   List of menu_link IDs on success, FALSE otherwise.
   */
  protected function createStub($migration, $source_id) {
    // If parent is empty, that means it has no parent, so don't create a stub.
    if (!$migration->sourceValues->parent) {
      return FALSE;
    }
    $menu_link = array(
      'menu_name' => $this->menuName,
      'link_path' => 'stub-path',
      'router_path' => 'stub-path',
      'link_title' => t('Stub title'),
    );
    $mlid = menu_link_save($menu_link);
    if ($mlid) {
      return array($mlid);
    }
    else {
      return FALSE;
    }
  }

}
