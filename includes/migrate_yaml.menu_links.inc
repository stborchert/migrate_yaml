<?php

/**
 * @file
 * Import menu links from YAML source.
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

    $item_url = $this->listUrl . '?id=:id';

    $this->map = new MigrateSQLMap($this->machineName, array(
        'id' => array(
          'type' => 'varchar',
          'length' => 255,
          'not null' => TRUE,
        ),
      ),
      MigrateDestinationTerm::getKeySchema()
    );

    // Define basic fields.
    $fields = array(
      'menu_name' => 'Menu name of the menu link',
      'link_path' => 'The Drupal path or external path this link points to',
      'link_title' => 'The text displayed for the link',
      'options' => 'Array of options, such as a query string or HTML attributes.',
    );

    // Create migration source.
    $this->source = new MigrateSourceList(new MigrateListYAML($this->listUrl), new MigrateItemYAML($item_url), $fields);

    // Define destination.
    $this->destination = new MigrateDestinationMenuLinks();

    // Add mappings.
    $this->addFieldMapping('menu_name')
            ->defaultValue($this->menuName);
    $this->addFieldMapping('link_path', 'link_path');
    $this->addFieldMapping('link_title', 'link_title');
  }

}
