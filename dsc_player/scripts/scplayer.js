(function($) {
	$.fn.scplayer = function() {
		
		if ($(this).length >= 1) {
			// If there are lists that match the selector, create players!!!
			return this.each(function() {
				var player = this;
				var list = $('li', this);
				
				var state = 'stopped';
				var loadedTrack = null;
				var current = 0;
				
				var buttons = new Buttons;
				var progressBar = new ProgressBar;
				var display = new Display;
				var playlist = new Playlist(list);
				
				// Making player themable.
				var themePieces = {
					player: player,
					buttons: buttons.theme,
					progressBar: progressBar.theme,
					display: display.theme,
					playlist: playlist.theme
				}
				Drupal.theme('dsc_player_player', themePieces);
				
				// Hiding until at least one track is loaded.
				$(player).hide();
				
				// Function: loadTrack(trackNumber)
				this.loadTrack = function (trackNumber) {
					if (state != 'stopped') {
            this.stop();
					}
					var track = playlist.getTrack(trackNumber);
					var options = getSoundOptions(track);
					loadedTrack = soundManager.createSound(options);
					current = trackNumber;
					
					// Track doesnt seem to get duration right away.
					// Fortunately we have it at hand.
					//this.loadedTrack.durationEstimate = data.duration;
					loadedTrack.durationEstimate = track.duration;
					player.setPosition(0);
					
					// Updating loaded bar on track load/change.
					// TODO: I don't think this is working properly.
					progressBar.updateLIndicator((loadedTrack.bytesLoaded/loadedTrack.bytesTotal) * 100);
					
					display.updateDisplay();
				}
				
				// Function: togglePlay(trackNumber)
				this.togglePlay = function (trackNumber) {
					if (trackNumber == null) {
            trackNumber = current;
					}
					switch (state) {
						case 'playing':
							player.pause();
							break;
						case 'paused':
							player.play(trackNumber);
							break;
						case 'stopped':
							player.play(trackNumber);
							break;
					}
					buttons.playpause.toggle(state);
				}
				
				// Function: play(trackNumber)
				this.play = function(trackNumber) {
					if (loadedTrack && state != 'playing') {
						loadedTrack.play();
						
						var currentItem = $(list.get(trackNumber));
						currentItem.removeClass(state);
						state = 'playing';
						currentItem.addClass(state);
					}
				}
				
				// Function: pause();
				this.pause = function() {
					if (state == 'playing') {
						loadedTrack.pause();
						
						var currentItem = $(list.get(current));
						currentItem.removeClass(state);
						state = 'paused';
						currentItem.addClass(state);
					}
				}
				
				// Function: stop()
				this.stop = function() {
					if (state == 'playing') {
						loadedTrack.stop();
					}
					var pos = 0;
					loadedTrack.setPosition(pos);
					
					$(list.get(current)).removeClass(state);
					state = 'stopped';
					
					buttons.playpause.toggle();
					this.setPosition(pos);
				}
				
				// Function: next()
				this.next = function () {
					if (loadedTrack && current < playlist.length()) {
						var next = current + 1;
						this.loadTrack(next);
						this.togglePlay(next);
					}
				}
				
				// Function: previous()
				this.previous = function () {
					if (loadedTrack && current > 0) {
						var next = current - 1;
						this.loadTrack(next);
						this.togglePlay(next);
					}
				}
				
				// Function: setPosition(percent) - Sets track's current position in song.
				this.setPosition = function (percent) {
					var track = loadedTrack;
					// If stopping, need to specifically declare position as 0 as soundManager retains position in track.
					var track_position = (percent == 0) ? 0 : track.position;
					
					// Updating visual elements.
					progressBar.updatePIndicator(percent * 100);
					display.setPosition(formatMs(track_position));
					display.setDuration(formatMs(track.durationEstimate));
				}
				
				/**********************************************
				 * Utility funcitons
				 **********************************************/
				
				/**
				 * Creates a Sound Manager sound configuration resource.
				 */
				function getSoundOptions(sound) {
					var options = {
						id: sound.permalink,
						url: sound.stream_url,
						whileloading: function () {
							var track = loadedTrack;
							var percent = (track.bytesLoaded/track.bytesTotal);
							progressBar.updateLIndicator(percent * 100);
						},
						whileplaying: function () {
							var track = loadedTrack;
							var percent = (track.position/track.durationEstimate);
							player.setPosition(percent);
						},
						onfinish: function() {
							player.next();
						}
					}
					return options;
				}
				
				/**
				 * Format milliseconds into MM.SS
				 */
				function formatMs(ms) {
					var s = Math.floor((ms/1000) % 60);
					if (s < 10) { s = "0"+s; }
					return Math.floor(ms/60000) + "." + s;
				};
				
				/**********************************************
				 * "Classes"
				 **********************************************/
				
				/*
				 Buttons.
				*/
				function Buttons() {
					var buttonPanel = Drupal.theme('dsc_player_buttons_buttonPanel'); //$('<div>').addClass('buttons');
					
					var theme = {
						buttonPanel: buttonPanel
					}
					
					this.theme = theme;
					
					// Play/Pause
					this.playpause = new function() {
						var button_play = new Button('play');
						var button_pause = new Button('pause');
						button_pause.hide();
						
						theme.play = button_play;
						theme.pause = button_pause;
						
						this.toggle = toggle;
						
						button_play.bind('click', function() {
							player.togglePlay(current);
							return false;
						});
						button_pause.bind('click', function() {
							player.togglePlay(current);
							return false;
						});
						
						function toggle(state) {
							if (!state) {
                state = player.state;
							}
							switch(state) {
								case 'playing':
									button_play.hide();
									button_pause.show();
									break;
								default:
									button_pause.hide();
									button_play.show();
									break;
							}
						}
					}
					
					// Stop
					this.stop = new function() {
						var button = new Button('stop');
						theme.stop = button;
						
						button.bind('click', function () {
							player.stop();
							return false;
						});
					}
					
					// Next
					this.next = new function() {
						var button = new Button('next');
						theme.next = button;
						
						button.bind('click', function () {
							player.next();
							return false;
						});
					}
					
					// Previous
					this.previous = new function() {
						var button = new Button('previous');
						theme.previous = button;
						
						button.bind('click', function () {
							player.previous();
							return false;
						});
					}
					
					function Button(text, classname) {
						if (!classname) {
              classname = text;
						}
						
						var button = Drupal.theme('dsc_player_buttons_button', text, classname); //$('<a>').attr('href', '#').text(text).addClass(classname);
						
						return button;
					}
				}
				
				/*
				 Progress Bar.
				*/
				function ProgressBar() {
					var overallBar = new OverallBar;
					var progressIndicator = new ProgressIndicator;
					var loadedIndicator = new LoadedIndicator;
					
					this.updatePIndicator = updatePIndicator;
					this.updateLIndicator = updateLIndicator;
					
					this.theme = {
						overallBar: overallBar,
						progressIndicator: progressIndicator,
						loadedIndicator: loadedIndicator
					}
					
					// Adjust progress bar.
					function updatePIndicator(percent) {
						progressIndicator.css('width', percent + '%');
					};
					
					// Adjust loading bar.
					function updateLIndicator(percent) {
						loadedIndicator.css('width', percent + '%');
					}
					
					function OverallBar() {
						var bar = Drupal.theme('dsc_player_progressBar_overallBar'); //$('<div>').addClass('progress-bar');
						
						// Adding scrubbing feature.
						bar.click(function(event) {
							if (loadedTrack) {
								var track = loadedTrack;
								var percent = (event.clientX - $(this).offset().left) / ($(this).width());
								if (track.durationEstimate * percent < track.duration) {
									track.setPosition(track.durationEstimate * percent);
									player.setPosition(percent);
								}
							} else {
								alert('No track loaded.');
							}
						});
						
						return bar;
					}
					
					function ProgressIndicator() {
						var bar = Drupal.theme('dsc_player_progressBar_progressIndicator'); //$('<div>').addClass('progress');
						// Starting width is hardcoded as it is functionality.
						bar.css('width', '0%');
						
						return bar;
					}
					
					function LoadedIndicator() {
						var bar = Drupal.theme('dsc_player_progressBar_loadedIndicator') //$('<div>').addClass('loaded');
						// Starting width is hardcoded as it is functionality.
						bar.css('width', '0%');
						
						return bar;
					}
				}
				
				/*
				 Playlist.
				*/
				function Playlist(list) {
					var tracks = Array;
					var numberTracks = list.length;
					
					this.addTrack = addTrack;
					this.length = length;
					this.getTrack = getTracks;
					this.getTracks = getTracks;
					
					list.parent().remove();
					this.theme = {
						list: list.parent()
					}
					
					list.each(function (trackNumber) {
						// Hiding the track until loaded.
						$(this).hide();
						
						// Calling SoundCloud.
						$.getJSON("http://api.soundcloud.com/resolve?url=" + encodeURIComponent($('a', this).attr("href")) + "&format=js&callback=?", function(data) {
							addTrack(trackNumber, data);
						});
						
						$('a', this).bind('click', function () {
							// This check makes sure we are ready to stream.
							if (soundManager.swfLoaded) {
								player.loadTrack(trackNumber);
								player.togglePlay(trackNumber);
								current = trackNumber;
							}
							return false;
						});
					});
					
					function getTracks(trackNumber) {
						if (trackNumber == null) {
              return tracks;
						}
						else {
              return tracks[trackNumber];
						}
					}
					
					function length() {
						return numberTracks;
					}
					
					function addTrack(trackNumber, data) {
						tracks[trackNumber] = data;
						var track = $(list.get(trackNumber));
						
						plTrackText = Drupal.theme('dsc_player_playlist_plTrackText', data);
						
						$('a', track).text(plTrackText);
						track.show();
						
						// Load the first track.
						if (loadedTrack == null) {
							player.loadTrack(trackNumber);
							$(player).show();
						}
					}
				}
				
				/*
				 Player info display.
				*/
				function Display() {
					var hud = Drupal.theme('dsc_player_display_hud'); //.prependTo(player);
					var position = Drupal.theme('dsc_player_display_position'); //.appendTo(hud);
					var duration = Drupal.theme('dsc_player_display_duration'); //.appendTo(hud);				
					var trackTitle = Drupal.theme('dsc_player_display_trackTitle'); //.appendTo(hud);
					var trackArtist = Drupal.theme('dsc_player_display_trackArtist'); //.appendTo(hud);
					
					this.theme = {
						hud: hud,
						position: position,
						duration: duration,
						trackTitle: trackTitle,
						trackArtist: trackArtist
					}
					
					this.setPosition = setPosition;
					this.setDuration = setDuration;
					this.updateDisplay = updateDisplay;
					
					function setPosition(value) {
						position.text(value);
					}
					
					function setDuration(value) {
						duration.text(value);
					}
					
					function updateDisplay() {
						var track = loadedTrack;
						track.playerData = playlist.getTrack(current);
						
						trackTitle.text(track.playerData.title);
						trackArtist.text(track.playerData.user.username);
					}
				}
			});
			
		} else {
			// No matching lists.
			return false;
		}
	}
})(jQuery);

