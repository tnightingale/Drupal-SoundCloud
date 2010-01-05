<?php
// $Id$
/**
 * @file
 * Custom views style plgin.
 * Renders views results with the Drupal SoundCloud Player.
 */

/**
 * dsc_player_plugin_style
 */
class dsc_player_plugin_style extends views_plugin_style {
  function init(&$view, &$display, $options = NULL) {
    parent::init(&$view, &$display, $options);
    
    // Adding SoundCloud player code.
    if (module_exists('dsc_player')) {
      dsc_player_load_player();
    }
  }
  
  // TODO: Add code to ensure that the $track->permalink_url field is included in this view.
}