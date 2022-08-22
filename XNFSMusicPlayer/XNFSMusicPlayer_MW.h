// Most Wanted-specific code for XNFSMusicPlayer

#pragma once
#include "XNFSMusicPlayer.h"
#include "XNFSMusicPlayer_Common.h"

int(__cdecl *NotifyChyron)(const char* TrackTitle, const char* TrackAlbum, const char* TrackArtist) = (int(__cdecl*)(const char*, const char*, const char*))NOTIFYCHYRON_ADDRESS;
unsigned int(__cdecl* bGetTicker)() = (unsigned int(__cdecl*)())BGETTICKER_ADDRESS;

#define DEFAULT_NODEPATHS_PATH "scripts\\XNFSMusicPlayer\\NodePaths.txt"
#define MAXNODES 3258
#define INTERACTIVEIDNUM 43

char* NodePathConfigFile = DEFAULT_NODEPATHS_PATH;

unsigned int JukeCaveExit = JUKECAVEEXIT_ADDRESS;
unsigned int SetSaveDataPointerExit = SAVEDATAEXIT1_ADDRESS;
unsigned int SetSaveDataPointerExit2 = SAVEDATAEXIT2_ADDRESS;
unsigned int SetSaveDataPointerExit3 = SAVEDATAEXIT3_ADDRESS;
unsigned int SetSaveDataPointerExit4 = SAVEDATAEXIT4_ADDRESS;
unsigned int SaveDataWriterExit = SAVEDATAWRITEREXIT_ADDRESS;
unsigned int NotifyChyronCaveExit = NOTIFYCHYRONCAVEEXIT_ADDRESS;

unsigned int TrackToSave = 0;
unsigned int ModeToSave = 0;

char shoutcasttitle[256];
char shoutcasttitlepart1[43];
char shoutcasttitlepart2[43];
char shoutcasttitlepart3[170];

unsigned int InteractiveIDs[INTERACTIVEIDNUM] =
{
	0x6DD6BB, 0x9DF7DA, 0xE6FF17, 0x7690D2, 0x96E300, 0x46EDA3, 0xB2D374, 0x9509F2, 0x919B1B, 0xE40616, 0x1BBC15, 0x34209F,
	0x2F7671, 0xC505B7, 0x27205F, 0x4C6876, 0xE2814E, 0xB39639, 0x518C15, 0xE9222F, 0xC0DC6F, 0xEA0327, 0x1B6B71, 0xB29728,
	0xE391AF, 0xDF442E, 0xD874B1, 0xD12570, 0x32F37E, 0x56667B, 0x515634, 0x580861, 0xD2E818, 0x9B7A50, 0x7B768A, 0x4F246E,
	0xC3FA91, 0x2BBA48, 0x22E859, 0x90B6DC, 0x641F27, 0x6E7282, 0x531659
};

NodeStruct node[MAXNODES];

JukeboxTrack *EATrax;
SaveGameTrax *SaveData;
JukeboxTrack** EATraxPointers;
unsigned int SaveDataPointer;

unsigned int __stdcall bRandom_Custom(int range)
{
	srand(bGetTicker());
	return rand() % range;
}

void __stdcall WriteModeToIni(unsigned int TrackNum, char* Mode)
{
	CIniReader inireader(PlaylistConfigFile);
	char IniReaderSectionID[16];

	sprintf(IniReaderSectionID, "%.6X", track[TrackNum].PathfinderID);
	inireader.WriteString(IniReaderSectionID, "Title", EATrax[TrackNum].TrackName);
	inireader.WriteString(IniReaderSectionID, "Album", EATrax[TrackNum].TrackAlbum);
	inireader.WriteString(IniReaderSectionID, "Artist", EATrax[TrackNum].TrackArtist);
	inireader.WriteString(IniReaderSectionID, "PlaybackMode", Mode);
}

void __declspec(naked) SaveDataWriter2()
{
	_asm mov PreviousEAX, eax
	switch (ModeToSave)
	{
	case 0:
		WriteModeToIni(TrackToSave, "OF");
		break;
	case 1:
		WriteModeToIni(TrackToSave, "FE");
		break;
	case 2:
		WriteModeToIni(TrackToSave, "IG");
		break;
	case 3:
		WriteModeToIni(TrackToSave, "AL");
		break;
	default:
		WriteModeToIni(TrackToSave, "OF");
		break;
	}
	_asm
	{
		mov eax, PreviousEAX
		pop edi
		pop esi
		pop ebp
		pop ebx
		add esp, 0x20
		retn 4
	}
}

