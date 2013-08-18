<?php

/**
 * @file
 * Support for taxonomy vocabulary destinations.
 */

/**
 * Destination class implementing migration into vocabularies.
 */
class MigrateYamlDestinationVocabulary extends MigrateDestinationEntity {

  static public function getKeySchema() {
    return array(
      'vid' => array(
        'type' => 'int',
        'unsigned' => TRUE,
        'description' => 'ID of destination vocabulary',
      ),
    );
  }

  /**
   * Basic initialization
   */
  public function __construct() {
    parent::__construct('taxonomy_vocabulary', 'vocabulary');
  }

  /**
   * Returns a list of fields available to be mapped for this vocabulary.
   *
   * @param Migration $migration
   *   Optionally, the migration containing this destination.
   * @return array
   *   Keys: machine names of the fields (to be passed to addFieldMapping)
   *   Values: Human-friendly descriptions of the fields.
   */
  public function fields($migration = NULL) {
    $fields = array();

    $fields['vid'] = t('Vocabulary: Existing vocabulary ID');
    $fields['name'] = t('Vocabulary: Name of the vocabulary');
    $fields['machine_name'] = t('Vocabulary: The vocabulary machine name');
    $fields['description'] = t('Vocabulary: Description of the vocabulary');
    $fields['weight'] = t('Vocabulary: Weight');

    return $fields;
  }

  /**
   * Delete a migrated vocabulary.
   *
   * @param $ids
   *   Array of fields representing the key (in this case, just vid).
   */
  public function rollback(array $key) {
    $vid = reset($key);

    migrate_instrument_start('taxonomy_vocabulary_load');
    if (taxonomy_vocabulary_load($vid)) {
      migrate_instrument_stop('taxonomy_vocabulary_load');
      migrate_instrument_start('taxonomy_vocabulary_delete');
      $this->prepareRollback($vid);
      $result = (bool) taxonomy_vocabulary_delete($vid);
      $this->completeRollback($vid);
      migrate_instrument_stop('taxonomy_vocabulary_delete');
    }
    else {
      migrate_instrument_stop('taxonomy_vocabulary_load');
      // If it didn't exist, consider this a success
      $result = TRUE;
    }
    return $result;
  }

  /**
   * Import a single vocabulary.
   *
   * @param $vocabulary
   *   Vocabulary object to build. Prefilled with any fields mapped in the
   *   Migration.
   * @param $row
   *   Raw source data object - passed through to prepare/complete handlers.
   * @return array
   *   Array of key fields (vid only in this case) of the vocabulary that was
   *   saved if successful. FALSE on failure.
   */
  public function import(stdClass $vocabulary, stdClass $row) {
    $migration = Migration::currentMigration();
    // Updating previously-migrated content?
    if (isset($row->migrate_map_destid1)) {
      if (isset($vocabulary->vid)) {
        if ($vocabulary->vid != $row->migrate_map_destid1) {
          throw new MigrateException(t("Incoming vid !vid and map destination nid !destid1 don't match", array('!vid' => $vocabulary->vid, '!destid1' => $row->migrate_map_destid1)));
        }
      }
      else {
        $vocabulary->vid = $row->migrate_map_destid1;
      }
    }
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      if (!isset($vocabulary->vid)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but no destination tid provided'));
      }
      $rawvocabulary = $vocabulary;
      $this->prepare($vocabulary, $row);
      $old_vocabulary = taxonomy_vocabulary_load($vocabulary->vid);
      if (empty($old_vocabulary)) {
        throw new MigrateException(t('System-of-record is DESTINATION, but vocabulary !vid does not exist', array('!vid' => $vocabulary->vid)));
      }
      foreach ($rawvocabulary as $field => $value) {
        $old_vocabulary->$field = $vocabulary->$field;
      }
      $vocabulary = $old_vocabulary;
    }
    else {
      // Default to autoamtically created name if no vocabulary machine name
      // provided.
      if (!isset($vocabulary->machine_name)) {
        $vocabulary->machine_name = check_plain(preg_replace('/[^a-z0-9_]+/', '_', strtolower($vocabulary->name)));
      }

      $this->prepare($vocabulary, $row);
    }

    // Trying to update an existing vocabulary.
    if ($migration->getSystemOfRecord() == Migration::DESTINATION) {
      $existing_vocabulary = taxonomy_vocabulary_load($vocabulary->vid);
      if ($existing_vocabulary) {
        // Incoming data overrides existing data, so only copy non-existent
        // fields.
        foreach ($existing_vocabulary as $field => $value) {
          if (!isset($vocabulary->$field)) {
            $vocabulary->$field = $existing_vocabulary->$field;
          }
        }
      }
    }

    if (isset($vocabulary->vid)) {
      $updating = TRUE;
    }
    else {
      $updating = FALSE;
    }

    migrate_instrument_start('taxonomy_vocabulary_save');
    $status = taxonomy_vocabulary_save($vocabulary);
    migrate_instrument_stop('taxonomy_vocabulary_save');
    $this->complete($vocabulary, $row);
    if (isset($vocabulary->vid)) {
      if ($updating) {
        $this->numUpdated++;
      }
      else {
        $this->numCreated++;
      }
      $return = array($vocabulary->vid);
    }
    else {
      $return = FALSE;
    }
    return $return;
  }

  /**
   * Ensure the machine name is valid.
   *
   * @param object $vocabulary
   *   Vocabulary object to validate.
   * @param stdClass $row
   *   Raw source data object - passed through to prepare handlers.
   */
  public function prepare($vocabulary, stdClass $row) {
    parent::prepare($vocabulary, $row);

    // Verify that the machine name not only consists of replacement tokens.
    if (preg_match('@^_+$@', $vocabulary->machine_name)) {
      throw new MigrateException(t('The machine-readable name must contain unique characters.'));
    }
    // Verify that the machine name contains no disallowed characters.
    if (preg_match('@[^a-z0-9_]+@', $vocabulary->machine_name)) {
      throw new MigrateException(t('The machine-readable name must contain only lowercase letters, numbers, and underscores.'));
    }
  }

}
