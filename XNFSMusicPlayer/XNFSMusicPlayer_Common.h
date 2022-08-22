// Common code for XNFSMusicPlayer
// TODO: further code splitting

#pragma once
#include "XNFSMusicPlayer.h"
#include "XNFSMusicPlayer_TagReader.h"
#include "XNFSMusicPlayer_StreamPusher.h"

void(__stdcall *EAXSound_Update)(float Something) = (void(__stdcall*)(float))EAXSOUNDUPDATE_ADDRESS;
unsigned int(__cdecl *PATHI_queuenode)(void* PATHTRACK) = (unsigned int(__cdecl*)(void*))QUEUENODE_ADDRESS;

// non-abstracted functions between games
void DoMeta(const char *meta);
int SearchPathByNodeID(unsigned int ID);
bool CheckIfInteractiveEventID(unsigned int ID);
void AllocateMemoryForTracks(unsigned int TrackCount);
int InjectCode();
int InitConfig_GameSpecific();
void SetNoTracksWarning();
int DoFilePathsStruct(char* TxtFile);
int HotkeyUpdate();
bool CheckIfFileExists(bool bSilent, const char* FileName);

// Some functions that need to be called.
unsigned int somefunction = SOMEFUNCTION1_ADDRESS;
unsigned int somefunction2 = SOMEFUNCTION2_ADDRESS;
unsigned int SNDSYS_leavecritical = SNDSYSLEAVECRITICAL_ADDRESS;
unsigned int PATH_clearallevents = CLEARALLEVENTS_ADDRESS;
unsigned int SFXObj_PFEATrax_NotifyChyron = SFXOBJNOTIFYCHYRON_ADDRESS;

bool bBASSCreated = 0;
//bool bBASSMIDIFontsLoaded = 0;
bool bPlayedAtLeastOnce = 0;
bool bPlayedInteractiveAtLeastOnce = 0;
bool bGameVolumeControl = 1;
bool bInteractiveMode = 0;
bool bStopNodeQueueing = 0;
bool bEnableInteractiveNoding = 1;
bool bIsVolumeSliding = 0;
bool bUseGameThread = 0;
bool bAsyncRead = 1;
bool bInstallerComplete = 1;
bool bSlowHashing = 0;
bool bInstallerMusic = 0;

//unsigned int CurrentPlayingFileType = 0;
unsigned int CurrentPlayingNode = 0;
unsigned int PauseChannel = 0;
unsigned int ResumeChannel = 0;
unsigned int GlobalVolume = 10000;
float GlobalTrackerAmp = 50.0;
unsigned int FoundIDNum = 0;
unsigned int SampleRate = 44100;
unsigned int BufferSize = 500;
int DevBufferSize = -1;
unsigned int AsyncBufferSize = 262144;
unsigned int UpdateLength = 500;
unsigned int NumOfThreads = 1;
unsigned int UpdatePeriod = 100;
unsigned int MusicID = 0;
int LoopCounter = 0;
QWORD CurrentTrackLoopStart = 0;
QWORD CurrentTrackLoopEnd = 0;
unsigned int FadeoutTime = 5000;
int PlayCounter = -1;
unsigned int TrackCount = 0;
volatile unsigned int OverrideCount = 0;
unsigned int MemCheck = 0;
unsigned int Flags = BASS_STREAM_AUTOFREE;
unsigned int PreviousEAX;
float VolMultiplier = 0.01;
float LastVolume = 0.22; // Adjusted to the very first volume the game sets (to reduce pops).
unsigned int hkTrackSwitch = VK_MEDIA_NEXT_TRACK;

DWORD music, oldhandle, PlaySync, LoopSync, MetaSync; // BASS' stream/music/sync handles.
BASS_CHANNELINFO ChanInfo;
BASS_INFO BassInfo;

char* PlaylistFile = DEFAULT_PLAYLIST_PATH;
char* PlaylistConfigFile = DEFAULT_PLAYLISTCONFIG_PATH;

char* OverridePathConfigFile = DEFAULT_OVERRIDES_PATH;
char* file = DEFAULT_FILE_PATH;

char* InstallMusicFile = INSTALL_MUSIC_FILE;

unsigned int BASSTimeRemainingCaveExit = TIMEREMAININGCAVEEXIT_ADDRESS;
unsigned int BASSVolumeCaveExit = VOLUMECAVEEXIT_ADDRESS;
unsigned int StartPursuitMusicCaveExit = STARTPURSUITMUSEXIT_ADDRESS;
unsigned int StartAmbienceCaveExit = STARTAMBIANCEEXIT_ADDRESS;

bool bOnlineChyron = 0;

PathfinderStruct NullPathfinder;
TrackStruct *track, *overrides;

char* SF2filename;

unsigned int QueueNodeInterceptor(void* PATHTRACK)
{
	if (bInteractiveMode)
		return PATHI_queuenode(PATHTRACK);
	return 0;
}

