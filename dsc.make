core = 6.x

projects[] = drupal

projects[] = devel
projects[admin] = 2.0-alpha2
projects[tao][location] = http://code.developmentseed.org/fserver
projects[rubik][location] = http://code.developmentseed.org/fserver

projects[] = blueprint
projects[] = autoload
# This patch prevents autoload from breaking Drush
projects[autoload][patch][] = "http://drupal.org/files/issues/autoload-547736_0.patch" 
projects[] = views
projects[] = advanced_help
projects[] = soundmanager2

libraries[dsc][download][type] = "git" 
libraries[dsc][download][url] = "git://github.com/thegreat/Drupal-SoundCloud.git"
libraries[dsc][directory_name] = dsc
libraries[dsc][destination] = modules

libraries[soundmanager][download][type] = "get"
libraries[soundmanager][download][url] = "http://www.schillmania.com/projects/soundmanager2/download/soundmanagerv295a-20090717.zip"
libraries[soundmanager][directory_name] = soundmanagerv2
libraries[soundmanager][destination] = libraries

#libraries[blueprint][download][type] = "git"
#libraries[blueprint][download][url] = "git://github.com/joshuaclayton/blueprint-css.git"
#libraries[blueprint][download][tag] = "v0.9.1"
#libraries[blueprint][directory_name] = blueprint
#libraries[blueprint][destination] = themes/blueprint