void __declspec(naked) SaveDataWriter()
{
	_asm
	{
		mov TrackToSave, ebx
		movzx eax, byte ptr[esi + 4]
		mov ModeToSave, eax
		cmp eax, 3
		jmp SaveDataWriterExit
	}
}

void __declspec(naked) SetSaveDataPointer4()
{
	_asm
	{
		mov edx, SaveDataPointer
		jmp SetSaveDataPointerExit4
	}
}

void __declspec(naked) SetSaveDataPointer3()
{
	_asm
	{
		mov esi, SaveDataPointer
		jmp SetSaveDataPointerExit3
	}
}

void __declspec(naked) SetSaveDataPointer2()
{
	_asm
	{
		mov eax, SaveDataPointer
		jmp SetSaveDataPointerExit2
	}
}

void __declspec(naked) SetSaveDataPointer()
{
	_asm
	{
		mov edx, SaveDataPointer
		mov ecx, edx
		add edx, 0x324
		jmp SetSaveDataPointerExit
	}
}

void __declspec(naked) NotifyChyronCave()
{
	if (!bOnlineChyron)
	{
		_asm
		{
			mov ecx, esi
			call SFXObj_PFEATrax_NotifyChyron
		}
	}
	_asm
	jmp NotifyChyronCaveExit
}

int HotkeyUpdate()
{
	// handled through ingame actions...
	/*if ((*(unsigned int*)THEGAMEFLOWMANAGER_ADDRESS == 3) || (*(unsigned int*)THEGAMEFLOWMANAGER_ADDRESS == 6))
	{
		if (!bInteractiveMode && !*(bool*)SOUNDLOSTWINFOCUS_ADDRESS && bInstallerComplete)
		{
			if ((GetAsyncKeyState(hkTrackSwitch) & 1))
			{
				XNFS_printf(2, "%s: Track switching not implemented for MW\n", PRINT_TYPE_WARNING);
				while ((GetAsyncKeyState(hkTrackSwitch) & 0x8000) > 0) { Sleep(0); }
			}
		}
	}*/
	return 0;
}

void __stdcall DoTheWriting()
{
	*(JukeboxTrack***)JUKESTRUCTPOINTER1 = EATraxPointers;
	*(JukeboxTrack***)JUKESTRUCTPOINTER2 = EATraxPointers;
}

void __declspec(naked) WriteJukePointers()
{
	DoTheWriting();
	_asm
	{
		mov ecx, [ebp - 0xC]
		pop edi
		pop esi
		jmp JukeCaveExit
	}
}

unsigned int GetNumberOfEATrax()
{
	return TrackCount;
}

int SearchPathByNodeID(unsigned int ID)
{
	unsigned int i = 0;
	XNFS_printf(2, "%s: Searching path by NodeID %X\n", PRINT_TYPE_INFO, ID);
	for (i = 0; i < MAXNODES; i++)
	{
		if (ID == node[i].PathfinderID)
		{
			CurrentPlayingNode = i;
			file = node[i].FilePath;
			//SetCurrentPlayingFileType(node[i].FileType);
			XNFS_printf(2, "%s: Found and set file %s to %X\n", PRINT_TYPE_INFO, file, node[i].PathfinderID);
			XNFS_printf(2, "%s: Type: %s\n", PRINT_TYPE_INFO, FormatTypeStrings[node[i].FileType]);
			FoundIDNum = i;
			break;
		}
	}
	if (FoundIDNum != i)
		XNFS_printf(1, "%s: File for NodeID %X NOT FOUND, using previous file: %s\n", PRINT_TYPE_WARNING, ID, file);
	return 0;
}

