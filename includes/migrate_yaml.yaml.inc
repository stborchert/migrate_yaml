<?php

/**
 * @file
 * Support for migration from YAML sources.
 */

// Import Yaml namespaces.
use Symfony\Component\Yaml\Yaml;
use Symfony\Component\Yaml\Exception\ParseException;

/**
 * Implementation of MigrateList, for retrieving a list of IDs to be migrated
 * from a YAML file.
 */
class MigrateListYAML extends MigrateList {

  /**
   * A URL pointing to an YAML object containing a list of IDs to be processed.
   *
   * @var string
   */
  protected $listUrl;

  /**
   * Basic constructor.
   *
   * @param string $list_url
   *   URL to file containing the list in YAML format.
   */
  public function __construct($list_url) {
    parent::__construct();

    // Load symfony classes.
    $loader = drupal_classloader();
    $loader->registerNamespace('Symfony\Component\Yaml', DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml') . '/lib/symfony/yaml');

    if (empty($list_url)) {
      throw new Exception(t('listUrl" is a required parameter for this migration.'));
    }
    $this->listUrl = $list_url;
  }

  /**
   * Our public face is the URL we're getting items from.
   *
   * @return string
   *   URL of YAML file.
   */
  public function __toString() {
    return $this->listUrl;
  }

  /**
   * Load the YAML  and return an array of the IDs found within the given file.
   *
   * @return array
   *   List of items.
   */
  public function getIdList() {
    migrate_instrument_start("Retrieve $this->listUrl");
    try {
      $file = $this->listUrl;
      // Parse data.
      $data = Yaml::parse($file);
      if ($data) {
        return $this->getIDsFromYAML($data);
      }
    }
    catch (ParseException $pe) {
      Migration::displayMessage(t('Loading of !listurl failed:', array('!listurl' => $this->listUrl)));
      return NULL;
    }
  }

  /**
   * List IDs of data in YAML file.
   *
   * Given an array generated from YAML, parse out the IDs for processing
   * and return them as an array. The default implementation assumes the IDs are
   * simply the values of the top-level elements - in most cases, you will need
   * to override this to reflect your particular YAML structure.
   *
   * @param array $data
   *   Data parsed from YAML.
   *
   * @return array
   *   List of IDs.
   */
  protected function getIDsFromYAML(array $data) {
    return array_keys($data);
  }

  /**
   * Count items in YAML file.
   *
   * Return a count of all available IDs from the source listing. The default
   * implementation assumes the count of top-level elements reflects the number
   * of IDs available - in many cases, you will need to override this to reflect
   * your particular JSON structure.
   *
   * @return int
   *   Number of items in YAML file.
   */
  public function computeCount() {
    $count = 0;
    try {
      $file = $this->listUrl;
      // Parse data.
      $data = Yaml::parse($file);
      if ($data) {
        $count = count($data);
      }
    }
    catch (ParseException $pe) {
      Migration::displayMessage(t('Loading of !listurl failed:', array('!listurl' => $this->listUrl)));
    }
    return $count;
  }

}

/**
 * Implementation of MigrateItem, for retrieving a parsed YAML object given
 * an ID provided by a MigrateList class.
 */
class MigrateItemYAML extends MigrateItem {

  /**
   * A URL pointing to a YAML object containing the data for one item to be
   * migrated.
   *
   * @var string
   */
  protected $itemUrl;

  /**
   * List of parsed data.
   *
   * @var array
   */
  protected $data = array();

  /**
   * Basic constructor.
   *
   * @param string $item_url
   *   URL to file containing the list in YAML format.
   */
  public function __construct($item_url) {
    parent::__construct();

    // Load symfony classes.
    $loader = drupal_classloader();
    $loader->registerNamespace('Symfony\Component\Yaml', DRUPAL_ROOT . '/' . drupal_get_path('module', 'migrate_yaml') . '/lib/symfony/yaml');

    $this->itemUrl = $item_url;
    try {
      // Parse data.
      $data = Yaml::parse($this->itemUrl);
      if ($data) {
        $this->data = $data;
      }
    }
    catch (ParseException $pe) {
      Migration::displayMessage(t('Loading of !listurl failed:', array('!listurl' => $this->itemUrl)));
      $this->data = array();
    }
  }

  /**
   * Implementors are expected to return an object representing a source item.
   *
   * @param mixed $id
   *   Internal identifier of list item
   *
   * @return stdClass
   *   A single item of the list loaded from YAML.
   */
  public function getItem($id) {
    if (isset($this->data[$id])) {
      // Return the item as object.
      return (object) $this->data[$id];
    }
    else {
      $migration = Migration::currentMigration();
      $message = t('Loading of !objecturl (!id) failed:', array('!objecturl' => $this->item_url, '!id' => $id));
      $migration->getMap()->saveMessage(array($id), $message, MigrationBase::MESSAGE_ERROR);
      return NULL;
    }
  }

}