void __stdcall SoundUpdater(float Something)
{
	EAXSound_Update(Something);
	if (!UpdateLength)
		UpdateLength = (unsigned int)(Something * 1000);
	if (bBASSCreated)
		if (!BASS_Update(UpdateLength))
			XNFS_printf(1, "%s: Error updating BASS with game thread: BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	HotkeyUpdate();
}

void LoadFileInBuffer(void* Buffer, const char* FilePath, unsigned int FileSize)
{
	char PathBuffer[1024];
	strcpy(PathBuffer, FilePath);
	//FILE *fin = fopen(PathBuffer, "rb");
	HANDLE fin = CreateFileA(FilePath, GENERIC_READ, FILE_SHARE_READ  | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fin == NULL)
	{
		strcpy(PathBuffer, "..\\");
		strcat(PathBuffer, FilePath);
		fin = CreateFileA(PathBuffer, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	}
	if (fin == NULL)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		XNFS_printf(1, "%s: Couldn't open %s\n", PRINT_TYPE_ERROR, PathBuffer);
		return;
	}
	ReadFile(fin, Buffer, FileSize, NULL, NULL);
	
	//fread(Buffer, sizeof(char), FileSize, fin);
	
	//fclose(fin);
}

unsigned int GetFileSizeFromPath(const char* FilePath)
{
	struct stat filesizer = { 0 };
	int stat_ret = stat(FilePath, &filesizer);

	if (stat_ret < 0)
	{
		char PathBuffer[1024];
		sprintf(PathBuffer, "..\\%s", FilePath);
		stat_ret = stat(PathBuffer, &filesizer);
		if (stat_ret < 0)
		{
			if (bConsoleExists)
				perror(PRINT_TYPE_ERROR);
			XNFS_printf(1, "%s: Couldn't open %s\n", PRINT_TYPE_ERROR, PathBuffer);
			return 0;
		}
	}

	return filesizer.st_size;
}

void __stdcall SetPathfinderNull(unsigned int EventID)
{
	unsigned int EventIDToWrite = EventID & 0x00FFFFFF;
	EventIDToWrite += 0x02000000;
	NullPathfinder.EventID = EventIDToWrite;
}

int __cdecl PATHI_getevent(unsigned int a1, int a2)
{
	int v2; // edi@1
	int v3; // edx@1
	int v4; // ecx@2
	int v5; // eax@3
	int v6; // ebx@3
	int result; // eax@3
	unsigned int dword_9C2964 = *(unsigned int*)PATHFINDER_THING_ADDRESS;
	MusicID = a1;

	SetPathfinderNull(a1);
	v2 = *(DWORD *)(dword_9C2964 + 24);
	v3 = *(BYTE *)(v2 + 15) - 1;
	if (v3 < 0)
	{
	LABEL_5:
		result = (int)&NullPathfinder;
	}
	else
	{
		v4 = *(DWORD *)(dword_9C2964 + 36) + 2 * v3;
		while (1)
		{
			v5 = *(WORD *)v4;
			v6 = a2 & *(DWORD *)(v2 + 4 * v5 + 12);
			result = v2 + 4 * v5;
			if ((v6 & 0xFFFFFF) == (a2 & a1))
				break;
			--v3;
			v4 -= 2;
			if (v3 < 0)
				goto LABEL_5;
		}
	}
	return result;
}

int Point3PrecisionFloatToInt(float Input)
{
	return (int)(Input * (float)1000.0);
}

unsigned int CheckIfSupportedFile(const char* filename)
{
	for (unsigned int i = 0; i <= 2; i++)
	{
		if (strstr(filename, SupportedOnlineStreamFormats[i]))
			return FILE_TYPE_ONLINESTREAM;
	}
	const char* point;
	if ((point = strrchr(filename, '.')) != NULL)
	{
		for (unsigned int i = 0; i <= 13; i++)
		{
			if (strcmp(point, SupportedTrackerFormats[i]) == 0)
				return FILE_TYPE_TRACKER;
		}
		for (unsigned int i = 0; i <= 11; i++)
		{
			if (strcmp(point, SupportedStreamingFormats[i]) == 0)
			{
#ifdef BASS_USE_FLAC
				if (strcmp(point, ".flac") == 0 || strcmp(point, ".FLAC") == 0)
					return FILE_TYPE_FLAC;
				else
#endif
				{
					if (strcmp(point, ".ogg") == 0 || strcmp(point, ".OGG") == 0 || strcmp(point, ".logg") == 0 || strcmp(point, ".LOGG") == 0)
						return FILE_TYPE_OGG;
					else
						return FILE_TYPE_STREAM;
				}
			}
		}
		/*for (unsigned int i = 0; i < _countof(SupportedVGMStreamFormats); i++)
		{
			if (strcmp(point, SupportedVGMStreamFormats[i]) == 0)
				return FILE_TYPE_VGMSTREAM;
		}*/
		for (unsigned int i = 0; i <= 5; i++)
		{
			if (strcmp(point, SupportedMIDIFormats[i]) == 0)
				return FILE_TYPE_MIDI;
		}
	}
	return FILE_TYPE_UNKNOWN;
}

/*int SetCurrentPlayingFileType(unsigned int FileType)
{
	switch (FileType)
	{
	case FILE_TYPE_TRACKER:
		CurrentPlayingFileType = FILE_TYPE_TRACKER;
		break;
#ifdef BASS_USE_FLAC
	case FILE_TYPE_FLAC:
		CurrentPlayingFileType = FILE_TYPE_FLAC;
		break;
#endif
	case FILE_TYPE_STREAM:
		CurrentPlayingFileType = FILE_TYPE_STREAM;
		break;
	case FILE_TYPE_ONLINESTREAM:
		CurrentPlayingFileType = FILE_TYPE_ONLINESTREAM;
		break;
/*#ifdef BASS_USE_VGMSTREAM
	case FILE_TYPE_VGMSTREAM:
		CurrentPlayingFileType = FILE_TYPE_VGMSTREAM;
		break;
#endif*/
/*#ifdef BASS_USE_MIDI
	case FILE_TYPE_MIDI:
		CurrentPlayingFileType = FILE_TYPE_MIDI;
		break;
#endif

	default:
		CurrentPlayingFileType = FILE_TYPE_UNKNOWN;
	}
	return 0;
}*/

int SearchPathByID(unsigned int ID) // needs abstraction
{
	unsigned int i = 0;
	XNFS_printf(2, "%s: Searching path by EventID %X\n", PRINT_TYPE_INFO, ID);
	for (i = 0; i < (TrackCount + OverrideCount); i++)
	{
		if (ID == track[i].PathfinderID)
		{
			file = track[i].FilePath;
			//SetCurrentPlayingFileType(track[i].FileType);
			XNFS_printf(2, "%s: Found and set file %s to %X\n", PRINT_TYPE_INFO, file, track[i].PathfinderID);
			XNFS_printf(2, "%s: Type: %s\n", PRINT_TYPE_INFO, FormatTypeStrings[track[i].FileType]);
			FoundIDNum = i;
			break;
		}
	}
	if (FoundIDNum != i)
		XNFS_printf(1, "%s: File for EventID %X NOT FOUND, using previous file: %s\n", PRINT_TYPE_WARNING, ID, file);
	return 0;
}

void _declspec(naked) PauseChannelCave()
{
	_asm
	{
		mov eax, [esi + 0x14]
		mov PauseChannel, eax
	}
	if (PauseChannel == 1)
	{
		if (BASS_ChannelIsActive(music) == BASS_ACTIVE_PLAYING)
			if (!BASS_ChannelPause(music))
				XNFS_printf(2, "%s: Error pausing channel (1) (handle: music): BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		if (bInteractiveMode)
			if (BASS_ChannelIsActive(oldhandle) == BASS_ACTIVE_PLAYING)
				if (!BASS_ChannelPause(oldhandle))
					XNFS_printf(1, "%s: Error pausing channel (1) during interactive mode (handle: oldhandle): BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		//bStopNodeQueueing = 1;
	}
	_asm
	{
		pop esi
		jmp SNDSYS_leavecritical
	}
}

void _declspec(naked) ResumeChannelCave()
{
	_asm
	{
		mov eax, [esi + 0x14]
		mov ResumeChannel, eax
	}
	if (ResumeChannel == 1 && ((bPlayedAtLeastOnce || bPlayedInteractiveAtLeastOnce)) && !IS_GAME_PAUSED)
	{
		if (!BASS_ChannelPlay(music, FALSE))
			XNFS_printf(1, "%s: Error resuming channel (handle: music, restart: FALSE): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		//bStopNodeQueueing = 0;
		if (BASS_ChannelIsActive(music) == BASS_ACTIVE_STOPPED)
			if (!BASS_ChannelPlay(music, TRUE))
				XNFS_printf(1, "%s: Error resuming channel (handle: music, restart: TRUE): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	}
	_asm
	{
		pop esi
		jmp SNDSYS_leavecritical
	}
}

int StopBassChannel()

{
	if (BASS_ChannelIsActive(music) == BASS_ACTIVE_PLAYING)
		if (!BASS_ChannelStop(music))
			XNFS_printf(1, "%s: Error stopping channel (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	if (BASS_ChannelIsActive(oldhandle) == BASS_ACTIVE_PLAYING)
		if (!BASS_ChannelStop(oldhandle))
			XNFS_printf(1, "%s: Error stopping channel (handle: oldhandle): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	if (PlaySync)
		if (!BASS_ChannelRemoveSync(oldhandle, PlaySync))
			XNFS_printf(3, "%s: Error removing sync (handle: oldhandle, sync: PlaySync): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	PlaySync = 0;
	bPlayedAtLeastOnce = 0;
	bPlayedInteractiveAtLeastOnce = 0;
	return 1;
}

int PauseBassChannel(int something)
{
	if (BASS_ChannelIsActive(music) == BASS_ACTIVE_PLAYING)
		if (!BASS_ChannelPause(music))
			XNFS_printf(1, "%s: Error pausing channel (2) (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	if (bInteractiveMode)
		if (BASS_ChannelIsActive(oldhandle) == BASS_ACTIVE_PLAYING)
			if (!BASS_ChannelPause(oldhandle))
				XNFS_printf(1, "%s: Error pausing channel (2) during interactive mode (handle: oldhandle): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	bStopNodeQueueing = 1;
	return 1;
}

void __declspec(naked) StartPursuitMusicCave()
{
	_asm call PATH_clearallevents
	bPlayedInteractiveAtLeastOnce = 0;
	bStopNodeQueueing = 0;
	_asm jmp StartPursuitMusicCaveExit
}

void __declspec(naked) StartAmbienceCave()
{
	_asm call PATH_clearallevents
	bPlayedInteractiveAtLeastOnce = 0;
	bStopNodeQueueing = 0;
	_asm jmp StartAmbienceCaveExit
}

void CALLBACK MetaSyncProc(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	const char *meta = BASS_ChannelGetTags(channel, BASS_TAG_META); // get metadata
	meta = strstr(meta, "StreamTitle='"); // look for title
	if (meta) { // found it, copy it...
		DoMeta(meta);
	}
}

void CALLBACK LoopCounterSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if ((LoopCounter == track[FoundIDNum].Loop) && (track[FoundIDNum].Loop != -1)) // -1 = infinite
		BASS_ChannelFlags(music, 0, BASS_SAMPLE_LOOP);
	else
		LoopCounter++;
}

void CALLBACK StreamPositionLoopSync(HSYNC handle, DWORD channel, DWORD data, void *user)
{
	if (!BASS_ChannelSetPosition(music, CurrentTrackLoopStart, BASS_POS_BYTE))
		XNFS_printf(1, "%s: Error setting position during StreamPositionLoopSync (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	if ((LoopCounter == track[FoundIDNum].Loop) && (track[FoundIDNum].Loop != -1)) // -1 = infinite
	{
		bIsVolumeSliding = 1;
		if (!BASS_ChannelSlideAttribute(music, BASS_ATTRIB_VOL, -1.0, FadeoutTime))
			XNFS_printf(1, "%s: Error sliding volume attribute during StreamPositionLoopSync (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	}
	else
		LoopCounter++;
}

int CreateBASSHandleByFileType(int FileType, char* FilePath, DWORD &hHandle)
{
	const char *meta;// get metadata

	if (fonthandle)
	{
		BASS_MIDI_FontFree(fonthandle);
		fonthandle = 0;
	}

	switch (FileType)
	{
	case FILE_TYPE_TRACKER:
		hHandle = BASS_MusicLoad(FALSE, FilePath, 0, 0, BASS_MUSIC_RAMPS | BASS_MUSIC_PRESCAN, 1);
		if (!BASS_ChannelSetAttribute(hHandle, BASS_ATTRIB_MUSIC_AMPLIFY, GlobalTrackerAmp))
			XNFS_printf(1, "%s: Error setting music amp attribute during handle creation: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		if (!BASS_ChannelSetAttribute(hHandle, BASS_ATTRIB_MUSIC_AMPLIFY, track[FoundIDNum].ModuleAmpLevel))
			XNFS_printf(1, "%s: Error setting music amp attribute during handle creation (track specific): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		if (!BASS_ChannelSetAttribute(hHandle, BASS_ATTRIB_MUSIC_PSCALER, 10))
			XNFS_printf(1, "%s: Error setting music pscaler attribute during handle creation: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		if (!hHandle)
			XNFS_printf(1, "%s: Error creating tracker handle for file: %s | BASS Error: %d\n", PRINT_TYPE_ERROR, FilePath, BASS_ErrorGetCode());
		break;
	case FILE_TYPE_ONLINESTREAM:
		hHandle = BASS_StreamCreateURL(FilePath, 0, 0, NULL, 0); // TODO: add error handling

		if (!hHandle)
		{
			XNFS_printf(1, "%s: Error creating online stream handle for file: %s | BASS Error: %d\n", PRINT_TYPE_ERROR, FilePath, BASS_ErrorGetCode());
			break;
		}
		meta = BASS_ChannelGetTags(hHandle, BASS_TAG_META);
		if (meta)
		{
			meta = strstr(meta, "StreamTitle='"); // look for title
			if (meta) { // found it, copy it...
				DoMeta(meta);
			}
		}
		MetaSync = BASS_ChannelSetSync(hHandle, BASS_SYNC_META, 0, MetaSyncProc, 0);
		break;
#ifdef BASS_USE_FLAC
	case FILE_TYPE_FLAC:
		hHandle = BASS_FLAC_StreamCreateFile(FALSE, FilePath, 0, 0, BASS_STREAM_PRESCAN);
		if (!hHandle)
			XNFS_printf(1, "%s: Error creating FLAC handle for file: %s | BASS Error: %d\n", PRINT_TYPE_ERROR, FilePath, BASS_ErrorGetCode());
		break;
#endif

	case FILE_TYPE_MIDI:
#ifdef BASS_USE_MIDI
		SF2filename = (char*)calloc(strlen(FilePath) + 1, sizeof(char));

		hHandle = BASS_MIDI_StreamCreateFile(FALSE, FilePath, 0, 0, NULL, 1);
		if (!hHandle)
		{
			XNFS_printf(1, "%s: Error creating MIDI handle for file: %s | BASS Error: %d\n", PRINT_TYPE_ERROR, FilePath, BASS_ErrorGetCode());
			free(SF2filename);
			break;
		}

		strcpy(SF2filename, FilePath);
		strcpy(strrchr(SF2filename, '.'), ".sf2");
		if (CheckIfFileExists(true, SF2filename))
		{
			XNFS_printf(2, "%s: BASSMIDI loading track-specific SoundFont: %s\n", PRINT_TYPE_INFO, SF2filename);
			fonthandle = BASS_MIDI_FontInit(SF2filename, 0);
			if (!fonthandle)
				XNFS_printf(1, "%s: Error loading SoundFont: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
			else
			{
				fontstruct[0].font = fonthandle;
				fontstruct[0].preset = -1;
				fontstruct[0].bank = 0;
				BASS_MIDI_StreamSetFonts(hHandle, fontstruct, 1);
			}
		}
		else
		{
			XNFS_printf(1, "%s: BASSMIDI loading SoundFont: %s\n", PRINT_TYPE_INFO, SoundFontFilePath);
			fonthandle = BASS_MIDI_FontInit(SoundFontFilePath, 0);
			if (!fonthandle)
				XNFS_printf(1, "%s: Error loading SoundFont: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
			else
			{
				fontstruct[0].font = fonthandle;
				fontstruct[0].preset = -1;
				fontstruct[0].bank = 0;
				BASS_MIDI_StreamSetFonts(hHandle, fontstruct, 1);
			}
		}
		free(SF2filename);
		break;
#endif
	default:
		hHandle = BASS_StreamCreateFile(FALSE, FilePath, 0, 0, Flags | BASS_STREAM_PRESCAN);
		if (!hHandle)
			XNFS_printf(1, "%s: Error creating stream handle for file: %s | BASS Error: %d\n", PRINT_TYPE_ERROR, FilePath, BASS_ErrorGetCode());
	}
	if (track[FoundIDNum].Loop && !bInteractiveMode) // Needs work. ADD MIDI MARKER READING FOR LOOP INFO! FIXME: looping on tracks shorter than 5s
	{
		if (track[FoundIDNum].Loop == -1)
			XNFS_printf(2, "%s: Track looping infinitely.\n", PRINT_TYPE_INFO, track[FoundIDNum].Loop);
		else
			XNFS_printf(2, "%s: Track looping %d times.\n", PRINT_TYPE_INFO, track[FoundIDNum].Loop);
		if ((FileType == FILE_TYPE_STREAM || FILE_TYPE_OGG) && track[FoundIDNum].LoopStart != 0)
		{
			if (!BASS_ChannelGetInfo(hHandle, &ChanInfo))
				XNFS_printf(1, "%s: Error getting channel info during handle creation: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
			CurrentTrackLoopStart = track[FoundIDNum].LoopStart * 2 * ChanInfo.chans;
			if (track[FoundIDNum].LoopEnd == 0)
				CurrentTrackLoopEnd = BASS_ChannelGetLength(hHandle, BASS_POS_BYTE);
			else
				CurrentTrackLoopEnd = track[FoundIDNum].LoopEnd * 2 * ChanInfo.chans;
			LoopSync = BASS_ChannelSetSync(hHandle, BASS_SYNC_MIXTIME | BASS_SYNC_POS, CurrentTrackLoopEnd, StreamPositionLoopSync, 0);
		}
		else
		{
			BASS_ChannelFlags(hHandle, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
			LoopSync = BASS_ChannelSetSync(hHandle, BASS_SYNC_MIXTIME | BASS_SYNC_END, 0, LoopCounterSync, 0);
		}
	}

	return 0;
}

int BASSTimeRemaining()
{
	QWORD remaining = 0;
	float time;
	if (bInteractiveMode)
		remaining = BASS_StreamPutData(music, NULL, 0) /*- BASS_ChannelGetPosition(music, BASS_POS_BYTE)*/;
	else
		remaining = BASS_ChannelGetLength(music, BASS_POS_BYTE) - BASS_ChannelGetPosition(music, BASS_POS_BYTE);

	if (!remaining)
		return 0;
	time = (float)BASS_ChannelBytes2Seconds(music, remaining); // time remaining in seconds
		
	//printf("Time remaining: %.3f\n%llx\n", time, remaining);
	if ((time < 0.0) || (bStopNodeQueueing && bInteractiveMode) || (track[FoundIDNum].Loop == -1) || (track[FoundIDNum].FileType == FILE_TYPE_ONLINESTREAM)) // If time is negative, it's probably infinite, so we stick the time to 1.0. Also a hack to stop interactive music.
		return 1000;

#ifdef GAME_CARBON // willing to make an exception not to put it in the Carbon.h file for this little snippet
	if (!bInteractiveMode)
	{
		time += (float)0.819; // hack to prevent the game from cutting off tracks, needs game code checking!
		if (track[FoundIDNum].Loop)
			time += (float)0.1;
	}
#endif

	return Point3PrecisionFloatToInt((float)time); // example: 12.345 -> 12345
}

int BASSPlayInteractiveQueue(unsigned int NodeID)
{
	QWORD ChannelLength, Latency;

	if (bPlayedInteractiveAtLeastOnce)
	{
		if (!bStopNodeQueueing)
		{
			SearchPathByNodeID(NodeID);
			XNFS_printf(2, "%s: Queueing NodeID: %X\n", PRINT_TYPE_INFO, NodeID);
			
			XNFS_BASS_PushFile(file, music);

			if (bGameVolumeControl)
				if (!BASS_ChannelSetAttribute(music, BASS_ATTRIB_VOL, LastVolume))
					XNFS_printf(1, "%s: Can't set volume attribute during interactive queue (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		}
	}
	else
	{
		SearchPathByNodeID(NodeID);
		bStopNodeQueueing = false;
		XNFS_BASS_CreateStreamPushHandle(music);
		if (bGameVolumeControl)
			BASS_ChannelSetAttribute(music, BASS_ATTRIB_VOL, LastVolume);
		XNFS_printf(2, "%s: Playing Pathfinder NodeID: %X\n", PRINT_TYPE_INFO, NodeID);
		if (!BASS_ChannelPlay(music, TRUE))
			XNFS_printf(1, "%s: Can't play interactive node (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		XNFS_BASS_PushFile(file, music);
	}
	bPlayedInteractiveAtLeastOnce = 1;
	PlayCounter += 0x100;
	return PlayCounter; // Adjusted to a common EAX value -- TODO: figure out how these are calculated exactly...
}

int BASSPlayChannel(int something1, unsigned int something2, int something3, int something4, unsigned int something5) // FIXME: change ambiguous name...
{
	if (bIsVolumeSliding)
		if (!BASS_ChannelSlideAttribute(music, BASS_ATTRIB_VOL, 1.0, 0))
			XNFS_printf(1, "%s: Error disabling volume attribute sliding during BASSPlayChannel (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());

	bIsVolumeSliding = 0;
	LoopCounter = 0;
	bOnlineChyron = 0;

	if (PlayCounter == -1)
		PlayCounter = 0x001;

	if (bEnableInteractiveNoding)
	{
		if (CheckIfInteractiveEventID(MusicID))
		{
			if (!bInteractiveMode)
			{
				XNFS_printf(2, "%s: Entering interactive mode...\n", PRINT_TYPE_INFO);
				if (MetaSync)
					if (!BASS_ChannelRemoveSync(music, MetaSync))
						XNFS_printf(3, "%s: Can't remove sync during interactive entrance (handle: music, sync: MetaSync): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
				MetaSync = 0;
				if (LoopSync)
					if (!BASS_ChannelRemoveSync(music, LoopSync))
						XNFS_printf(3, "%s: Can't remove sync during interactive entrance (handle: music, sync: LoopSync): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
				LoopSync = 0;
				bInteractiveMode = 1;
			}
			return BASSPlayInteractiveQueue(something2);
		}
		else
		{
			if (bInteractiveMode)
				XNFS_printf(2, "%s: Exiting interactive mode...\n", PRINT_TYPE_INFO);
			bInteractiveMode = 0;
			bPlayedInteractiveAtLeastOnce = 0;
		}
	}
	if (MetaSync)
		if (!BASS_ChannelRemoveSync(music, MetaSync))
			XNFS_printf(1, "%s: Can't remove sync (handle: music, sync: MetaSync): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	MetaSync = 0;
	SearchPathByID(MusicID);
	CreateBASSHandleByFileType(track[FoundIDNum].FileType, file, music);
	if (bGameVolumeControl)
		if (!BASS_ChannelSetAttribute(music, BASS_ATTRIB_VOL, LastVolume))
			XNFS_printf(1, "%s: Can't set volume attribute (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	XNFS_printf(2, "%s: Playing Pathfinder EventID: %X\n", PRINT_TYPE_INFO, MusicID);
	if (!BASS_ChannelPlay(music, TRUE))
		XNFS_printf(1, "%s: Can't play channel (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	bPlayedAtLeastOnce = 1;
	PlayCounter += 0x100;
	return PlayCounter; // Adjusted to a common EAX value
}

int PlayInstallerMusic()
{
	MusicID = INSTALL_MUSICID;
	BASSPlayChannel(0, 0, 0, 0, 0);
	BASS_ChannelSetAttribute(music, BASS_ATTRIB_VOL, 0.4);
	return 0;
}

void __declspec(naked) BASSTimeRemainingCave()
{
	BASSTimeRemaining();
	_asm jmp BASSTimeRemainingCaveExit
}

void BASSVolume(int something, int something2, float volume)
{
	if (volume >= 1.0)
		volume = 1.0;
	//printf("Setting volume to %f\n", volume);
	LastVolume = volume;
	if (!bIsVolumeSliding)
		if (BASS_ChannelIsActive(music))
			if (!BASS_ChannelSetAttribute(music, BASS_ATTRIB_VOL, volume))
				XNFS_printf(1, "%s: Can't set volume (handle: music): BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
}

void __declspec(naked) BASSVolumeCave()
{
	_asm
	{
		fmul ds : VolMultiplier
		jmp BASSVolumeCaveExit
	}
}

int BASSDestroy()
{
	_asm call somefunction2
	XNFS_printf(2, "%s: BASSDestroy()\n", PRINT_TYPE_INFO);
	if (!BASS_Free())
		XNFS_printf(1, "%s: Error during BASS_Free in BASSDestroy: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());

	MetaSync = 0;
	LoopSync = 0;
	PlaySync = 0;
	LastVolume = 0.22;
	bPlayedAtLeastOnce = 0;
	bPlayedInteractiveAtLeastOnce = 0;
	bIsVolumeSliding = 0;
	bInteractiveMode = 0;
	LoopCounter = 0;
	bStopNodeQueueing = 1;
	PlayCounter = -1;
	return 1;
}

int BASSCreate()
{
	unsigned int somefunctionreturn = 0;
	_asm
	{
		call somefunction
		mov somefunctionreturn, eax
	}
	XNFS_printf(2, "%s: BASSCreate()\n", PRINT_TYPE_INFO);
	if (!bBASSCreated)
	{
		XNFS_printf(1, "%s: Setting up BASS\n", PRINT_TYPE_INFO);

		if (BASS_GetDevice() != -1)
		{
			if (!BASS_Free()) // hack, BASS freezes if this is called during DoFilePathsStruct
				XNFS_printf(1, "%s: Can't BASS_Free() during init\n", PRINT_TYPE_ERROR);
		}

		if (bUseGameThread)
		{
			if (!BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, 0))
				XNFS_printf(1, "%s: Error setting config for game thread: BASS_CONFIG_UPDATETHREADS: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
			if (!BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 0))
				XNFS_printf(1, "%s: Error setting config for game thread: BASS_CONFIG_UPDATEPERIOD: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		}
		else
		{
			if (NumOfThreads)
				if (!BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, NumOfThreads))
					XNFS_printf(1, "%s: Error setting config: BASS_CONFIG_UPDATETHREADS: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
			if (UpdatePeriod)
				if (!BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, UpdatePeriod))
					XNFS_printf(1, "%s: Error setting config: BASS_CONFIG_UPDATEPERIOD: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		}
		if (!BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, GlobalVolume))
			XNFS_printf(1, "%s: Error setting config: BASS_CONFIG_GVOL_STREAM: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		if (!BASS_SetConfig(BASS_CONFIG_GVOL_MUSIC, GlobalVolume))
			XNFS_printf(1, "%s: Error setting config: BASS_CONFIG_GVOL_MUSIC: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		if (!BASS_SetConfig(BASS_CONFIG_ASYNCFILE_BUFFER, AsyncBufferSize))
			XNFS_printf(1, "%s: Error setting config: BASS_CONFIG_ASYNCFILE_BUFFER: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		if (BufferSize < 10 || BufferSize > 5000)
			BufferSize = 500;
		if (!BASS_SetConfig(BASS_CONFIG_BUFFER, BufferSize))
			XNFS_printf(1, "%s: Error setting config: BASS_CONFIG_BUFFER: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		if (DevBufferSize > 0)
			if (!BASS_SetConfig(BASS_CONFIG_DEV_BUFFER, DevBufferSize))
				XNFS_printf(1, "%s: Error setting config: BASS_CONFIG_DEV_BUFFER: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());

		bBASSCreated = 1;
	}
	if (!BASS_Init(-1, SampleRate, BASS_DEVICE_LATENCY, 0, NULL))
		XNFS_printf(1, "%s: Error during BASS_Init in BASSCreate: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	if (!BASS_GetInfo(&BassInfo))
		XNFS_printf(1, "%s: Error during BASS_GetInfo (BassInfo) in BASSCreate: BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	XNFS_printf(2, "%s: Latency: %d ms\n", PRINT_TYPE_INFO, BassInfo.latency);
	PlayCounter = -1;
	bStopNodeQueueing = 0;

	if (!bInstallerComplete && bInstallerMusic)
		PlayInstallerMusic();

	return somefunctionreturn;
}

int GenerateEventIDForOnlineStream(unsigned int TrackNumber, const char* URL)
{
	track[TrackNumber].PathfinderID = crc_octets((unsigned char*)URL, strlen(URL) + 1);
	return 0;
}

int GenerateEventIDForTrack(unsigned int TrackNumber)
{
	// Generate the ID for file
	if (bSlowHashing)
	{
		void *TempFileBuffer;
		unsigned int FileSize = GetFileSizeFromPath(track[TrackNumber].FilePath);

		TempFileBuffer = (unsigned char*)malloc(sizeof(char)*FileSize);
		TempFileBuffer = calloc(FileSize, sizeof(char));
		LoadFileInBuffer(TempFileBuffer, track[TrackNumber].FilePath, FileSize);
		track[TrackNumber].PathfinderID = crc_octets((unsigned char*)TempFileBuffer, FileSize);
		free(TempFileBuffer);
	}
	else
		track[TrackNumber].PathfinderID = crc_octets((unsigned char*)track[TrackNumber].FilePath, strlen(track[TrackNumber].FilePath) + 1);

	return 0;
}

bool CheckIfFileExists(bool bSilent, const char* FileName)
{
	FILE *finput = fopen(FileName, "rb");
	if (finput != NULL)
	{
		fclose(finput);
		return 1;
	}
	if (bSilent == 0)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		XNFS_printf(1, "%s: File %s does not exist.\n", PRINT_TYPE_WARNING, FileName);
	}
	return 0;
}

unsigned int CountLinesInFile(FILE *finput)
{
	unsigned long int OldOffset = ftell(finput);
	unsigned int LineCount = 0;
	char ReadCh;

	while (!feof(finput)) // WRONG WRONG WRONG
	{
		ReadCh = fgetc(finput);
		if (ReadCh == '\n')
			LineCount++;
	}
	fseek(finput, OldOffset, SEEK_SET);
	return LineCount + 1;
}

int AllocateTemporaryMemoryForOverrides(int count)
{
	if (!count)
		return 0;
	overrides = (TrackStruct*)calloc(count, sizeof(TrackStruct));
	return 0;
}

int AppendOverridesAndFree(unsigned int index_from)
{
	unsigned int j = 0;
	for (unsigned int i = 0; i < OverrideCount; i++)
	{
		if (overrides[i].FilePath)
		{
			track[index_from + j] = overrides[i];
			j++;
		}
	}
	free(overrides);

	return 0;
}

DWORD WINAPI TrackSwitchHotkeyThread()
{
	while (1)
	{
		Sleep(1);
		HotkeyUpdate();
	}
	return 0;
}

// 11/2021. - support for music during initial setup / installation
int SetupInstallMusic(const char* MusicFileName)
{
	AllocateMemoryForTracks(1);
	track[0].FilePath = (char*)malloc(_MAX_PATH);
	strcpy(track[0].FilePath, MusicFileName);
	track[0].PathfinderID = INSTALL_MUSICID;
	TrackCount = 1;
	JukeboxTrack dummy = {};

	switch (CheckIfSupportedFile(track[0].FilePath))
	{
	case FILE_TYPE_TRACKER:
		track[0].FileType = FILE_TYPE_TRACKER;
		XNFS_printf(2, "%s: Assigned tracker module %s to %X\n", PRINT_TYPE_INFO, track[0].FilePath, track[0].PathfinderID);
		if (!dummy.TrackName || !dummy.TrackAlbum || !dummy.TrackArtist)
		{
			//if (ReadEATraxTags_Module(track[0].FilePath, 0, &dummy, music))
			//{
			//	XNFS_printf(2, "%s: Read tracker tags for file %s\n", PRINT_TYPE_INFO, track[0].FilePath);
			//}
			//else
				ReadEATraxTags_Unknown(track[0].FilePath, 0, &dummy);
		}
		break;

#ifdef BASS_USE_FLAC
	case (FILE_TYPE_FLAC):
		track[0].FileType = FILE_TYPE_FLAC;
		XNFS_printf(2, "%s: Assigned FLAC stream %s to %X\n", PRINT_TYPE_INFO, track[0].FilePath, track[0].PathfinderID);
		if (!dummy.TrackName || !dummy.TrackAlbum || !dummy.TrackArtist)
		{
			if (ReadEATraxTags_FLAC(track[0].FilePath, 0, &dummy, track))
				XNFS_printf(2, "%s: Read FLAC tags for file %s\n", PRINT_TYPE_INFO, track[0].FilePath);
			else
				ReadEATraxTags_Unknown(track[0].FilePath, 0, &dummy);
		}
		break;
#endif
	case (FILE_TYPE_OGG):
		track[0].FileType = FILE_TYPE_OGG;
		XNFS_printf(2, "%s: Assigned OGG stream %s to %X\n", PRINT_TYPE_INFO, track[0].FilePath, track[0].PathfinderID);
		if (!dummy.TrackName || !dummy.TrackAlbum || !dummy.TrackArtist)
		{
			XNFS_printf(2, "%s: Checking for Vorbis comments for file %s\n", PRINT_TYPE_INFO, track[0].FilePath);

			if (ReadEATraxTags_OGG(track[0].FilePath, 0, &dummy, track))
				XNFS_printf(2, "%s: Read OGG tags for file %s\n", PRINT_TYPE_INFO, track[0].FilePath);
			else
				ReadEATraxTags_Unknown(track[0].FilePath, 0, &dummy);
		}
		break;

	case (FILE_TYPE_STREAM):
		track[0].FileType = FILE_TYPE_STREAM;
		XNFS_printf(2, "%s: Assigned streamed track %s to %X\n", PRINT_TYPE_INFO, track[0].FilePath, track[0].PathfinderID);
		if (!dummy.TrackName || !dummy.TrackAlbum || !dummy.TrackArtist)
		{
			if (ReadEATraxTags_ID3v2(track[0].FilePath, 0, &dummy))
				XNFS_printf(2, "%s: Read ID3v2 tags for file %s\n", PRINT_TYPE_INFO, track[0].FilePath);
			//else if (ReadEATraxTags_ID3v1(track[0].FilePath, 0, &dummy, music))
			//	XNFS_printf(2, "%s: Read ID3v1 tags for file %s\n", PRINT_TYPE_INFO, track[0].FilePath);
			else if (ReadEATraxTags_RIFFInfo(track[0].FilePath, 0, &dummy))
				XNFS_printf(2, "%s: Read RIFF INFO tags for file %s\n", PRINT_TYPE_INFO, track[0].FilePath);
			else
				ReadEATraxTags_Unknown(track[0].FilePath, 0, &dummy);
		}
		break;

	case FILE_TYPE_ONLINESTREAM:
		track[0].FileType = FILE_TYPE_ONLINESTREAM;
		XNFS_printf(2, "%s: Assigned online stream %s to %X\n", PRINT_TYPE_INFO, track[0].FilePath, track[0].PathfinderID);
		if (!dummy.TrackName || !dummy.TrackAlbum || !dummy.TrackArtist)
		{
			ReadEATraxTags_Online(track[0].FilePath, 0, &dummy);
		}
		break;

	case FILE_TYPE_MIDI:
#ifdef BASS_USE_MIDI
		track[0].FileType = FILE_TYPE_MIDI;
		XNFS_printf(2, "%s: Assigned a MIDI track %s to %X\n", PRINT_TYPE_INFO, track[0].FilePath, track[0].PathfinderID);
		if (!dummy.TrackName || !dummy.TrackAlbum || !dummy.TrackArtist)
		{
			ReadEATraxTags_Unknown(track[0].FilePath, 0, &dummy);
		}
		break;
#else
		XNFS_printf(1, "%s: Could NOT assign a MIDI track %s -- Compile with BASS_USE_MIDI to enable support.\n", PRINT_TYPE_WARNING, track[i].FilePath, track[i].PathfinderID);
#endif

	default:
		track[0].FileType = FILE_TYPE_UNKNOWN;
		XNFS_printf(1, "%s: Assigned unknown type track %s to %X\n", PRINT_TYPE_WARNING, track[0].FilePath, track[0].PathfinderID);
		if (!dummy.TrackName || !dummy.TrackAlbum || !dummy.TrackArtist)
		{
			ReadEATraxTags_Unknown(track[0].FilePath, 0, &dummy);
		}
	}

	track[0].Loop = -1;
	return 0;
}

int DoFileOverrides(const char* TxtFile)
{
	FILE *fin;
	unsigned int i = 0;
	int lastcharpos = 0;
	char ReadLine[2048];
	char ReadPath[2048];
	fin = fopen(TxtFile, "r");
	if (fin == NULL)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		return(-1);
	}

	AllocateTemporaryMemoryForOverrides(CountLinesInFile(fin));

	while (!feof(fin))	// FIXME: Make less hacky code.
	{
		fgets(ReadLine, 2048, fin);
		if (strcmp(ReadLine, "") != 0)
		{
			if (ReadLine[0] != '#')
			{
				sscanf(ReadLine, "%X = %s\n", &overrides[i].PathfinderID, ReadPath);
				lastcharpos = strlen(ReadPath) - 1;
				if (ReadPath[lastcharpos] == '\n')
					ReadPath[lastcharpos] = 0;
				overrides[i].FilePath = (char*)malloc(strlen(ReadPath) + 1);
				strcpy(overrides[i].FilePath, ReadPath);
				switch (CheckIfSupportedFile(overrides[i].FilePath))
				{
				case FILE_TYPE_TRACKER:
					overrides[i].FileType = FILE_TYPE_TRACKER;
					XNFS_printf(2, "%s: Assigned tracker module %s to %X\n", PRINT_TYPE_INFO, overrides[i].FilePath, overrides[i].PathfinderID);
					break;

#ifdef BASS_USE_FLAC
				case (FILE_TYPE_FLAC):

					overrides[i].FileType = FILE_TYPE_FLAC;
					XNFS_printf(2, "%s: Assigned FLAC stream %s to %X\n", PRINT_TYPE_INFO, overrides[i].FilePath, overrides[i].PathfinderID);
					break;
#endif
				case (FILE_TYPE_OGG):
					overrides[i].FileType = FILE_TYPE_OGG;
					XNFS_printf(2, "%s: Assigned OGG stream %s to %X\n", PRINT_TYPE_INFO, overrides[i].FilePath, overrides[i].PathfinderID);
					break;

				case (FILE_TYPE_STREAM):
					overrides[i].FileType = FILE_TYPE_STREAM;
					XNFS_printf(2, "%s: Assigned streamed track %s to %X\n", PRINT_TYPE_INFO, overrides[i].FilePath, overrides[i].PathfinderID);
					break;

				case FILE_TYPE_ONLINESTREAM:
					overrides[i].FileType = FILE_TYPE_ONLINESTREAM;
					XNFS_printf(2, "%s: Assigned online stream %s to %X\n", PRINT_TYPE_INFO, overrides[i].FilePath, overrides[i].PathfinderID);
					break;

				case FILE_TYPE_MIDI:
#ifdef BASS_USE_MIDI
					overrides[i].FileType = FILE_TYPE_MIDI;
					XNFS_printf(2, "%s: Assigned a MIDI track %s to %X\n", PRINT_TYPE_INFO, overrides[i].FilePath, overrides[i].PathfinderID);
					break;
#else
					XNFS_printf(1, "%s: Could NOT assign a MIDI track %s -- Compile with BASS_USE_MIDI to enable support.\n", PRINT_TYPE_WARNING, overrides[i].FilePath);
#endif
				default:
					overrides[i].FileType = FILE_TYPE_UNKNOWN;
					XNFS_printf(1, "%s: Assigned unknown type track %s to %X\n", PRINT_TYPE_WARNING, overrides[i].FilePath, overrides[i].PathfinderID);
				}
				i++;
				OverrideCount++;
			}
		}
	}
	fclose(fin);
	XNFS_printf(1, "%s: Overrides processing complete\n", PRINT_TYPE_INFO);
	return 0;
}

void M3UPathCheck(const char* M3UFile, const char* InPath, char* OutPath)
{
	char FinalPath[2048];
	const char *M3UPath;
	const char *StringToCopy = InPath;

	// if it's an absolute path, no need to check anything further...
	if ((InPath[1] == ':') || (InPath[0] == '/'))
	{
		strcpy(OutPath, InPath);
		return;
	}

	if (InPath[0] == '\\') // assume Winamp style path if true
	{
		strcpy(&FinalPath[2], InPath);
		FinalPath[1] = ':';
		for (char i = 'A'; i < 'Z'; i++) // check drive letters
		{
			FinalPath[0] = i;
			if (CheckIfFileExists(1, FinalPath))
			{
				strcpy(OutPath, FinalPath);
				return;
			}
		}
	}

	// if all else fails, check the directory the playlist is in
	strcpy(FinalPath, "..\\");
	strcat(FinalPath, InPath);

	if ((!CheckIfFileExists(1, InPath)) && (!CheckIfFileExists(1, FinalPath)))
	{
		M3UPath = strrchr(M3UFile, '\\');
		strncpy(FinalPath, M3UFile, (M3UPath - M3UFile) + 1);
		FinalPath[(M3UPath - M3UFile) + 1] = '\0';
		strcat(FinalPath, InPath);
		StringToCopy = FinalPath;
	}
	strcpy(OutPath, StringToCopy);
}

// this hack is necessary to prevent bass.dll from hanging during BASS_Init when reading tags...
// for some reason the callback that sets a handle number from 0xFFFFFFFF never gets written during this stage.
// this did not happen in the older versions of BASS so I have no clue what's going on
// NOTE: this was written during BASS version 2.4.16.7 - I'd LOVE to use pattern detection here but it simply doesn't work currently
// To actually fix this, I'll need help from the author of BASS himself (Ian Luck) because this is a doozy
// (or better yet - I've written a RIFF/ID3 tag reader of my own instead of relying on BASS to handle this task)
// 
// Currently this hack is in standby mode here as a last resort. All BASS tag readers are disabled. (sadly we lose tracker title reads but OH WELL)
// 
//void BASSInitHack(bool bUndo)
//{
//	if (bUndo)
//	{
//		injector::WriteMemory<unsigned char>(((unsigned int)(&BASS_Init) - 0x666A), 0x74, true);
//		injector::WriteMemory<unsigned char>(((unsigned int)(&BASS_Init) - 0x6669), 0xEF, true);
//	}
//	else
//		injector::MakeNOP(((unsigned int)(&BASS_Init) - 0x666A), 2, true);
//}

int InitConfig()
{
	CIniReader inireader("");

	bInstallerComplete = inireader.ReadInteger("Installer", "InstallerCompleted", 0);
	bLogFileEnabled = inireader.ReadInteger("XNFSMusicPlayer", "LogFileEnabled", 0);
	bInstallerMusic = inireader.ReadInteger("Installer", "InstallerMusic", 1);
	InstallMusicFile = inireader.ReadString("Installer", "InstallMusicFile", INSTALL_MUSIC_FILE);

	if (bLogFileEnabled)
		LogFilePath = inireader.ReadString("XNFSMusicPlayer", "LogFilePath", DEFAULT_LOG_PATH);

	GlobalVolume = inireader.ReadInteger("XNFSMusicPlayer", "GlobalVolume", 10000);
	SampleRate = inireader.ReadInteger("XNFSMusicPlayer", "SampleRate", 44100);
	BufferSize = inireader.ReadInteger("XNFSMusicPlayer", "InputBufferSize", 500);
	DevBufferSize = inireader.ReadInteger("XNFSMusicPlayer", "DeviceBufferSize", -1);
	bAsyncRead = inireader.ReadInteger("XNFSMusicPlayer", "UseAsyncRead", 1);
	if (bAsyncRead)
	{
		Flags |= BASS_ASYNCFILE;
		BASSPush_DecodeFlags |= BASS_ASYNCFILE;
	}

	AsyncBufferSize = inireader.ReadInteger("XNFSMusicPlayer", "AsyncBufferSize", 262144);
	if (AsyncBufferSize < 4096)
		AsyncBufferSize = 4096;
	bSlowHashing = inireader.ReadInteger("XNFSMusicPlayer", "SlowHashing", 0);
	VerbosityLevel = inireader.ReadInteger("XNFSMusicPlayer", "VerbosityLevel", 0);

	bGameVolumeControl = inireader.ReadInteger("Playback", "GameVolumeControl", 1);
	bEnableInteractiveNoding = inireader.ReadInteger("Playback", "EnableInteractiveNoding", 1);
	GlobalTrackerAmp = inireader.ReadFloat("Playback", "GlobalTrackerAmp", 50.0);
	VolMultiplier = inireader.ReadFloat("Playback", "VolumeMultiplier", 1.0) / (float)100.0;
	FadeoutTime = inireader.ReadInteger("Playback", "FadeoutTime", 5000);
	hkTrackSwitch = inireader.ReadInteger("Playback", "TrackSwitchHotkey", VK_MEDIA_NEXT_TRACK);

	bUseGameThread = inireader.ReadInteger("GameThread", "UseGameThread", 0);
	UpdateLength = inireader.ReadInteger("GameThread", "UpdateLength", 500);

	NumOfThreads = inireader.ReadInteger("Threading", "NumOfThreads", 0);
	UpdatePeriod = inireader.ReadInteger("Threading", "UpdatePeriod", 0);

	PlaylistFile = inireader.ReadString("Playlists", "PlaylistFile", DEFAULT_PLAYLIST_PATH);
	PlaylistConfigFile = inireader.ReadString("Playlists", "PlaylistConfigFile", DEFAULT_PLAYLISTCONFIG_PATH);

	OverridePathConfigFile = inireader.ReadString("Playlists", "OverridePathConfigFile", DEFAULT_OVERRIDES_PATH);

#ifdef BASS_USE_MIDI
	SoundFontFilePath = inireader.ReadString("XNFSMusicPlayer", "SoundFontFilePath", "default.sf2");
#endif

	if (bInstallerComplete)
	{
		XNFS_printf(1, "%s: Processing EventID overrides\n", PRINT_TYPE_INFO);
		if (CheckIfFileExists(0, OverridePathConfigFile))
			DoFileOverrides(OverridePathConfigFile);

		XNFS_printf(1, "%s: Processing playlist\n", PRINT_TYPE_INFO);

		// for tag reading...
		//BASSInitHack(false);

		if (CheckIfFileExists(0, PlaylistFile))
			DoFilePathsStruct(PlaylistFile);
		else
			SetNoTracksWarning();

		AppendOverridesAndFree(TrackCount);

		//BASSInitHack(true);
	}
	else
	{
		if (bInstallerMusic)
			SetupInstallMusic(InstallMusicFile);
	}
	
	InitConfig_GameSpecific();
	if (!bUseGameThread)
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&TrackSwitchHotkeyThread, NULL, 0, NULL);

	return 1;
}