void DoMeta(const char *meta)
{
	memset(shoutcasttitle, 0, 256);
	strcpy(shoutcasttitle, meta + 13);
	if (strchr(shoutcasttitle, ';'))
		memset(strchr(shoutcasttitle, ';') - 1, 0, 4); // for safer termination, still broken for some reason...

	bOnlineChyron = 1;
	XNFS_printf(2, "%s: Notifying Chyron: %s\n", PRINT_TYPE_INFO, shoutcasttitle);
	if (strlen(shoutcasttitle) > 43)
	{
		memset(shoutcasttitlepart1, 0, 43);
		memset(shoutcasttitlepart2, 0, 43);
		memset(shoutcasttitlepart3, 0, 170);
		strncpy(shoutcasttitlepart1, shoutcasttitle, 42);
		strncpy(shoutcasttitlepart2, shoutcasttitle + 42, 42);
		strcpy(shoutcasttitlepart3, shoutcasttitle + 84);
		NotifyChyron(shoutcasttitlepart1, shoutcasttitlepart2, shoutcasttitlepart3);
	}
	else
		NotifyChyron(shoutcasttitle, "", "");
}

void SetNoTracksWarning()
{
	XNFS_printf(1, "%s: No tracks detected. Please load a valid playlist file!\n", PRINT_TYPE_WARNING);
	TrackCount = 1;
	EATrax[0].TrackName = "WARNING - No tracks detected";
	EATrax[0].TrackAlbum = "Please load a valid playlist file!";
	EATrax[0].TrackArtist = "XNFSMusicPlayer";
	EATrax[0].EventID = 0x013C3E07;
	EATraxPointers[0] = EATrax;
}

bool CheckIfInteractiveEventID(unsigned int ID)
{
	for (int i = 0; i < INTERACTIVEIDNUM; i++)
		if (ID == InteractiveIDs[i])
			return 1;
	return 0;
}

int DoFileNodePathsStruct(const char* TxtFile)
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
	while (!feof(fin))	// FIXME: Make less hacky code.
	{
		fgets(ReadLine, 2048, fin);
		if (strcmp(ReadLine, "") != 0)
		{
			if (ReadLine[0] != '#')
			{
				sscanf(ReadLine, "%X = %s\n", &node[i].PathfinderID, ReadPath);
				lastcharpos = strlen(ReadPath) - 1;
				if (ReadPath[lastcharpos] == '\n')
					ReadPath[lastcharpos] = 0;
				node[i].FilePath = (char*)malloc(strlen(ReadPath) + 1);
				strcpy(node[i].FilePath, ReadPath);
				switch (CheckIfSupportedFile(node[i].FilePath))
				{
				case FILE_TYPE_TRACKER:
					node[i].FileType = FILE_TYPE_TRACKER;
					XNFS_printf(3, "%s: Assigned tracker module %s to %X\n", PRINT_TYPE_INFO, node[i].FilePath, node[i].PathfinderID);
					break;

#ifdef BASS_USE_FLAC
				case (FILE_TYPE_FLAC):

					node[i].FileType = FILE_TYPE_FLAC;
					XNFS_printf(3, "%s: Assigned FLAC stream %s to %X\n", PRINT_TYPE_INFO, node[i].FilePath, node[i].PathfinderID);
					break;
#endif
				case (FILE_TYPE_OGG):
					node[i].FileType = FILE_TYPE_OGG;
					XNFS_printf(3, "%s: Assigned OGG stream %s to %X\n", PRINT_TYPE_INFO, node[i].FilePath, node[i].PathfinderID);
					break;

				case (FILE_TYPE_STREAM):
					node[i].FileType = FILE_TYPE_STREAM;
					XNFS_printf(3, "%s: Assigned streamed track %s to %X\n", PRINT_TYPE_INFO, node[i].FilePath, node[i].PathfinderID);
					break;

				case FILE_TYPE_ONLINESTREAM:
					node[i].FileType = FILE_TYPE_ONLINESTREAM;
					XNFS_printf(3, "%s: Assigned online stream %s to %X\n", PRINT_TYPE_INFO, node[i].FilePath, node[i].PathfinderID);
					break;

				case FILE_TYPE_MIDI:
#ifdef BASS_USE_MIDI
					node[i].FileType = FILE_TYPE_MIDI;
					XNFS_printf(3, "%s: Assigned a MIDI track %s to %X\n", PRINT_TYPE_INFO, node[i].FilePath, node[i].PathfinderID);
					break;
#else
					XNFS_printf(1, "%s: Could NOT assign a MIDI track %s -- Compile with BASS_USE_MIDI to enable support.\n", PRINT_TYPE_WARNING, node[i].FilePath);
#endif

				default:
					node[i].FileType = FILE_TYPE_UNKNOWN;
					XNFS_printf(1, "%s: Assigned unknown type track %s to %X\n", PRINT_TYPE_WARNING, node[i].FilePath, node[i].PathfinderID);
				}
				i++;
			}
		}
	}
	fclose(fin);
	XNFS_printf(1, "%s: Node path processing complete\n", PRINT_TYPE_INFO);
	return 0;
}

