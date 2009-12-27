<?php
// $Id$
/**
 * @file
 * Contains Drupal menu page callback functions.
 */

/**
 * Temporary page callback for testing.
 */
function _dsc_page() {
  return 'hello';
}

function user_music($account) {
  drupal_set_title(check_plain($account->name) . "'s Music");
  
  $scuser = SoundCloudUser::getUser($account->uid);
  if ($scuser) {
    $tracks = $scuser->getTracks();
    return theme('list_tracks', $tracks, TRUE);
  }
  else {
    $link = l(t('here'), "user/{$account->uid}/edit/music");
    drupal_set_message(t("You need to set a SoundCloud account. You can do that !here", array('!here' => $link)), 'error');
    return t('No Music.');
  }
  
}

/**
 * User settings page
 */
function dsc_user_settings_form($form_state, $account) {
  drupal_set_title(check_plain($account->name));
  
  // Prepare current values if they have been set.
  if ($scuser = SoundCloudUser::getUser($account->uid)) {
    $permalink = $scuser->permalink;
    $import = $scuser->import;
  }
  
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $account->uid,
  );
  
  $form['permalink'] = array(
    '#type' => 'textfield',
    '#title' => t('Your SoundCloud url'),
    '#field_prefix' => 'http://soundcloud.com/',
    '#default_value' => $permalink,
    '#required' => TRUE,
  );
  
  // TODO: Think about this import thing a bit more.
  /*
  $form['import'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cache SoundCloud Data (Required for advanced features).'),
    '#default_value' => $import,
  );
  */
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  
  return $form;
}

function dsc_user_settings_form_validate($form, &$form_state) {
  $uid = $form_state['values']['uid'];
  
  $permalink = $form_state['values']['permalink'];
  $request_user = "users/$permalink";
  
  if ($response = SoundCloudUser::request($request_user, $uid, FALSE)) {
    // SoundCloudData::request() returns an array. We are expecting only one result so POP!.
    $form_state['storage']['scuser'] = array_pop($response);
  }
  else {
    form_set_error('permalink', t('The url you entered is not a SoundCloud account.'));
  }
}

function dsc_user_settings_form_submit($form, &$form_state) {
  // If updating SoundCloud user account, make sure to clear old cached data.
  if ($current = SoundCloudUser::getUser($form_state['values']['uid'])) {
    $tracks = $current->getTracks(FALSE);
    if (!empty($tracks)) {
      SoundCloudTrack::flush($tracks);
    }
  }
  
  $scuser = $form_state['storage']['scuser'];
  
  // Forcing import to TRUE for the time being.
  //$scuser->import = $form_state['values']['import'];
  $scuser->import = TRUE;
  
  $scuser->save();
  
  // Importing tracks;
  if ($scuser->import) {
    $scuser->getTracks(TRUE);
  }
}