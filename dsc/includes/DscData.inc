<?php
// $Id$
/**
 * @file
 * Defines SoundCloud object class definitions that interact with the SoundCloud
 * API.
 */
/**
 * Class used to manage data recieved from SoundCloud.
 */
abstract class SoundCloudData {
  private static $dsc_fields;
  
  /**
   * Constructors
   */
  function __construct($dsc_fields, $uid, $properties = array()) {    
    $this->uid = $uid;
    
    foreach ($dsc_fields as $dsc_fieldname => $prop_name) {
      $this->$dsc_fieldname = $properties->$prop_name;
    }
    
    if (!empty($properties)) {
      foreach ($properties as $property => $value) {
        $this->$property = $value;
      }
    }
    
    return $this;
  }
  
  static function _newFromObj($class, $obj, $uid, $import = TRUE) {
    
    if (is_null($uid)) return FALSE;
    
    if (is_array($obj)) $obj = (object)$obj;
    elseif (!is_object($obj)) return FALSE;
    
    $new = new $class($uid, $obj);
    if ($import) $new->save();
    
    return $new;
  }
  
  /**
   * Perform a SoundCloud api request.
   *
   * @param string $request - An api request string to be appended to the api url.
   * @param int $uid - A Drupal user's uid.
   * @param bool $import - Flag whether to cache data or not (Optional). Default: TRUE.
   *
   * @return Mixed - Ether returns an array containing requested data or FALSE if no data returned.
   */
  static function request($request, $uid, $import = TRUE) {
    if (is_object($uid)) $uid = $uid->uid;
    
    $class = get_called_class();
    
    if ($class == get_class()) return FALSE;
    
    $api_url = variable_get('soundcloud_api_url', 'http://api.soundcloud.com');
    $request = "$api_url/$request";
    
    // JSON Implementation
    $json_response = drupal_http_request($request, array('Content-Type' => 'application/json'));
    $json = json_decode($json_response->data);
    if (isset($json->error)) return FALSE;
    
    if (!is_array($json)) $json = array($json);
    
    //_p($json, "JSON | $class");
    foreach ($json as $data) {
      $requested_items[] = call_user_func(array($class, 'newFromObj'), $data, $uid, $import);
    }

    if (isset($requested_items)) return $requested_items;
    else return FALSE;
  }
  
  /**
   * Required Methods
   */
  abstract public static function newFromObj($obj, $uid, $import = TRUE);
  abstract protected function save();
}


/**
 * Class SoundCloudUser
 */
class SoundCloudUser extends SoundCloudData {
  private static $dsc_fields = array(
    'sc_user_id' => 'id',
    'sc_user_username' => 'username',
  );
  
  /**
   * Constructors
   */
  function __construct($uid, $properties) {
    parent::__construct(SoundCloudUser::$dsc_fields, $uid, $properties);
  }
  
  public static function newFromObj($obj, $uid, $import = TRUE) {
    return parent::_newFromObj(get_class(), $obj, $uid, $import);
  }
  
  /**
   * Getters
   */  
  public static function getUser($uid) {
    if (is_object($uid)) $uid = $uid->uid;
    
    $sc_user = db_fetch_object(db_query('SELECT * FROM {dsc_user} WHERE uid = %d', $uid));
    if (!$sc_user) return NULL;
    
    return SoundCloudUser::newFromObj($sc_user, $uid, FALSE);
  }
  
  /**
   * Gets user's tracks. Checks DB first. If no results it makes an api call.
   */
  public function getTracks($import = TRUE) {
    $results = db_query('SELECT * FROM {dsc_tracks} t WHERE t.sc_user_id = %d', $this->sc_user_id);
    
    while ($result = db_fetch_object($results)) {
      $tracks[] = SoundCloudTrack::newFromObj($result, $this->uid, FALSE);
    }
    
    if (empty($tracks) && $import) {
      $request = "users/{$this->permalink}/tracks";
      $tracks = SoundCloudTrack::request($request, $this->uid);
    }
    
    return $tracks;
  }
  