unsigned int loc_462B9B = 0x462B9B;
unsigned int vsprintfasdfasdf = 0x00462AD9;
void __declspec(naked) vsprintfdamnit()
{
	_asm
	{
		mov eax, ecx
		test eax, eax
		jz vsprinthackexit
		mov al, [ecx]
		test al, al
		mov edx, ds:0x90E654
		jmp vsprintfasdfasdf

		vsprinthackexit:
		jmp loc_462B9B
	}
}

void AllocateMemoryForTracks(unsigned int TrackCount)
{
	if (TrackCount)
	{
		track = (TrackStruct*)calloc(TrackCount, (sizeof(TrackStruct)));
		EATrax = (JukeboxTrack*)calloc(TrackCount, (sizeof(JukeboxTrack)));
		SaveData = (SaveGameTrax*)calloc(TrackCount, (sizeof(SaveGameTrax)));
		EATraxPointers = (JukeboxTrack**)calloc(TrackCount, sizeof(JukeboxTrack*));
	}
	else
	{
		track = (TrackStruct*)calloc(1, (sizeof(TrackStruct)));
		EATrax = (JukeboxTrack*)calloc(1, (sizeof(JukeboxTrack)));
		SaveData = (SaveGameTrax*)calloc(1, (sizeof(SaveGameTrax)));
		EATraxPointers = (JukeboxTrack**)calloc(1, sizeof(JukeboxTrack*));
	}
}

int InjectCode()
{
	injector::MakeJMP(RESUMECHANNELCAVE_ADDRESS, ResumeChannelCave, true);
	injector::MakeJMP(PAUSECHANNELCAVE_ADDRESS, PauseChannelCave, true);
	injector::MakeJMP(STARTPURSUITMUSCAVE_ADDRESS, StartPursuitMusicCave, true);
	injector::MakeJMP(STARTAMBIENCECAVE_ADDRESS, StartAmbienceCave, true);

	injector::WriteMemory<int>(STOPCHANNELVTABLE_ADDRESS, (int)&StopBassChannel, true);
	injector::WriteMemory<int>(PAUSECHANNELVTABLE_ADDRESS, (int)&PauseBassChannel, true);
	injector::WriteMemory<int>(PLAYCHANNELVTABLE_ADDRESS, (int)&BASSPlayChannel, true);
	injector::MakeJMP(TIMEREMAININGCAVE_ADDRESS, BASSTimeRemainingCave, true);

	if (bUseGameThread)
		injector::MakeCALL(SOUNDUPDATERCAVE_ADDRESS, SoundUpdater, true);

	injector::MakeJMP(NULLPATHFINDERFUNCJMP_ADDRESS, PATHI_getevent, true);

	injector::MakeCALL(GETNUMBEROFTRAX_ADDRESS, GetNumberOfEATrax, true);
	injector::MakeJMP(WRITEJUKEPOINTER_ADDRESS, WriteJukePointers, true);
	injector::MakeNOP(DISABLEJUKEWRITE1_ADDRESS, 2, true);
	injector::MakeNOP(DISABLEJUKEWRITE2_ADDRESS, 2, true);
	injector::MakeJMP(DISABLEFEDATABISEWRITEFROM_ADDRESS, DISABLEFEDATABISEWRITETO_ADDRESS, true);

	injector::MakeJMP(SAVEDATACAVE1_ADDRESS, SetSaveDataPointer, true);
	injector::MakeJMP(SAVEDATACAVE2_ADDRESS, SetSaveDataPointer2, true);
	injector::MakeJMP(SAVEDATACAVE3_ADDRESS, SetSaveDataPointer3, true);
	injector::MakeJMP(SAVEDATACAVE4_ADDRESS, SetSaveDataPointer4, true);

	injector::MakeJMP(SAVEDATAWRITER1_ADDRESS, SaveDataWriter, true);
	injector::MakeJMP(SAVEDATAWRITER2_ADDRESS, SaveDataWriter2, true);

	injector::MakeJMP(NOTIFYCHYRONCAVE_ADDRESS, NotifyChyronCave, true);

	if (bGameVolumeControl)
	{
		injector::MakeCALL(SETVOLCAVE_ADDRESS, BASSVolume, true);
		injector::MakeJMP(VOLMULTIPLIERCAVE_ADDRESS, BASSVolumeCave, true);
		injector::MakeNOP(VOLCRITICALDISABLE1_ADDRESS, 5, true);
		injector::MakeNOP(VOLCRITICALDISABLE2_ADDRESS, 5, true);
	}

	//injector::MakeCALL(QUEUENODECAVE_ADDRESS, QueueNodeInterceptor, true);

	//injector::MakeCALL(PATHFINDERCONSTRUCTOR_ADDRESS, BASSCreate, true);
	//injector::MakeCALL(PATHFINDERDESTRUCTOR_ADDRESS, BASSDestroy, true);

	injector::MakeJMP(0x00462ACF, vsprintfdamnit, true);

	// RNG fix
	injector::MakeCALL(MW_RNGFIX_ADDRESS, bRandom_Custom, true);

	// track change fix
	injector::MakeNOP(TRACKCHANGE_NOP_ADDR, 2, true);

	XNFS_printf(1, "%s: Code injection complete.\n", PRINT_TYPE_INFO);

	return 0;
}

