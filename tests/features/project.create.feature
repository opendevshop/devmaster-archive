@api
Feature: Create a project
  In order to start developing a drupal site
  As a project admin
  I need to create a new project

  Scenario Outline: Create a new project
    Given users:
      | name     | mail              | status | roles          |
      | devshop  | no@getdevshop.com | 1      | administrator  |

    Examples:
    |name    |url                                                  |docroot|makefile   |
    |drpl8   |http://github.com/opendevshop/drupal8.git            |       |           |
    |made    |https://github.com/opendevshop/example-drush-make.git|built  |drupal.make|
    |docroot |https://github.com/opendevshop/drupal_docroot.git    |docroot|           |

    Given I am logged in as "devshop"
    And I am on the homepage
    When I click "Projects"
    And I click "Start a new Project"
    Then I should see "Step 1"
    Then I fill in "<name>" for "Project Code Name"
    And I fill in "<url>" for "Git URL"
    When I press "Next"

    # Step 2
    Then I should see "<name>"
    And I should see "<url>"
    When I fill in "<docroot>" for "Path to Drupal"

    # Step 3
    When I press "Next"
    Then I should see "Please wait while we connect to your repository and determine any branches."
#    And I should see "Path to Drupal: docroot"

    When I run drush "hosting-tasks -v --force --fork=0 --strict=0"
    Then print last drush output
    And I reload the page
    And I reload the page

    Then I should see "Create as many new environments as you would like."
    When I fill in "dev" for "project[environments][NEW][name]"
    And I select "master" from "project[environments][NEW][git_ref]"

    And I press "Add environment"
    And I fill in "live" for "project[environments][NEW][name]"
    And I select "master" from "project[environments][NEW][git_ref]"
    And I press "Add environment"
    Then I press "Next"

    # Step 4
    And I should see "dev"
    And I should see "live"
    And I should see "master"
    And I should see "master"

    Then I run drush "eval 'print_r(hosting_context_load(platform_<name>_dev))'"
    Then print last drush output
    When I run drush "hosting-tasks -v --debug --force --fork=0 --strict=0"
    Then print last drush output
    And I reload the page

    Then I should see "dev"
    And I should see "live"
    And I should see "master"

    And I should see "master"
    And I reload the page
#    When I click "Process Failed"
#    Then I should see "8."
    Then I should not see "Platform verification failed"
    When I select "standard" from "install_profile"

#    Then I break

    And I press "Create Project & Environments"

    # FINISH!
    Then I should see "Your project has been created. Your sites are being installed."
    And I should see "Dashboard"
    And I should see "Settings"
    And I should see "Logs"
    And I should see "standard"
#    And I should see "http://github.com/opendevshop/drupal"
    And I should see the link "dev"
    And I should see the link "live"

#    Then I break
    And I should see the link "http://<name>.dev.devshop.local.computer"
    And I should see the link "Aegir Site"

    When I run drush "hosting-tasks -v --force --fork=0 --strict=0"
    Then print last drush output
    Then drush output should not contain "This task is already running, use --force"

    And I reload the page
    Then I should see the link "dev"
    Then I should see the link "live"

    When I click "Create New Environment"
    And I fill in "testenv" for "Environment Name"
    And I select "master" from "Branch or Tag"
    And I select the radio button "Drupal Profile"
    Then I select the radio button "Standard Install with commonly used features pre-configured."

    #@TODO: Check lots of settings

    Then I press "Create New Environment"
    Then I should see "Environment testenv created in project <name>."

    When I run drush "hosting-tasks --force --fork=0 --strict=0"
    Then print last drush output
    When I run drush "hosting-tasks --force --fork=0 --strict=0"
    Then print last drush output
    When I run drush "hosting-tasks --force --fork=0 --strict=0"
    Then print last drush output

    When I click "testenv" in the "main" region
    Then I should see "Environment Dashboard"
    And I should see "Environment Settings"

    When I click "Visit Site"
    Then I should see "Welcome to <name>.testenv"

    Then I move backward one page
    When I click "Project Settings"
    Then I select "testenv" from "Primary Environment"
    And I press "Save"

    Then I should see "DevShop Project <name> has been updated."
    And I should see an ".environment-link .fa-bolt" element