<?php

/**
 * @file
 * Import vocabularies from YAML source.
 */
class YamlExampleVocabularyMigration extends YamlVocabularyMigration {

  protected $listUrl;

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