int DoFilePathsStruct(char* TxtFile)
{
	FILE *fin;
	unsigned int i = 0;
	int lastcharpos = 0;
	char ReadLine[2048];
	char FinalTrackPath[2048];
	char *TempTxt;

	fin = fopen(TxtFile, "r");
	CIniReader inireader(PlaylistConfigFile);
	char IniReaderSectionID[16];

	if (fin == NULL)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		return -1;
	}

	//BASS_Init(0, 44100, 0, NULL, NULL);
	
	AllocateMemoryForTracks(CountLinesInFile(fin) + OverrideCount);

	while (!feof(fin))	// FIXME: Make less hacky code.
	{
		if (fgets(ReadLine, 2048, fin))
		{
			if (ReadLine[0] != '#')
			{
				lastcharpos = strlen(ReadLine) - 1;
				if (ReadLine[lastcharpos] == '\n')
					ReadLine[lastcharpos] = 0;

				//printf("TEST: FilePath: %s ReadLine: %s\n", track[i].FilePath, ReadLine);

				switch (CheckIfSupportedFile(ReadLine))
				{
				case FILE_TYPE_TRACKER:
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_TRACKER;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					XNFS_printf(2, "%s: Assigned tracker module %s to %X\n", PRINT_TYPE_INFO, track[i].FilePath, track[i].PathfinderID);
					track[i].ModuleAmpLevel = inireader.ReadFloat(IniReaderSectionID, "ModuleAmpLevel", 50.0);
					//if (VerbosityLevel >= 2)
					//	printf("%s: Amplification level: %.3f\n", PRINT_TYPE_INFO, track.ModuleAmpLevel[i]);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						//if (ReadEATraxTags_Module(track[i].FilePath, i, EATrax, music))
						//{
						//	XNFS_printf(2, "%s: Read tracker tags for file %s\n", PRINT_TYPE_INFO, track[i].FilePath);
						//}
						//else
							ReadEATraxTags_Unknown(track[i].FilePath, i, EATrax);
					}
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
					break;

#ifdef BASS_USE_FLAC
				case (FILE_TYPE_FLAC):
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_FLAC;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					XNFS_printf(2, "%s: Assigned FLAC stream %s to %X\n", PRINT_TYPE_INFO, track[i].FilePath, track[i].PathfinderID);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						if (ReadEATraxTags_FLAC(track[i].FilePath, i, EATrax, track))
							XNFS_printf(2, "%s: Read FLAC tags for file %s\n", PRINT_TYPE_INFO, track[i].FilePath);
						else
							ReadEATraxTags_Unknown(track[i].FilePath, i, EATrax);
					}
					if (!track[i].LoopStart)
						track[i].LoopStart = inireader.ReadInteger(IniReaderSectionID, "LoopStart", 0);
					if (!track[i].LoopEnd)
						track[i].LoopEnd = inireader.ReadInteger(IniReaderSectionID, "LoopEnd", 0);
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
					break;
#endif
				case (FILE_TYPE_OGG):
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_OGG;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					XNFS_printf(2, "%s: Assigned OGG stream %s to %X\n", PRINT_TYPE_INFO, track[i].FilePath, track[i].PathfinderID);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						XNFS_printf(2, "%s: Checking for Vorbis comments for file %s\n", PRINT_TYPE_INFO, track[i].FilePath);

						if (ReadEATraxTags_OGG(track[i].FilePath, i, EATrax, track))
							XNFS_printf(2, "%s: Read OGG tags for file %s\n", PRINT_TYPE_INFO, track[i].FilePath);
						else
							ReadEATraxTags_Unknown(track[i].FilePath, i, EATrax);
					}
					if (!track[i].LoopStart)
						track[i].LoopStart = inireader.ReadInteger(IniReaderSectionID, "LoopStart", 0);
					if (!track[i].LoopEnd)
						track[i].LoopEnd = inireader.ReadInteger(IniReaderSectionID, "LoopEnd", 0);
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
					break;

				case (FILE_TYPE_STREAM):
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_STREAM;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					XNFS_printf(2, "%s: Assigned streamed track %s to %X\n", PRINT_TYPE_INFO, track[i].FilePath, track[i].PathfinderID);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						if (ReadEATraxTags_ID3v2(track[i].FilePath, i, EATrax))
							XNFS_printf(2, "%s: Read ID3v2 tags for file %s\n", PRINT_TYPE_INFO, track[i].FilePath);
						//else if (ReadEATraxTags_ID3v1(track[i].FilePath, i, EATrax, music))
						//	XNFS_printf(2, "%s: Read ID3v1 tags for file %s\n", PRINT_TYPE_INFO, track[i].FilePath);
						else if (ReadEATraxTags_RIFFInfo(track[i].FilePath, i, EATrax))
							XNFS_printf(2, "%s: Read RIFF INFO tags for file %s\n", PRINT_TYPE_INFO, track[i].FilePath);
						else
							ReadEATraxTags_Unknown(track[i].FilePath, i, EATrax);
					}
					if (!track[i].LoopStart)
						track[i].LoopStart = inireader.ReadInteger(IniReaderSectionID, "LoopStart", 0);
					if (!track[i].LoopEnd)
						track[i].LoopEnd = inireader.ReadInteger(IniReaderSectionID, "LoopEnd", 0);
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
					break;

				case FILE_TYPE_ONLINESTREAM:
					track[i].FilePath = (char*)malloc(strlen(ReadLine) + 1);
					strcpy(track[i].FilePath, ReadLine);
					track[i].FileType = FILE_TYPE_ONLINESTREAM;
					GenerateEventIDForOnlineStream(i, track[i].FilePath);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					XNFS_printf(2, "%s: Assigned online stream %s to %X\n", PRINT_TYPE_INFO, track[i].FilePath, track[i].PathfinderID);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						ReadEATraxTags_Online(track[i].FilePath, i, EATrax);
					}
					break;

				case FILE_TYPE_MIDI:
