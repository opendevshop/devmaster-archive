<?php
/**
 * @file deploy.devshop.provision.inc
 * Contains code for the provision-devshop-deploy command.
 */

/**
 * Pre provision-devshop-deploy
 */
function drush_devshop_provision_pre_provision_devshop_rebuild($source_alias){

  if (d()->type == 'site') {
    if (empty(d()->environment)) {
      return drush_set_error('DEVSHOP_FRAMEWORK_ERROR', 'This site is not a part of a project. You cannot use this command.');
    }
  }
  else {
    return drush_set_error('DEVSHOP_FRAMEWORK_ERROR', 'provision-devshop-rebuild must be run on a site context.');
  }

  $project_alias = '@project_' . d()->project;
  $project = (object) d($project_alias)->project;

  //. @TODO: Confirm that the source alias exists.

}

/**
 * Implements the provision-devshop-deploy command.
 */
function drush_devshop_provision_provision_devshop_rebuild($source_alias)
{
  $project_name = d()->project;
  $project = (object) d("@project_{$project_name}")->project;
  $environment = (object) $project->environments[d()->environment];

  // @TODO: Drop the target database and sync from the source alias.
  // @TODO: Delete the target files and sync from the source alias.
}

/**
 * Post provision-devshop-deploy
 */
function drush_devshop_provision_post_provision_devshop_rebuild($source_alias) {
  drush_devshop_provision_post_provision_devshop_deploy($source_alias);
}

