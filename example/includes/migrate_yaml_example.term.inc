<?php

/**
 * @file
 * Import terms from YAML source.
 */
class YamlExampleTermMigration extends YamlTermMigration {

  protected $listUrl;
  protected $vocabulary;

  /**
   * Default constructor.
   *
   * @param string $list_url
   *   Path to YAML source file.
   */
  public function __construct($arguments) {
    // Always call the parent constructor first for basic setup.
    parent::__construct($arguments);
  }

}