#ifdef BASS_USE_MIDI
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_MIDI;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					XNFS_printf(2, "%s: Assigned a MIDI track %s to %X\n", PRINT_TYPE_INFO, track[i].FilePath, track[i].PathfinderID);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						ReadEATraxTags_Unknown(track[i].FilePath, i, EATrax);
					}
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
					break;
#else
					XNFS_printf(1, "%s: Could NOT assign a MIDI track %s -- Compile with BASS_USE_MIDI to enable support.\n", PRINT_TYPE_WARNING, track[i].FilePath, track[i].PathfinderID);
#endif

				default:
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_UNKNOWN;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					XNFS_printf(1, "%s: Assigned unknown type track %s to %X\n", PRINT_TYPE_WARNING, track[i].FilePath, track[i].PathfinderID);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						ReadEATraxTags_Unknown(track[i].FilePath, i, EATrax);
					}
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
				}

				TempTxt = inireader.ReadString(IniReaderSectionID, "Title", NULL);
				if (TempTxt && (strcmp(TempTxt, "") != 0))
				{
					EATrax[i].TrackName = (char*)malloc(strlen(TempTxt));
					strcpy(EATrax[i].TrackName, TempTxt);
				}
				free(TempTxt);

				TempTxt = inireader.ReadString(IniReaderSectionID, "Album", NULL);
				if (TempTxt && (strcmp(TempTxt, "") != 0))
				{
					EATrax[i].TrackAlbum = (char*)malloc(strlen(TempTxt));
					strcpy(EATrax[i].TrackAlbum, TempTxt);
				}
				free(TempTxt);
				TempTxt = inireader.ReadString(IniReaderSectionID, "Artist", NULL);
				if (TempTxt && (strcmp(TempTxt, "") != 0))
				{
					EATrax[i].TrackArtist = (char*)malloc(strlen(TempTxt));
					strcpy(EATrax[i].TrackArtist, TempTxt);
				}
				free(TempTxt);
				EATrax[i].EventID = track[i].PathfinderID;
				EATrax[i].EventID &= 0xFFFFFF;
				EATrax[i].EventID += 0x01000000;

				// Read tags
				EATrax[i].PlaybackMode = (char*)malloc(0x3);
				EATrax[i].PlaybackMode = inireader.ReadString(IniReaderSectionID, "PlaybackMode", "AL");

				// Generate custom savegame data (not saved in the savegame)

				SaveData[i].TrackMode = 3;
				switch (EATrax[i].PlaybackMode[0])
				{
				case 'O':
					if (EATrax[i].PlaybackMode[1] == 'F')
						SaveData[i].TrackMode = 0;
					break;
				case 'F':
					if (EATrax[i].PlaybackMode[1] == 'E')
						SaveData[i].TrackMode = 1;
					break;
				case 'I':
					if (EATrax[i].PlaybackMode[1] == 'G')
						SaveData[i].TrackMode = 2;
					break;
				case 'A':
					if (EATrax[i].PlaybackMode[1] == 'L')
						SaveData[i].TrackMode = 3;
					break;
				default:
					break;
				}

				SaveData[i].TrackNumber = i;
				memset(SaveData[i].UnknownCD, 0xCD, 3);

				EATraxPointers[i] = &(EATrax[i]);
				i++;
			}
		}
	}
	TrackCount = i;

	fclose(fin);
	if (!TrackCount)
		SetNoTracksWarning();
	XNFS_printf(1, "%s: Playlist processing complete\n", PRINT_TYPE_INFO);
	return 0;
}