  /**
   * Saves user's SoundCloud account details.
   */
  public function save() {
    if (db_result(db_query('SELECT uid FROM {dsc_user} WHERE uid = %d', $this->uid))) {
      // User already has an account set, updating.
      drupal_write_record('dsc_user', $this, array('uid'));
      drupal_set_message(t('Account infomation updated.'));
    }
    else {
      // Adding new value.
      drupal_write_record('dsc_user', $this);
      drupal_set_message(t('Account information saved.'));
    }
  }
}


class SoundCloudTrack extends SoundCloudData {
  private static $dsc_fields = array(
    'tid' => 'id',
    'sc_user_id' => 'user_id',
  );
  public $visible = TRUE;
  
  /**
   * Constructors
   */
  function __construct($uid, $properties) {
    parent::__construct(SoundCloudTrack::$dsc_fields, $uid, $properties);
  }
  
  public static function newFromObj($obj, $uid, $import = TRUE) {
    return parent::_newFromObj(get_class(), $obj, $uid, $import);
  }
  
  public static function getTrack($tid) {
    $obj = db_fetch_object(db_query('SELECT * FROM {dsc_tracks} t WHERE t.tid = %d', $tid));
    $uid = db_result(db_query('SELECT u.uid FROM {dsc_user} u WHERE u.sc_user_id = %d', $obj->sc_user_id));
    
    return SoundCloudTrack::newFromObj($obj, $uid, FALSE);
  }
  
  private function _flush() {
    if (!db_query('DELETE FROM {dsc_tracks} WHERE tid=%d', $this->tid)) return FALSE;
    else return TRUE;
  }
  
  public static function flush($tracks) {
    $error = FALSE;
    foreach ($tracks as $track) {
      if (is_numeric($track)) $track = SoundCloudTrack::getTrack($track);
      elseif (!is_object($track)) $error = TRUE;
      
      if (!$track->_flush()) {
        $error = TRUE;
      }
    }
    
    if ($error) drupal_set_message(t("There was an error while flushing tracks."), 'error');
  }
  
  public function toggleVisibility() {
    $this->visible = ($this->visible) ? 0 : 1;
    $this->save();
  }
  
  /**
   * Saves SoundCloud track details.
   */
  public function save() {
    if (db_result(db_query('SELECT tid FROM {dsc_tracks} WHERE tid = %d', $this->tid))) {
      drupal_write_record('dsc_tracks', $this, array('tid'));
    }
    else {
      drupal_write_record('dsc_tracks', $this);
    }
  }
}

/********************************
 * Retro-support of get_called_class()
 * Tested and works in PHP 5.2.4
 * http://www.sol1.com.au/
 ********************************/
if (!function_exists('get_called_class')) {
  function get_called_class($bt = FALSE, $l = 1) {
    if (!$bt) $bt = debug_backtrace();
    if (!isset($bt[$l])) throw new Exception("Cannot find called class -> stack level too deep.");
    if (!isset($bt[$l]['type'])) {
      throw new Exception('type not set');
    }
    else switch ($bt[$l]['type']) {
      case '::':
        $lines = file($bt[$l]['file']);
        $i = 0;
        $callerLine = '';
        do {
          $i++;
          $callerLine = $lines[$bt[$l]['line']-$i] . $callerLine;
        } while (stripos($callerLine, $bt[$l]['function']) === FALSE);
          preg_match('/([a-zA-Z0-9\_]+)::' . $bt[$l]['function'] . '/', $callerLine, $matches);
        if (!isset($matches[1])) {
          // must be an edge case.
          throw new Exception("Could not find caller class: originating method call is obscured.");
        }
        switch ($matches[1]) {
          case 'self':
          case 'parent':
            return get_called_class($bt, $l+1);
          default:
            return $matches[1];
        }
        // won't get here.
      case '->': switch ($bt[$l]['function']) {
        case '__get':
          // edge case -> get class of calling object
          if (!is_object($bt[$l]['object'])) throw new Exception ("Edge case fail. __get called on non object.");
          return get_class($bt[$l]['object']);
        default: return $bt[$l]['class'];
      }

      default: throw new Exception("Unknown backtrace method type");
    }
  }
}