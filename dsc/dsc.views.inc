<?php
// $Id$
/**
 * @file
 * Hooks required for module's Views integration.
 */

/**
 * Implementation of hook_views_data().
 */
function dsc_views_data() {
  $data['dsc_user'] = array(
    'table' => array(
      'group' => t('DSC'),
      // Advertise this table as a possible base table
      'base' => array(
        'field' => 'uid',
        'title' => t('SoundCloud User Table'),
        'help' => t("Stores User's SoundCloud user account information."),
      ),
      'join' => array(
        'users' => array(
          'left_field' => 'uid',
          'field' => 'uid',
        ),
      ),
    ),
    'uid' => array(
      'title' => t('User ID'),
      'help' => t('The ID of the Drupal User.'),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'permalink' => array(
      'title' => t('SoundCloud Permalink'),
      'help' => t("The unique 'machine name' of the SoundCloud user account."),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'sc_user_id' => array(
      'title' => t('SoundCloud User ID'),
      'help' => t('The ID of the SoundCloud user account.'),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
  );
  
  $data['dsc_tracks'] = array(
    'table' => array(
      'group' => t('DSC Tracks'),
      'join' => array(
        'users' => array(
          'left_table' => 'dsc_user',
          'left_field' => 'sc_user_id',
          'field' => 'sc_user_id',
        ),
      ),
    ),
    'tid' => array(
      'title' => t('Sound Cloud Track ID'),
      'help' => t('The ID of a SoundCloud Track.'),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'sc_user_id' => array(
      'title' => t('SoundCloud User ID'),
      'help' => t('The ID of a SoundCloud user account.'),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'title' => array(
      'title' => t("Track Title"),
      'help' => t("The title of a Track."),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
    'permalink_url' => array(
      'title' => t('Track Permalink'),
      'help' => t('A URL for a track\'s SoundCloud page.'),
      'field' => array(
        'handler' => 'views_handler_field',
      ),
    ),
  );
  
  return $data;
}

/**
 * Implementation of hook_views_plugins().
 */
function dsc_views_plugins() {  
  $plugins = array(
    'module' => 'dsc',
    'style' => array(
      'dsc_player' => array(
        'title' => t('SoundCloud Player'),
        'help' => t('Renders a list of SoundCloud tracks in a single player.'),
        'handler' => 'dsc_player_plugin_style',
        'theme' => 'views_view_dsc_player',
        'uses fields' => TRUE,
        'uses options' => TRUE,
        'type' => 'normal',
      ),
    ),
  );
  
  return $plugins;
}

/**
 * Implementation of hook_views_handlers().
 */
/*
function dsc_views_handlers() {
  return array(
    'handlers' => array(
      'handlername' => array(
        'parent' => 'views_handler_field',
        'file' => 'dsc_views_field_handlers.inc',
      ),
    ),
  );
}
*/