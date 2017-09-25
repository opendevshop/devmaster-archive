core = 7.x
api = 2

projects[drupal][type] = "core"

# This is not used when building the distribution. See https://github.com/opendevshop/devshop/blob/1.x/build-devmaster.make
projects[drupal][version] = "7.54"

; Includes
includes[devmaster] = drupal-org.make

defaults[projects][subdir] = "contrib"
defaults[projects][type] = "module"
