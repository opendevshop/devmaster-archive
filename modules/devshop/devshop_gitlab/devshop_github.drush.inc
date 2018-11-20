<?php

// Include vendors
require_once 'vendor/autoload.php';

/**
 * Implements hook_hosting_task_update_status().
 */
function devshop_gitlab_hosting_task_update_status($task, $status) {

  // React only on certain task types and if project and environment exist.
  $types = array('install', 'test', 'devshop-deploy');
  if (in_array($task->task_type, $types) && isset($task->ref->project) && !empty($task->ref->project) && isset($task->ref->environment) && !empty($task->ref->environment)) {
    $project = $task->ref->project;
    $environment = $task->ref->environment;
  }
  else {
    return;
  }

  // If a pull request object is available...
  if (isset($environment->gitlab_pull_request->pull_request_object->deployment)) {

    // If project is configured to reinstall every time, only react on "install" tasks. Otherwise, we get two gitlab deployments because both a "deploy" (git pull) and a "install" task are run on each git push.
    if ($project->settings->gitlab['pull_request_reinstall'] && $task->task_type == 'devshop-deploy') {
      return;
    }

    // Create a deployment status
    $owner = $project->gitlab_owner;
    $repo = $project->gitlab_repo;
    $deployment_id = $environment->gitlab_pull_request->pull_request_object->deployment->id;

    try {
      $token = variable_get('devshop_gitlab_token', '');
      $client = new \Gitlab\Client();
      $client->authenticate($token, Gitlab\Client::AUTH_HTTP_TOKEN);

      $params = new stdClass();
      if ($status == HOSTING_TASK_SUCCESS || $status == HOSTING_TASK_WARNING) {
        $params->state = $state = 'success';
      }
      else {
        $params->state = $state = 'failure';
      }

      // If task is a test run, only submit a commit status for devshop/tests context.
      if ($task->task_type == 'test') {
        $sha = $environment->gitlab_pull_request->pull_request_object->head->sha;

        $params = new stdClass();
        $params->state = $state;
        $params->target_url = url("node/{$task->nid}/view", array('absolute' => TRUE));

        if ($status == HOSTING_TASK_WARNING) {
          $params->description = t('DevShop: Tests passed with warnings');
        }
        else {
          $params->description = t('DevShop: Tests !status!', array('!status' => $state));
        }
        $params->context = 'devshop/tests';

        $status = $client->getHttpClient()->post("/repos/$owner/$repo/statuses/$sha", json_encode($params));
        drush_log('Commit status created for devshop/tests!', 'success');
      }
      // Otherwise we create a deployment and a commit status.
      else {

      $params->target_url = $environment->url;
      $params->description = t('Visit !url', array('!url' => $task->ref->environment->url));
      $post_url = "/repos/$owner/$repo/deployments/{$deployment_id}/statuses";

      drush_log('Attempting to create gitlab deployment status: ' . $post_url, 'success');

      $deployment_status = $client->getHttpClient()->post($post_url, json_encode($params));
      drush_log('Deployment status created!', 'success');


      // Update Status API

      // Create a status
      $sha = $environment->gitlab_pull_request->pull_request_object->head->sha;

      $params = new stdClass();
      $params->state = $state;
      $params->target_url = url("node/{$task->nid}", array('absolute' => TRUE));;

      if ($status == HOSTING_TASK_WARNING) {
        $params->description = t('DevShop: Deploy success with warnings. [!url]', array(
          '!url' => $environment->url,
        ));
      }
      else {
        $params->description = t('DevShop: Deploy !status [!url]', array(
          '!status' => $state,
          '!url' => $environment->url,
        ));
      }
      $params->context = 'devshop/deploy';

      $deployment_status = $client->getHttpClient()->post("/repos/$owner/$repo/statuses/$sha", json_encode($params));
      drush_log('Commit status created!', 'success');

        // If deploy task fails, tests won't run.
        if ($environment->settings->deploy['test'] && $status == HOSTING_TASK_ERROR) {

          $params = new stdClass();
          $params->state = $state;
            $params->description = t('DevShop: Tests not run due to Deploy Fail');
          $params->context = 'devshop/tests';

          $deployment_status = $client->getHttpClient()->post("/repos/$owner/$repo/statuses/$sha", json_encode($params));
          drush_log('Commit status created for devshop/tests', 'success');
        }
      }
    } catch (Gitlab\Exception\RuntimeException $e) {
      drush_log('GitLab API Error: ' . $e->getMessage(), 'error');
    }
  }
}
