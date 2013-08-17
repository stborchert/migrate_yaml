<?php

/**
 * @file
 * Import menu links from YAML source.
 */
class YamlTestMenuLinksMigration extends YamlMenuLinksMigration {

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
    parent::__construct($arguments);
  }

}
