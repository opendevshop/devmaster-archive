<?php
/**
 * @file deploy.inc
 * Functions related to the "Deploy" task.
 */

/**
 * Implementation of hook_hosting_task_TASK_TYPE_form().
 *
 * For "Deploy" task.
 *
 * @see drush_devshop_projects_pre_hosting_task()
 */
function hosting_task_devshop_rebuild_form($node) {

  $form = array();

  $project = $node->project;

  $environments = array_keys($project->environments);
  $environment_options = array_combine($environments, $environments);

//  $project->settings->live

  $form['source'] = array(
    '#title' => t('Rebuild from'),
    '#type' => 'radios',
    '#options' => $environment_options,
  );

  return $form;
}
