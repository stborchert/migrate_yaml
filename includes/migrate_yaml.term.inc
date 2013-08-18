<?php

/**
 * @file
 * Import terms from YAML source.
 */

/**
 * Basic migration class to import terms from YAML files.
 */
class YamlTermMigration extends DynamicMigration {

  protected $listUrl;
  protected $vocabulary;

  /**
   * Default constructor.
   *
   * @param string $arguments
   *   Associative array containing several arguments needed for the migration.
   *   - listURL: Path to YAML source file.
   *   - vocabulary: Machine name of vocabulary to import the terms to.
   */
  public function __construct($arguments) {
    // Always call the parent constructor first for basic setup.
    parent::__construct();
    if (empty($arguments['listUrl'])) {
      throw new Exception(t('"listUrl" is a required parameter for this migration.'));
    }
    $this->listUrl = $arguments['listUrl'];
    if (empty($arguments['vocabulary'])) {
      throw new Exception(t('"vocabulary" is a required parameter for this migration.'));
    }
    $this->vocabulary = $arguments['vocabulary'];

    // Add a description.
    $this->description = t('Migrate data from YAML to taxonomy terms');

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
      'id' => 'Unique identifier of term data',
      'name' => 'Term name',
      'description' => 'Term description',
      'parent' => 'ID of parent term',
      'parent_name' => 'Name of parent term',
      'weight' => 'Term weight',
    );

    // Create migration source.
    $this->source = new MigrateSourceList(new MigrateListYAML($this->listUrl), new MigrateItemYAML($item_url), $fields);

    // Define destination.
    $this->destination = new MigrateDestinationTerm($this->vocabulary);

    // Add mappings.
    $this->addFieldMapping('name', 'name');
    $this->addFieldMapping('description', 'description')
            ->defaultValue('');
    $this->addFieldMapping('parent', 'parent')
            ->sourceMigration(array($this->getMachineName()));
    $this->addFieldMapping('parent_name', 'parent_name')
            ->sourceMigration(array($this->getMachineName()));
    $this->addFieldMapping('format')
            ->defaultValue('plain_text');
    $this->addFieldMapping('weight', 'weight')
            ->defaultValue(0);
  }

  /**
   * Prepare row data for migration.
   *
   * @param object $row
   *   The current processed row.
   */
  public function prepareRow($row) {
    // If there is not parent term id for the term, unset the parent value.
    // Taxonomy does not like for a value to be provided if it does not exist.
    if (empty($row->parent)) {
      unset($row->parent);
    }

    return TRUE;
  }

  /**
   * Creates a stub for non-existing terms.
   *
   * @param object $migration
   *   The implementing migration.
   * @param array $source_id
   *   List of term IDs to look for.
   *
   * @return (array|bool)
   *   List of term IDs of new terms or FALSE on failure.
   */
  protected function createStub($migration, $source_id) {
    // Ignore an attempt to create a stub corresponding to "tid" 0.
    if (empty($source_id[0])) {
      return FALSE;
    }

    migrate_instrument_start('YamlTermMigration::createStub');
    $vocab = taxonomy_vocabulary_machine_name_load($this->vocabulary);
    $term = new stdClass();
    $term->vid = $vocab->vid;
    $term->name = t('Stub for term !term', array('!term' => $source_id[0]));
    taxonomy_term_save($term);
    migrate_instrument_stop('YamlTermMigration::createStub');
    if (isset($term->tid)) {
      return array($term->tid);
    }
    else {
      return FALSE;
    }
  }

}