/**
 * Drupal theme functions (Garland).
 */
Drupal.theme.prototype.dsc_player_player = function(p) {
	//_p(p);
	
	// Progress bar
	p.progressBar.progressIndicator.appendTo(p.progressBar.overallBar);
	p.progressBar.loadedIndicator.appendTo(p.progressBar.overallBar);

	// Buttons
	p.buttons.previous.appendTo(p.buttons.buttonPanel);
	p.buttons.play.appendTo(p.buttons.buttonPanel);
	p.buttons.pause.appendTo(p.buttons.buttonPanel);
	p.buttons.stop.appendTo(p.buttons.buttonPanel);
	p.buttons.next.appendTo(p.buttons.buttonPanel);
	
	// Track Title & Artist
	var meta = $('<div class="track-meta">').appendTo(p.display.hud);
	meta.append(p.display.trackTitle);
	meta.append(' | ');
	meta.append(p.display.trackArtist);
	
	// Track Duration
	var time = $('<span class="track-time">').appendTo(p.display.hud);
	time.append(p.display.position);
	time.append(' : ');
	time.append(p.display.duration);
	
	// Putting it all together.
	p.buttons.buttonPanel.appendTo(p.player);
	
	p.progressBar.overallBar.appendTo(p.player);
	p.display.hud.appendTo(p.progressBar.overallBar);
	
	p.playlist.list.appendTo(p.player);
}

