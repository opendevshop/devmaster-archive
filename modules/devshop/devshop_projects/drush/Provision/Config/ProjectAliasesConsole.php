<?php
/**
 * @file
 * Provides the Provision_Config_ProjectAliasesConsole class.
 */

use Symfony\Component\Yaml\Dumper;

/**
 * Class to write an alias records.
 */
class Provision_Config_ProjectAliasesConsole extends Provision_Config {

  /**
   * @param $name
   *   String '\@name' for named context.
   * @param $options
   *   Array of string option names to save.
   */
  function __construct($context, $environments = array()) {

    parent::__construct($context, $environments);
    $this->data = array(
      'name' => ltrim($context, '@project_'),
      'environments' => $environments,
    );
  }

  function filename() {
    return drush_server_home() . '/.console/sites/' . $this->data['name'] . '.yml';
  }

  function getYmlDump() {
    $aliases = array();

    foreach ($this->data['environments'] as $name => $environment) {
      if ($environment['site_status'] != 1) {
        continue;
      }
      $aliases[$name] = array(
          'root' => $environment['root'],
          'uri' => $environment['uri'],
          'host' => d('web_server')->remote_host,
          'user' => d('web_server')->script_user,
      );
    }
    $dumper = new Dumper();
    return $dumper->dump($aliases);
  }

  /**
   * Dummy method. We don't need a template, it's YAML!
   */
  function load_template() {
  }

  /**
   * Write out this configuration.
   *
   * 1. Make sure parent directory exists and is writable.
   * 2. Load template with load_template().
   * 3. Process $data with process().
   * 4. Make existing file writable if necessary and possible.
   * 5. Render template with $this and $data and write out to filename().
   * 6. If $mode and/or $group are set, apply them for the new file.
   */
  function write() {
    $filename = $this->filename();
    // Make directory structure if it does not exist.
    if ($filename && !provision_file()->exists(dirname($filename))->status()) {
      provision_file()->mkdir(dirname($filename))
          ->succeed('Created directory @path.')
          ->fail('Could not create directory @path.');
    }

    $status = FALSE;
    if ($filename && is_writeable(dirname($filename))) {
      // Make sure we can write to the file
      if (!is_null($this->mode) && !($this->mode & 0200) && provision_file()->exists($filename)->status()) {
        provision_file()->chmod($filename, $this->mode | 0200)
            ->succeed('Changed permissions of @path to @perm')
            ->fail('Could not change permissions of @path to @perm');
      }

      $status = provision_file()->file_put_contents($filename, $this->getYmlDump())
          ->succeed('Generated Drupal Console aliases: ' . (empty($this->description) ? $filename : $this->description . ' (' . $filename. ')'), 'success')
          ->fail('Could not generate Drupal Console aliases: ' . (empty($this->description) ? $filename : $this->description . ' (' . $filename. ')'))->status();

      // Change the permissions of the file if needed
      if (!is_null($this->mode)) {
        provision_file()->chmod($filename, $this->mode)
            ->succeed('Changed permissions of @path to @perm')
            ->fail('Could not change permissions of @path to @perm');
      }
      if (!is_null($this->group)) {
        provision_file()->chgrp($filename, $this->group)
            ->succeed('Change group ownership of @path to @gid')
            ->fail('Could not change group ownership of @path to @gid');
      }
    }
    return $status;
  }
}
