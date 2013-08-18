<?php

/**
 * @file
 * Import vocabularies from YAML source.
 */

/**
 * Basic migration class to import vocabularies from YAML files.
 */
class YamlVocabularyMigration extends DynamicMigration {

  protected $listUrl;

  /**
   * Default constructor.
   *
   * @param string $arguments
   *   Associative array containing several arguments needed for the migration.
   *   - listURL: Path to YAML source file.
   */
  public function __construct($arguments) {
    // Always call the parent constructor first for basic setup.
    parent::__construct();
    if (empty($arguments['listUrl'])) {
      throw new Exception(t('listUrl" is a required parameter for this migration.'));
    }
    $this->listUrl = $arguments['listUrl'];

    // Add a description.
    $this->description = t('Migrate data from YAML to taxonomy vocabularies');

    $item_url = $this->listUrl;

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
      'id' => 'Unique identifier of vocabulary data',
      'name' => 'Vocabulary name',
      'machine_name' => 'Vocabulary machine name',
      'description' => 'Vocabulary description',
      'weight' => 'Vocabulary weight',
    );

    // Create migration source.
    $this->source = new MigrateSourceList(new MigrateListYAML($this->listUrl), new MigrateItemYAML($item_url), $fields);

    // Define destination.
    $this->destination = new MigrateYamlDestinationVocabulary();

    // Add mappings.
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('machine_name', 'machine_name');
    $this->addFieldMapping('description', 'description')
            ->defaultValue('');
    $this->addFieldMapping('weight', 'weight')
            ->defaultValue(0);
  }

}