/**
 * Button elements.
 */
Drupal.theme.prototype.dsc_player_buttons_buttonPanel = function() {
	return $('<div>').addClass('buttons');
}
Drupal.theme.prototype.dsc_player_buttons_button = function(text, classname) {
	var button = $('<a>').attr('href', '#').text(text);
	button.addClass(classname);
	button.addClass('button');
	button.text('');
	
	return button;
}

/**
 * Playlist elements.
 */
Drupal.theme.prototype.dsc_player_playlist_plTrackText = function(trackData) {
	return trackData.title + ' | ' + trackData.user.username;
}

/**
 * Progress elements.
 */
Drupal.theme.prototype.dsc_player_progressBar_overallBar = function() {
	bar = $('<div>').addClass('overall');
	bar.css('width' , '500px');
	bar.css('background-color' , '#58B3EC');
	
	return bar;
}

Drupal.theme.prototype.dsc_player_progressBar_progressIndicator = function() {
	bar = $('<div>').addClass('progress');
	bar.css('height', '4em');
	bar.css('background-color', '#2588C4');
	
	return bar;
}
Drupal.theme.prototype.dsc_player_progressBar_loadedIndicator = function() {
	bar = $('<div>').addClass('loaded');
	bar.css('height', '0.3em');
	bar.css('background-color', '#2588C4');
	
	return bar;
}

/**
 * Display elements.
 */
Drupal.theme.prototype.dsc_player_display_hud = function() {
	return $('<div>').addClass('display');
}
Drupal.theme.prototype.dsc_player_display_position = function() {
	return $('<span>').addClass('position');
}
Drupal.theme.prototype.dsc_player_display_duration = function() {
	return $('<span>').addClass('duration');
}
Drupal.theme.prototype.dsc_player_display_trackTitle = function() {
	return $('<span>').addClass('title');
}
Drupal.theme.prototype.dsc_player_display_trackArtist = function() {
	return $('<span>').addClass('artist');
}

/**
 * I'm a lazy typer...
 */
var _p = function(obj, debug) {
	window.console.log(obj);
}