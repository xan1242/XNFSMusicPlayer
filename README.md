# Xan's NFS Music Player

This is a custom music player replacement and a BASS library interface for NFS.

## FEATURES

- Custom playlist generation - put any number of songs you like!
- Basic support for M3U playlists!
- Interactive music playback - you're not missing out on these! You can even modify them to your liking! (requires a bit of skill)
- Online streaming - listen to online radio in the game!
- Automatic metadata reading
- SHOUTcast metadata support - a track changed on the radio? No problem, the Chyron will pop up when it happens!
- Music looping
- Per track settings - need to change the song's shown title or change the loop point? You can set that in a separate settings file!
- A LOT of formats supported:
  - BASS' natively supported streaming formats: MP3/MP2/MP1/OGG/WAV/AIFF
  - BASS' natively supported tracker formats (same as XMPlay): mo3/xm/mod/s3m/it/mtm/umx 
  - BASSFLAC: FLAC
  - BASSMIDI (only a single soundfont at a time for now :/)
- Game mastered audio - the game itself controls the music volume, just like its own music player

## INSTALLATION

1. Install Visual Studio 2015-2022 x86 Redist (from here: https://aka.ms/vs/17/release/vc_redist.x86.exe ) (SKIP IF ALREADY INSTALLED)
2. Extract this package to your game's root directory
3. Start the game and follow the instructions
4. Do either of following things:
   1. Make an M3U playlist with your favorite music player and save to scripts\XNFSMusicPlayer\Playlist.m3u
   2. Open scripts\XNFSMusicPlayer.ini and set PlaylistFile to the path of your M3U file!

NOTE: M3U must be NON-UNICODE! If your path contains Unicode letters, until Unicode paths are implemented, please use non-Unicode paths (don't use special characters).

## CONFIGURATION

The configuration consists of multiple files:

1. The main config: XNFSMusicPlayer.ini
2. Per track settings (default at scripts\XNFSMusicPlayer\PlaylistConfig.ini)
3. The main playlist (default at scripts\XNFSMusicPlayer\Playlist.m3u)
4. Node paths (default at scripts\XNFSMusicPlayer\NodePaths.txt) - auto generated upon first time setup
5. EventID overrides (default at scripts\XNFSMusicPlayer\Overrides.txt) - auto generated upon first time setup

### 1. MAIN CONFIG

In this file, you can set up various settings related to playback. Most of it is explained in the file itself.

If you have any trouble (hitching, delaying) during playback, this is the file to look at.

You can also enable the console log output by changing the VerbosityLevel in case of any errors.
DO NOT use this if you're on WinXP, it will cause crashes.

This will also cause a crash if you don't have a console attached to the game's window. Use ConsoleAttachment.asi included in the package.

Each of the other config file locations can be changed within this file as well. (in [Playlists])

Example: You can change the playlist file by changing PlaylistFile like this:

`[Playlists]`

`PlaylistFile = D:\MyMusicFolder\MyPlaylist.m3u`

If you wish to restart the first time setup, set `InstallerCompleted` to 0 (and optionally rename/remove the InteractiveMusic folder).

### 2. PER TRACK SETTINGS

This is the file that saves any settings that can be set on a single track.
Settings include:
- Looping settings
- Track title, artist and album (overrides existing metadata in file)
- Playback mode
- Tracker amplification level

To create an entry, simply go to the EA Trax settings in the game, and change the playback mode (All, Ingame, Menu). (press left/right)

The track's ID will be appended to the file.

An example configuration exists within the file itself already.

### 3. MAIN PLAYLIST FILE

In here you can place paths to your music files. You can use either a music player that can create M3U files.
It's simple - one line per file!

You can also comment a line with `#` infront of it. Inline comments are not yet supported.

### 4. NODE PATH FILE

This file contains a table of paths per each NodeID. (NodeID in hex = FilePath)
It is automatically generated during first time setup if you chose to use interactive music.

The contents of this file depends on the .mpf file (PathFinder definition file).

If you happen to use a different definition file in your game after first time setup, this file will become invalid.

If you wish to extract all of the files from the game, not just interactive specific music, set ScanForAllFiles to 1 in the main config file.

The installer can also generate this and the rest of the files with the PS2's MUS/MPF combo. In this case, be sure to enable ScanForAllFiles.

### 5. EVENTID OVERRIDE FILE

The format of this file is identical to the previous one, except this time the ID matches an EventID.

An EventID is what it says it is - a number to identify an event in the game.
These numbers are used throughout the game to trigger different music.

The music player is capable of generating its own EventIDs for each track, however there are exceptions;
As the title screen (and maybe others if you disabled interactive music) has no ties to EA Trax, the EventID must be overrided manually.

In this file you can define which EventID is tied to which file precisely, so if you're missing any, you can add them here.

## TODO list:

- add NFSC chyron handling and calling
- add NFSC music playback types like MW - IG, FE, AL, OF (instead of just on/off)
- Unicode
- installer direct process calling instead of batch scripting - this will allow for percentage overview in the dialog boxes
- sflist support
- track changing support for MW

## CREDITS

ThirteenAG - for the [Ultimate ASI Loader](https://github.com/ThirteenAG/Ultimate-ASI-Loader) and the injector and IniReader

[Un4seen Developments Ltd.](http://www.un4seen.com/) (Ian Luck) - BASS library, BASSMIDI and BASSFLAC addons

