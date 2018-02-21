DevShop Pull
============

Provides a single webhook for each project.

This module now integrates with Hosting Git Pull.

Hosting Git Pull provides a unique webhook URL for each platform & site.  This would require a new webhook to be added to the git repository for each individual environment.

Instead, this module provides a webhook callback for the entire project, and then kicks off hosting_git_pull's code for the rest.

GitHub Setup
------------

1. Visit your repos page: http://github.com/YOURNAME/YOURREPO
2. Click "Settings".
3. Click "Service Hooks".
4. Click "WebHook URLs"
5. Copy and paste your project's Git Pull Trigger URL into the URL field of the
   WebHook URLs page.
6. Click "Test Hook" to run a test, then check your DevShop project to ensure a
   Pull Code task was triggered.
