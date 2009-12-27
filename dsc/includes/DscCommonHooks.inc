<?php
// $Id$
/**
 * @file
 * Contains all module's Drupal hooks that are not used regularly.
 */
/**
 * Class used to collect hook implementations that doesn't need to be included
 * on every page load.
 */
class DscCommonHooks {
  /**
   * Implementation of hook_menu().
   */
  public static function menu() {
    $menu = array();
    
    $menu['user/%user_category/edit/dsc_account'] = array(
      'title' => t('SoundCloud Account'),
      'page callback' => 'drupal_get_form',
      'page arguments' => array('dsc_user_settings_form', 1),
      'access' => 'administer DrupalSoundCloud',
      'type' => MENU_LOCAL_TASK,
      'load arguments' => array('%map', '%index'),
      'file' => 'dsc.pages.inc',
    );
    
    $menu['user/%user/music'] = array(
      'title' => t('Music'),
      'page callback' => 'user_music',
      'page arguments' => array(1),
      'access' => 'access user profiles',
      'type' => MENU_LOCAL_TASK,
      'file' => 'dsc.pages.inc',
    );
    
    return $menu;
  }
  
  /**
   * Implementation of hook_user().
   */
  public static function user($op, &$edit, &$account, $category = NULL) {
    switch ($op) {
      case 'categories':
        return array(
          array(
            'name' => 'dsc_account',
            'title' => t('SoundCloud Account'),
          ),
        );
        break;
    }
  }
  
  public static function theme() {
    return array(
      'list_tracks' => array(
        'arguments' => array('tracks', 'load_player'),
      ),
    );
  }
  
}