int InitConfig_GameSpecific()
{
	CIniReader inireader("");
	NodePathConfigFile = inireader.ReadString("Playlists", "NodePathConfigFile", DEFAULT_NODEPATHS_PATH);

	if (bEnableInteractiveNoding && bInstallerComplete)
	{
		XNFS_printf(1, "%s: Processing NodeID paths\n", PRINT_TYPE_INFO);

		if (CheckIfFileExists(0, NodePathConfigFile))
			DoFileNodePathsStruct(NodePathConfigFile);
	}

	NullPathfinder.Unknown1 = 0;
	NullPathfinder.Unknown2 = 0;
	NullPathfinder.Unknown3 = 0;
	NullPathfinder.Unknown4 = 0;
	NullPathfinder.Unknown5 = 0x11000001;
	NullPathfinder.Unknown6 = 0x000F0400;
	NullPathfinder.Unknown7 = 0x01FFFFFF;
	NullPathfinder.Unknown8 = 0x11000001;
	NullPathfinder.Unknown9 = 0x00030400;
	//NullPathfinder.Unknown10 = 0x00FF075A; // requires more hacking for the time remaining thing, can be too long or too short
	NullPathfinder.Unknown10 = 0x00FF0619;
	//NullPathfinder.Unknown10 = 0xFF05CF;

	SaveDataPointer = (unsigned int)SaveData - 0x324;
	return 0;
}
