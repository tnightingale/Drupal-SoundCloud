<?php


/**
 * Save a set of data.
 */
function dsc_save($data) {
	_p($data);
	$type = $data['type'];
	$uid = $data['uid'];
	
	switch ($type) {
		case 'track':
			foreach ($data->track as &$track) {
				$track->tid = $track->id;
				$track->uid = $uid;
				//drupal_write_record('dsc_tracks', $track);
			}
			break;
		
		default:
			return FALSE;
			break;
	}
}