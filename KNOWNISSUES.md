# KNOWN ISSUES

- LACK OF UNICODE SUPPORT - difficulties due to IniReader and the game itself, in short, this means no special characters in any string (including paths)

- game gets delayed upon connecting to an online stream -- needs a separate thread for playback startup to avoid
- rare sudden volume changes
- disconnect upon long pauses of online streams - this is in fact normal behavior as the buffer runs out
- some random unpausing between menu changing in pause menu - game's own bug (MW only)
- sometimes it's super loud when changing ingame volume when increasing from 0 to 1 bar - 
this is because it switches from playing ambiance to playing regular music, BASS library has no time to start before game tells it to go low volume

- no low pass filter (during SpeedBreaker) - not a big deal IMO :)
- multi line SHOUTcast metadata is a a bit glitchy (overlapping text)

as of 1.8:
- changing audio device during playback doesn't update BASS device until it's reinitialized, while the game audio does get updated
- sample rate update during interactive music happens before the previous part ends - currently unavoidable without writing some slow code...
- Carbon - can occasionally keep playing same parts in Canyon races, restart fixes it
- Carbon - Canyon music type override doesn't stick in Stage 2 during boss races
- when setting music or master volume to 0 with the game, music channel can still be faintly heard
- Carbon - larger files can cause slight hitches during decoding (noticeable only during switching or interactive music) - for this reason stick to OGGs