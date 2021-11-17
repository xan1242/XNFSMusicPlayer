# CHANGELOG (before Git)

1.8:
-- split codebase to support multiple games
    - Carbon version
    - preparing for GitHub code release
-- Interactive music - continuous channel playback - no more lag between nodes.
-- updated the ASI working directory to be the same as the executable (except for IniReader)
-- Added installer music - makes your waiting time slightly less boring
-- streamlined installer paths (less dialogs overall)
-- DROPPED VGMSTREAM AND ASF - caused more trouble than it's worth
-- added infinite loop mode
	- in track's config, set Loop to -1 to use
-- added LOOP_START and LOOP_END tag reads (previously it was only LOOPSTART and LOOPEND)
-- FAST HASHING - switched EventID hashing to be based on filename rather than file contents (you can still enable the old mode by setting SlowHashing to 1 in the config)
-- MIDI - track specific soundfonts - name your sf2 file the same as your midi file and put them right next to each other
-- updated BASS to the latest available version - 2.4.16.7
-- updated BASSFLAC to 2.4.4.3
-- updated BASSMIDI to 2.4.13

1.7
-- removed fixed memory areas for tracks, added dynamic memory allocation
	- calloc's structures by the number of tracks
	- the only limits for tracks now should be amount of available memory and my code's optimization
-- decreased the delay between tracks a bit
	
1.6
-- attempt to force disable channel slide if a looped song was playing
	- interactive music could sometimes fade out between parts

1.5:
-- FIXED THE 120 TRACK LIMIT!
	- Player1's UserProfile database was being overwritten by track stuff so I disabled that
	- I'm not sure if this will introduce new bugs but it seems good so far
	- Only tested with 1331 tracks so far, there were no crashes
	- In theory, the limit should now be 65535 tracks before I implement dynamic allocation

1.4: 
-- prevented the OS from stopping the DLL to start
	- by returning TRUE even if an error occurs
	- special thanks to LW_X_M_Officer_01b for testing
-- fixed build number
-- added rudimentary logging to file
	 - set LogFileEnabled to 1 in the ini to use
-- removed comment from the default SF2 file path
	- strings don't have inline comment supported
-- hacked the hack previously made for infinite playback
	- disabled it for interactive mode
	- it previously caused game to randomly stop playing interactive music if it's ASF and kept queueing them
-- fixed some stupid issues with ID3v2 again

1.3:
-- added M3U drive detection for Winamp style paths
-- added more bootflows for the installer
-- hack to prevent infinite playback of some files with VGMStream (except looped)
-- fixed duplicate path creation

1.2:
-- fixed Vorbis comment reading
-- added M3U support
-- changed up some paths again for default files

1.1:
-- fixed crashing issues with ID3v2 tag reading
-- changed default paths to suck less
-- added warning to user for a lot of tracks
	- found out game has bugs with more than 120 or so tracks
-- added temporary Unicode to ASCII conversion before going to full Unicode (ID3v2 fixed)
	
1.0:
-- initial release