// Carbon-specific code for XNFSMusicPlayer

#pragma once
#include "XNFSMusicPlayer.h"
#include "XNFSMusicPlayer_Common.h"

int(__thiscall *EAXSound_Random)(void* EAXSound, int range) = (int(__thiscall*)(void*, int))EAXSOUND_RANDOM_ADDRESS;
unsigned int(__cdecl *bGetTicker)() = (unsigned int(__cdecl*)())BGETTICKER_ADDRESS;
void(*ForceLicensedMusic)() = (void(*)())FORCELICENSEDMUSIC_ADDRESS;
unsigned int(__thiscall *StartLicensedMusic)(void* SFXObj_PFEATrax, unsigned int PathEvent) = (unsigned int(__thiscall*)(void*, unsigned int))STARTLICENSEDMUSIC_ADDRESS;

#define DEFAULT_NODEPATHS_PATH_0 "scripts\\XNFSMusicPlayer\\NodePaths0.txt"
#define DEFAULT_NODEPATHS_PATH_1 "scripts\\XNFSMusicPlayer\\NodePaths1.txt"
#define DEFAULT_NODEPATHS_PATH_2 "scripts\\XNFSMusicPlayer\\NodePaths2.txt"
#define DEFAULT_NODEPATHS_PATH_3 "scripts\\XNFSMusicPlayer\\NodePaths3.txt"
#define MAXNODES 2048

#define NONINTERACTIVEIDNUM 31
#define INTERACTIVEIDNUM 66

NodeStruct node[4][MAXNODES];

JukeboxTrack *EATrax;

struct SongAttrib // TODO: try to get rid of attribute stuff altogether
{
	unsigned int ArtistStringHash; //+ 0 // if game is bitchy, generate using the game itself
	const char* TrackArtist; // +4
	unsigned int NameStringHash; // +8
	const char* TrackName; // +C
	unsigned int AlbumStringHash; // +10
	const char* TrackAlbum; // +14
	unsigned int maybecategory1; // +18
	unsigned int CarClassCategory; // +1C
	unsigned int EventID; // +20 // with 01 and then a 24-bit unique ID, example: 019F52FF
};

SongAttrib *SongAttribs;
SongAttrib DummyTrack = { 0, "WARNING", 0, "Out of range", 0, "-", 0, 0, 0 };

unsigned int BankNumber, StolenEBX;
unsigned int loc_8CE4A6 = CRAPFIX_1_EXIT;

bool* TrackEnables;
bool bSequencedPlayback;
bool bDisableInteractive_FE, bExitedTitleScreen;
bool bDisableInteractive_Crew;
bool bDisableInteractive_Canyon;
bool bDisableInteractive_Pursuit;
unsigned int TrackSequencer = 0;
unsigned char DriftMusicType = 2;
unsigned char CanyonDuelMusicType = 16;
unsigned char CanyonSprintMusicType = 16;
unsigned char CrewSprintMusicType = 8;
unsigned char CrewCircuitMusicType = 8;
unsigned char PursuitMusicType = 4;
unsigned char DefaultMusicType = 1;
void* Current_SFXObj_PFEATrax = NULL;

char* NodePathConfigFile[4] =
{
	{ DEFAULT_NODEPATHS_PATH_0 },
	{ DEFAULT_NODEPATHS_PATH_1 },
	{ DEFAULT_NODEPATHS_PATH_2 },
	{ DEFAULT_NODEPATHS_PATH_3 }
};

/*unsigned int NonInteractiveIDs[NONINTERACTIVEIDNUM]
{
	0xC53FC7, 0x0B9EC4, 0xCF17AB, 0x936D34, 0x691978, 0xC46FCD, 0x99FC75, 0xA90981, 0xA8068A, 0xD78805, 0x73EE3F, 0x267EFC,
	0x9F52FF, 0x19BEC3, 0x5A7CBD, 0x9C0FCD, 0xE28391, 0xC5326F, 0x8C5ECD, 0x6090F6, 0xDB4410, 0x7F0EB8, 0x6E4FFC, 0xF05578,
	0x2A9A8F, 0x24A600, 0xD8CEE4, 0xF3F034, 0xC0763C, 0xA84E85,	0x1BAF74
};*/

unsigned int InteractiveIDs[INTERACTIVEIDNUM] =
{
	0x6DD6BB, 0x9DF7DA, 0xE6FF17, 0x7690D2, 0x96E300, 0x46EDA3, 0xB2D374, 0x9509F2, 0x919B1B, 0xE40616, 0x1BBC15, 0x34209F,
	0x2F7671, 0xC505B7, 0x27205F, 0x4C6876, 0xE2814E, 0xB39639, 0x518C15, 0xE9222F, 0xC0DC6F, 0xEA0327, 0x1B6B71, 0xB29728,
	0xE391AF, 0xDF442E, 0xD874B1, 0xD12570, 0x32F37E, 0x56667B, 0x515634, 0x580861, 0xD2E818, 0x9B7A50, 0x7B768A, 0x4F246E,
	0xC3FA91, 0x2BBA48, 0x22E859, 0x90B6DC, 0x641F27, 0x6E7282, 0x531659, 0xC70D6E, 0x935ABF, 0xDF86F2, 0x1384BC, 0x9FF39C, 
	0x31CA1D, 0x575898, 0x5B8C57, 0x71F6C0, 0xFEF0DB, 0xDBF7EE, 0xE3AB33, 0x8FE38A, 0xB2A45E, 0x168D74, 0x35F2DB, 0x4CF288,
	0x1781CA, 0x7C53F9, 0x698844, 0x66CB77, 0x09FEC1, 0xAE31BA
};

unsigned int GetNumberOfEATrax()
{
	return TrackCount;
}

void __stdcall SetFilePath_Part2(const char* path)
{
	const char* StolenFilePathNumber = strrchr(path, '.');
	sscanf(StolenFilePathNumber - 1, "%d.mus", &BankNumber); // limited to a single digit for now
	XNFS_printf(3, "%s: Using bank %d\tFile: %s\n", PRINT_TYPE_INFO, BankNumber, path);
}

void __declspec(naked) PathTrackSndStream_SetFilePathHook(const char* path)
{
	_asm
	{
		push ebp
		mov ebp, esp
		push esi
		mov esi, [ebp+8]
		push edi
		push 0x20
		lea edi, [ecx+0x7C]
		pop ecx
		rep movsd
		pop edi
		pop esi
		mov eax, [ebp + 8]
		push eax
		call SetFilePath_Part2
		pop ebp
		retn 4
	}
}

void AllocateMemoryForTracks(unsigned int TrackCount)
{
	if (TrackCount)
	{
		track = (TrackStruct*)calloc(TrackCount, (sizeof(TrackStruct)));
		EATrax = (JukeboxTrack*)calloc(TrackCount, (sizeof(JukeboxTrack))); // reusing from MW
		SongAttribs = (SongAttrib*)calloc(TrackCount, (sizeof(SongAttrib)));
		TrackEnables = (bool*)calloc(TrackCount, sizeof(bool));
		memset(TrackEnables, true, sizeof(bool) * TrackCount);
	}
	else
	{
		track = (TrackStruct*)calloc(1, (sizeof(TrackStruct)));
		EATrax = (JukeboxTrack*)calloc(1, (sizeof(JukeboxTrack)));
		SongAttribs = (SongAttrib*)calloc(1, (sizeof(SongAttrib)));
		TrackEnables = (bool*)calloc(1, sizeof(bool));
		TrackEnables[0] = true;
	}
}

int SearchPathByNodeID(unsigned int ID)
{
	unsigned int i = 0;
	XNFS_printf(2, "%s: Searching path by NodeID %X\n", PRINT_TYPE_INFO, ID);
	for (i = 0; i < MAXNODES; i++)
	{
		if (ID == node[BankNumber][i].PathfinderID)
		{
			file = node[BankNumber][i].FilePath;
			XNFS_printf(2, "%s: Found and set file %s to %X\n", PRINT_TYPE_INFO, file, node[BankNumber][i].PathfinderID);
			XNFS_printf(2, "%s: Type: %s\n", PRINT_TYPE_INFO, FormatTypeStrings[node[BankNumber][i].FileType]);
			FoundIDNum = i;
			break;
		}
	}
	if (FoundIDNum != i)
		XNFS_printf(1, "%s: File for NodeID %X NOT FOUND, using previous file: %s\n", PRINT_TYPE_WARNING, ID, file);
	return 0;
}

bool CheckIfInteractiveEventID(unsigned int ID)
{
	for (int i = 0; i < INTERACTIVEIDNUM; i++)
		if (ID == InteractiveIDs[i])
			return true;
	return false;
}

/*bool CheckIfInteractiveEventID(unsigned int ID)
{
	for (int i = 0; i < NONINTERACTIVEIDNUM; i++)
		if (ID == NonInteractiveIDs[i])
			return false;
	return true;
}*/

unsigned int StartCrewMusicCaveExit = STARTCREWMUSICEXIT_ADDRESS;
void __declspec(naked) StartCrewMusicCave()
{
	{
		_asm call PATH_clearallevents
		bPlayedInteractiveAtLeastOnce = 0;
		bStopNodeQueueing = 0;
		_asm jmp StartCrewMusicCaveExit
	}
}

unsigned int StartCanyonMusicCaveExit = STARTCANYONMUSICEXIT_ADDRESS;
void __declspec(naked) StartCanyonMusicCave()
{
	{
		_asm call PATH_clearallevents
		bPlayedInteractiveAtLeastOnce = 0;
		bStopNodeQueueing = 0;
		_asm jmp StartCanyonMusicCaveExit
	}
}

void* __stdcall GetTrackAttribPointer(unsigned int TrackNumber)
{
	if (TrackNumber > TrackCount)
		return &DummyTrack;
	//printf("Returning %X\nTrack[%d]: %X / %X / %X\n", &SongAttribs[TrackNumber], TrackNumber, SongAttribs[TrackNumber].TrackName, SongAttribs[TrackNumber].TrackAlbum, SongAttribs[TrackNumber].TrackArtist);
	return &SongAttribs[TrackNumber];
}


unsigned int loc_5BE28A = EATRAXSETUPEXIT_ADDRESS;
unsigned int AddTrax = FEJUKEBOXSCREEN_ADDTRAX_ADDRESS;
void __declspec(naked) InjectTracks1()
{
	_asm
	{
		mov eax, TrackCount
		xor ebp, ebp
		cmp eax, ebx
		jle thyloopexit
		
	thyloop:
		push ebp
		call GetTrackAttribPointer
		// add checks here
		mov edx, [esp+0x10]
		mov dword ptr [esp+0x30], 1
		push ebp
		mov ecx, edx
		call AddTrax
		mov dword ptr[esp+0x30], 0xFFFFFFFF
		mov eax, TrackCount
		inc ebp
		cmp ebp, eax
		jl thyloop
	thyloopexit:
		jmp loc_5BE28A
	}
}

unsigned int InjectTracks2Exit = INJECTTRACKS2_EXIT_ADDRESS;
void __declspec(naked) InjectTracks2()
{
	_asm
	{
	thyloop2:
		push edi
		call GetTrackAttribPointer
		mov ecx, eax
		mov eax, [esp+0x44]
		cmp [ecx+0x20], eax
		jnz thyloop2skip

		mov eax, [esi+0x25C]
		lea edx, [eax+eax*2]
		mov ds:EATRAX_SFXOBJ_ADDRESS[edx*8], edi
		mov eax, [esi+0x208]
		mov ebp, [ecx+0x20]
		and eax, 0x0FFFFFFBF
		mov bl, 1
		mov [esi+0x208], eax
		mov dword ptr [esi+0x220], 0

	thyloop2skip:
		lea ecx, [esp+0x14]
		inc edi
		mov eax, TrackCount
		cmp edi, eax
		jb thyloop2
		jmp InjectTracks2Exit
	}
}

unsigned int InjectNamesExit = INJECTNAMEEXIT_ADDRESS;
void __declspec(naked) InjectTrackNamesInMenu()
{
	_asm
	{
		push ebx
		push ebp
		mov esi, [esi + 0x10]
		push esi
		call GetTrackAttribPointer
		mov [esp+0x14], eax
		jmp InjectNamesExit
	}
}

unsigned int InjectNames2Exit = INJECTNAME2EXIT_ADDRESS;
void __declspec(naked) InjectNames2()
{
	_asm
	{
		mov eax, TrackEnables
		mov al, byte ptr [eax+esi]
		and eax, 0xFF
		jmp InjectNames2Exit
	}
}


unsigned int EATraxPreviewHookExit = EATRAXPREVIEWHOOKEXIT_ADDRESS;
void __declspec(naked) DALOptions_EATraxPreviewHook(int tracknum)
{
	_asm
	{
		push 0xFFFFFFFF
		push 0x00997038
		mov eax, fs:0
		push eax
		sub esp, 0x30
		push esi
		xor esi, esi
		mov ecx, [esp+0x44]
		push ecx
		call GetTrackAttribPointer
		mov ecx, eax
		mov eax, [ecx+0x20]
		jmp EATraxPreviewHookExit
	}
}

bool CheckIfAllTracksDisabled()
{
	for (unsigned int i = 0; i < TrackCount; i++)
	{
		if (TrackEnables[i])
			return false;
	}
	return true;
}

void* __stdcall GiveRandomTrack()
{
	if (CheckIfAllTracksDisabled()) // sanity check before entering an infinite loop
		return GetTrackAttribPointer(0);
	
	unsigned int SelectedTrackIndex = 0;

	//*(unsigned int*)SOUNDRANDOMSEED_ADDRESS = bGetTicker();

	if (!bSequencedPlayback)
		srand(bGetTicker());
		

	while (true)
	{
		if (bSequencedPlayback)
		{
			TrackSequencer %= TrackCount;
			SelectedTrackIndex = TrackSequencer;
			TrackSequencer++;
		}
		else
			SelectedTrackIndex = rand() % TrackCount;
			//SelectedTrackIndex = EAXSound_Random(*(void**)EAXSOUND_GLOBAL_POINTER, TrackCount); // not using EAXSound's RNG, since it's very "sticky"
		
		if (TrackEnables[SelectedTrackIndex])
			break;
	}

	return GetTrackAttribPointer(SelectedTrackIndex);
}

//unsigned int RandomTrackInjectExit1 = RANDOMTRACKINNJECT_EXIT1_ADDRESS;
unsigned int RandomTrackInjectExit2 = RANDOMTRACKINNJECT_EXIT2_ADDRESS;
//unsigned int StartLicensedMusicFunc = STARTLICENSEDMUSIC_ADDRESS;
void __declspec(naked) RandomTrackInject()
{
	_asm
	{
		call GiveRandomTrack
		mov edx, eax
		mov ebp, [edx+0x20]
		mov ecx, esi
		push ebp
		call StartLicensedMusic // RECURSION!!!
		/*mov eax, [esi+0x25C]
		lea ecx, [eax+eax*2]
		mov eax, ds:EATRAX_SFXOBJ_ADDRESS[ecx*8]
		or edi, 0xFFFFFFFF
		mov byte ptr[esp + 0x3C], 0*/
		/*cmp eax, edi
		jnz thyexit1
		jmp RandomTrackInjectExit2

	thyexit1:*/
		jmp RandomTrackInjectExit2
	}
}

void __declspec(naked) FixCrappyCarbonCode()
{
	if (!StolenEBX)
		_asm mov StolenEBX, ebx
	else
		_asm mov ebx, StolenEBX
	_asm
	{
		or byte ptr[ebx + 5], 0x80
		jmp loc_8CE4A6
	}
}

void DoMeta(const char *meta)
{
	XNFS_printf(3, "%s: DoMeta() not implemented for Carbon\n", PRINT_TYPE_WARNING);
}

bool __stdcall DALOptions_IncEATraxSongPlayability_Hook(unsigned int TrackNum)
{
	CIniReader inireader(PlaylistConfigFile);
	char IniReaderSectionID[16];
	sprintf(IniReaderSectionID, "%.6X", track[TrackNum].PathfinderID);

	TrackEnables[TrackNum] = !TrackEnables[TrackNum];
	inireader.WriteInteger(IniReaderSectionID, "TrackEnabled", TrackEnables[TrackNum]);

	inireader.WriteString(IniReaderSectionID, "Title", EATrax[TrackNum].TrackName);
	inireader.WriteString(IniReaderSectionID, "Album", EATrax[TrackNum].TrackAlbum);
	inireader.WriteString(IniReaderSectionID, "Artist", EATrax[TrackNum].TrackArtist);

	return true;
}

unsigned int InjectPadRightLeftExit = INJECTPADLEFTRIGHT_EXIT_ADDRESS;
void __declspec(naked) Inject_FeJukeboxScreen_HandlePadRightLeft()
{
	_asm
	{
		push eax
		call DALOptions_IncEATraxSongPlayability_Hook
		jmp InjectPadRightLeftExit
	}
}


void __declspec(naked) Inject_UpdateInGame()
{
	if (bExitedTitleScreen || *(unsigned int*)SKIPFE_ADDRESS)
		_asm
	{
		cmp edi, 1
		jnz updateigexit
		mov eax, [esi]
		push 0
		mov ecx, esi
		call dword ptr [eax+0x48]
	}

	_asm
	{
	updateigexit:
		pop ebx
		pop edi
		pop ebp
		pop esi
		pop ecx
		retn 4
	}
}

void __declspec(naked) Inject_TitleScreenExit()
{
	_asm
	{
		mov bExitedTitleScreen, 1
		jmp StartLicensedMusic
	}
}

unsigned int StealSFXObj_Exit = STEALSFXOBJ_EXIT_ADDRESS;
void __declspec(naked) StealSFXObj()
{
	_asm
	{
		mov Current_SFXObj_PFEATrax, ecx
		push 0xFFFFFFFF
		push SFXOBJ_SEH
		jmp StealSFXObj_Exit
	}
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

	//BASS_Init(0, 0, 0, 0, NULL);

	if (fin == NULL)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		return -1;
	}

	AllocateMemoryForTracks(CountLinesInFile(fin) + OverrideCount); // needs existence checks first!!!

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
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
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
					break;

#ifdef BASS_USE_FLAC
				case (FILE_TYPE_FLAC):
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_FLAC;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
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
					break;
#endif
				case (FILE_TYPE_OGG):
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_OGG;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
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
					break;

				case (FILE_TYPE_STREAM):
					M3UPathCheck(TxtFile, ReadLine, FinalTrackPath);
					track[i].FilePath = (char*)malloc(strlen(FinalTrackPath) + 1);
					strcpy(track[i].FilePath, FinalTrackPath);

					track[i].FileType = FILE_TYPE_STREAM;
					GenerateEventIDForTrack(i);
					sprintf(IniReaderSectionID, "%.6X", track[i].PathfinderID);
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
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
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
					XNFS_printf(2, "%s: Assigned a MIDI track %s to %X\n", PRINT_TYPE_INFO, track[i].FilePath, track[i].PathfinderID);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						ReadEATraxTags_Unknown(track[i].FilePath, i, EATrax);
					}
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
					track[i].Loop = inireader.ReadInteger(IniReaderSectionID, "Loop", 0);
					XNFS_printf(1, "%s: Assigned unknown type track %s to %X\n", PRINT_TYPE_WARNING, track[i].FilePath, track[i].PathfinderID);
					if (!EATrax[i].TrackName || !EATrax[i].TrackAlbum || !EATrax[i].TrackArtist)
					{
						ReadEATraxTags_Unknown(track[i].FilePath, i, EATrax);
					}
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
				//EATrax[i].PlaybackMode = (char*)malloc(0x3);
				//EATrax[i].PlaybackMode = inireader.ReadString(IniReaderSectionID, "PlaybackMode", "AL");
				TrackEnables[i] = inireader.ReadInteger(IniReaderSectionID, "TrackEnabled", true);


				// Carbon-specific BS
				SongAttribs[i].TrackName = EATrax[i].TrackName;
				SongAttribs[i].TrackAlbum = EATrax[i].TrackAlbum;
				SongAttribs[i].TrackArtist = EATrax[i].TrackArtist;
				SongAttribs[i].EventID = EATrax[i].EventID;
				SongAttribs[i].maybecategory1 = 2;
				SongAttribs[i].CarClassCategory = 2;


				i++;
			}
		}
	}
	TrackCount = i;

	if (!TrackCount)
		SetNoTracksWarning();
	XNFS_printf(1, "%s: Playlist processing complete\n", PRINT_TYPE_INFO);
	return 0;
}

void SetNoTracksWarning()
{
	XNFS_printf(3, "%s: SetNoTracksWarning() not implemented for Carbon\n", PRINT_TYPE_WARNING);
}

int DoFileNodePathsStruct(const char* TxtFile, int BankNumberToProcess)
{
	FILE *fin;
	unsigned int i = 0;
	int lastcharpos = 0;
	int remainingchars = 0;
	char ReadLine[2048];
	char ReadPath2[2048];
	char* ReadPath;
	fin = fopen(TxtFile, "r");
	if (fin == NULL)
	{
		XNFS_printf(1, "%s: File %s can't be opened for node paths!\n", PRINT_TYPE_ERROR, TxtFile);
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
				sscanf(ReadLine, "%X = %s\n", &node[BankNumberToProcess][i].PathfinderID, ReadPath2);
				ReadPath = strchr(ReadLine, '=');
				remainingchars = strlen(ReadPath);

				for (int i = 0; i < remainingchars; i++)
				{
					ReadPath++;
					if (*ReadPath != 0x20)
						break;
				}

				lastcharpos = strlen(ReadPath) - 1;
				if (ReadPath[lastcharpos] == '\n')
					ReadPath[lastcharpos] = 0;
				node[BankNumberToProcess][i].FilePath = (char*)malloc(strlen(ReadPath) + 1);
				strcpy(node[BankNumberToProcess][i].FilePath, ReadPath);
				switch (CheckIfSupportedFile(node[BankNumberToProcess][i].FilePath))
				{
				case FILE_TYPE_TRACKER:
					node[BankNumberToProcess][i].FileType = FILE_TYPE_TRACKER;
					XNFS_printf(3, "%s: Assigned tracker module %s to %X\n", PRINT_TYPE_INFO, node[BankNumberToProcess][i].FilePath, node[BankNumberToProcess][i].PathfinderID);
					break;

#ifdef BASS_USE_FLAC
				case (FILE_TYPE_FLAC):

					node[BankNumberToProcess][i].FileType = FILE_TYPE_FLAC;
					XNFS_printf(3, "%s: Assigned FLAC stream %s to %X\n", PRINT_TYPE_INFO, node[BankNumberToProcess][i].FilePath, node[BankNumberToProcess][i].PathfinderID);
					break;
#endif
				case (FILE_TYPE_OGG):
					node[BankNumberToProcess][i].FileType = FILE_TYPE_OGG;
					XNFS_printf(3, "%s: Assigned OGG stream %s to %X\n", PRINT_TYPE_INFO, node[BankNumberToProcess][i].FilePath, node[BankNumberToProcess][i].PathfinderID);
					break;

				case (FILE_TYPE_STREAM):
					node[BankNumberToProcess][i].FileType = FILE_TYPE_STREAM;
					XNFS_printf(3, "%s: Assigned streamed track %s to %X\n", PRINT_TYPE_INFO, node[BankNumberToProcess][i].FilePath, node[BankNumberToProcess][i].PathfinderID);
					break;

				case FILE_TYPE_ONLINESTREAM:
					node[BankNumberToProcess][i].FileType = FILE_TYPE_ONLINESTREAM;
					XNFS_printf(3, "%s: Assigned online stream %s to %X\n", PRINT_TYPE_INFO, node[BankNumberToProcess][i].FilePath, node[BankNumberToProcess][i].PathfinderID);
					break;

				case FILE_TYPE_MIDI:
#ifdef BASS_USE_MIDI
					node[BankNumberToProcess][i].FileType = FILE_TYPE_MIDI;
					XNFS_printf(3, "%s: Assigned a MIDI track %s to %X\n", PRINT_TYPE_INFO, node[BankNumberToProcess][i].FilePath, node[BankNumberToProcess][i].PathfinderID);
					break;
#else
					XNFS_printf(1, "%s: Could NOT assign a MIDI track %s -- Compile with BASS_USE_MIDI to enable support.\n", PRINT_TYPE_WARNING, node[BankNumberToProcess][i].FilePath);
#endif
					

				default:
					node[BankNumberToProcess][i].FileType = FILE_TYPE_UNKNOWN;
					XNFS_printf(1, "%s: Assigned unknown type track %s to %X\n", PRINT_TYPE_WARNING, node[BankNumberToProcess][i].FilePath, node[BankNumberToProcess][i].PathfinderID);
				}
				i++;
			}
		}
	}
	fclose(fin);
	XNFS_printf(1, "%s: Node path processing complete\n", PRINT_TYPE_INFO);
	return 0;
}

int HotkeyUpdate()
{
	if ((*(unsigned int*)THEGAMEFLOWMANAGER_ADDRESS == 3) || (*(unsigned int*)THEGAMEFLOWMANAGER_ADDRESS == 6))
	{
		if (Current_SFXObj_PFEATrax && !bInteractiveMode && !*(bool*)SOUNDLOSTWINFOCUS_ADDRESS & bInstallerComplete)
		{
			if ((GetAsyncKeyState(hkTrackSwitch) & 1))
			{
				StartLicensedMusic(Current_SFXObj_PFEATrax, 0);
				while ((GetAsyncKeyState(hkTrackSwitch) & 0x8000) > 0) { Sleep(0); }
			}
		}
	}
	return 0;
}

int InjectCode()
{
	injector::MakeJMP(RESUMECHANNELCAVE_ADDRESS, ResumeChannelCave, true);
	injector::MakeJMP(PAUSECHANNELCAVE_ADDRESS, PauseChannelCave, true);
	injector::MakeJMP(STARTPURSUITMUSCAVE_ADDRESS, StartPursuitMusicCave, true);
	injector::MakeJMP(STARTAMBIENCECAVE_ADDRESS, StartAmbienceCave, true);
	injector::MakeJMP(STARTCREWMUSICCAVE_ADDRESS, StartCrewMusicCave, true);
	injector::MakeJMP(STARTCANYONMUSICCAVE_ADDRESS, StartCanyonMusicCave, true);

	injector::WriteMemory<int>(STOPCHANNELVTABLE_ADDRESS, (int)&StopBassChannel, true);
	injector::WriteMemory<int>(PAUSECHANNELVTABLE_ADDRESS, (int)&PauseBassChannel, true);
	injector::WriteMemory<int>(PLAYCHANNELVTABLE_ADDRESS, (int)&BASSPlayChannel, true);
	injector::MakeJMP(TIMEREMAININGCAVE_ADDRESS, BASSTimeRemainingCave, true);

	if (bUseGameThread)
		injector::MakeCALL(SOUNDUPDATERCAVE_ADDRESS, SoundUpdater, true);

	injector::MakeJMP(NULLPATHFINDERFUNCJMP_ADDRESS, PATHI_getevent, true);

	if (bGameVolumeControl)
	{
		injector::MakeCALL(SETVOLCAVE_ADDRESS, BASSVolume, true);
		injector::MakeJMP(VOLMULTIPLIERCAVE_ADDRESS, BASSVolumeCave, true);
		injector::MakeNOP(VOLCRITICALDISABLE1_ADDRESS, 5, true);
		injector::MakeNOP(VOLCRITICALDISABLE2_ADDRESS, 5, true);
	}

	injector::MakeCALL(QUEUENODECAVE_ADDRESS, QueueNodeInterceptor, true);

	injector::MakeCALL(PATHFINDERCONSTRUCTOR_ADDRESS, BASSCreate, true);
	injector::MakeCALL(PATHFINDERDESTRUCTOR_ADDRESS, BASSDestroy, true);

	injector::WriteMemory<int>(SETFILEPATHHOOK_VTABLE_ADDRESS, (int)&PathTrackSndStream_SetFilePathHook, true);
	injector::MakeJMP(CRAPFIX_1, FixCrappyCarbonCode, true);

	// playlist injection start
	injector::MakeJMP(GETNUMBEROFTRAX_ADDRESS, GetNumberOfEATrax, true);
	//injector::MakeJMP(GETCURRENTSONGINDEX_ADDRESS, FeJukeboxScreen_GetCurrentSongIndex_Hook, true);
	injector::MakeRET(JUKEBOXDEFAULT_ADDRESS, 0, true);

//	injector::MakeNOP(DISABLECLASSESINMENU_ADDRESS, 2, true);
	injector::MakeJMP(INJECTTRACKS1_ADDRESS, InjectTracks1, true);
	injector::MakeJMP(DALOPTIONS_EATRAXPREVIEW_ADDRESS, DALOptions_EATraxPreviewHook, true);
	injector::MakeNOP(DISABLEATTRIBDESTRUCT1_ADDRESS1, 4, true);
	injector::MakeNOP(DISABLEATTRIBDESTRUCT1_ADDRESS2, 5, true);
	injector::MakeJMP(INJECTTRACKS2_ADDRESS, InjectTracks2, true);
	injector::MakeNOP(DISABLEATTRIBDESTRUCT2_ADDRESS, 5, true);
	injector::MakeJMP(INJECTRANDOMTRACK_ADDRESS, RandomTrackInject, true);
	injector::MakeNOP(DISABLEATTRIBDESTRUCT3_ADDRESS1, 4, true);
	injector::MakeNOP(DISABLEATTRIBDESTRUCT3_ADDRESS2, 5, true);
	injector::MakeJMP(INJECTNAME_ADDRESS, InjectTrackNamesInMenu, true);
	injector::MakeJMP(INJECTNAME2_ADDRESS, InjectNames2, true);
	//injector::MakeJMP(EATRAXPLAYABILITY_ADDRESS, DALOptions_IncEATraxSongPlayability_Hook, true);
	injector::MakeJMP(INJECTPADRIGHT_ADDRESS, Inject_FeJukeboxScreen_HandlePadRightLeft, true);
	injector::MakeJMP(INJECTPADLEFT_ADDRESS, Inject_FeJukeboxScreen_HandlePadRightLeft, true);

	if (bDisableInteractive_FE)
	{
		injector::MakeJMP(INJECTUPDATEINGAME_ADDRESS, Inject_UpdateInGame, true);

		injector::WriteMemory<unsigned char>(INJECTTITLESCREEN1_ADDRESS, 0, true);
		injector::MakeCALL(INJECTTITLESCREEN2_ADDRESS, Inject_TitleScreenExit, true);
		injector::MakeJMP(DISABLEINTERACTIVEFE_ADDRESS, DISABLEINTERACTIVE_FORCEEATRAX_ADDRESS, true);
	}

	if (bDisableInteractive_Canyon)
	{
		injector::MakeJMP(DISABLEINTERACTIVECANYON_ADDRESS1, DISABLEINTERACTIVE_FORCEEATRAX_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECANYON_ADDRESS2, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECANYON_ADDRESS3, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECANYON_ADDRESS4, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECANYON_ADDRESS5, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECANYON_ADDRESS6, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
	}

	if (bDisableInteractive_Crew)
	{
		injector::MakeJMP(DISABLEINTERACTIVECREW_ADDRESS1, DISABLEINTERACTIVE_FORCEEATRAX_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECREW_ADDRESS2, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECREW_ADDRESS3, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECREW_ADDRESS4, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECREW_ADDRESS5, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVECREW_ADDRESS6, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
	}

	if (bDisableInteractive_Pursuit)
	{
		injector::MakeJMP(DISABLEINTERACTIVEPURSUIT_ADDRESS1, DISABLEINTERACTIVE_FORCEEATRAX_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVEPURSUIT_ADDRESS2, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVEPURSUIT_ADDRESS3, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVEPURSUIT_ADDRESS4, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVEPURSUIT_ADDRESS5, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
		injector::MakeJMP(DISABLEINTERACTIVEPURSUIT_ADDRESS6, DISABLEINTERACTIVE_FORCERETURN_ADDRESS, true);
	}

	injector::WriteMemory<unsigned char>(MUSICTYPE_ADDRESS_DRIFT, DriftMusicType, true);
	injector::WriteMemory<unsigned char>(MUSICTYPE_ADDRESS_CANYONDUEL, CanyonDuelMusicType, true);
	injector::WriteMemory<unsigned char>(MUSICTYPE_ADDRESS_CANYONSPRINT, CanyonSprintMusicType, true);
	injector::WriteMemory<unsigned char>(MUSICTYPE_ADDRESS_CREWSPRINT, CrewSprintMusicType, true);
	injector::WriteMemory<unsigned char>(MUSICTYPE_ADDRESS_CREWCIRCUIT, CrewCircuitMusicType, true);
	injector::WriteMemory<unsigned char>(MUSICTYPE_ADDRESS_PURSUIT, PursuitMusicType, true);
	injector::WriteMemory<unsigned char>(MUSICTYPE_ADDRESS_DEFAULT, DefaultMusicType, true);

	// hotkey stuff
	injector::MakeJMP(STEALSFXOBJ_ADDRESS, StealSFXObj, true);

	//injector::MakeJMP(0x8570C7, 0x8570EE, true);
	//injector::MakeJMP(0x8570FE, 0x85711F, true);


	//injector::WriteMemory<unsigned char>(INJECTNAME_FIXSTACK_ADDRESS, 0x8, true);
	//injector::MakeNOP(INJECTNAME2_ADDRESS, 7, true);


	XNFS_printf(1, "%s: Code injection complete.\n", PRINT_TYPE_INFO);

	return 0;
}

int InitConfig_GameSpecific()
{
	CIniReader inireader("");

	NodePathConfigFile[0] = inireader.ReadString("Playlists", "NodePathConfigFile0", DEFAULT_NODEPATHS_PATH_0);
	NodePathConfigFile[1] = inireader.ReadString("Playlists", "NodePathConfigFile1", DEFAULT_NODEPATHS_PATH_1);
	NodePathConfigFile[2] = inireader.ReadString("Playlists", "NodePathConfigFile2", DEFAULT_NODEPATHS_PATH_2);
	NodePathConfigFile[3] = inireader.ReadString("Playlists", "NodePathConfigFile3", DEFAULT_NODEPATHS_PATH_3);

	bSequencedPlayback = inireader.ReadInteger("Playback", "SequencedPlayback", 1);
	bDisableInteractive_FE = inireader.ReadInteger("Playback", "DisableInteractive_FE", 0);
	bDisableInteractive_Crew = inireader.ReadInteger("Playback", "DisableInteractive_Crew", 0);
	bDisableInteractive_Canyon = inireader.ReadInteger("Playback", "DisableInteractive_Canyon", 0);
	bDisableInteractive_Pursuit = inireader.ReadInteger("Playback", "DisableInteractive_Pursuit", 0);

	DriftMusicType = inireader.ReadInteger("MusicTypes", "DriftMusicType", 2) % 0xFF;
	CanyonDuelMusicType = inireader.ReadInteger("MusicTypes", "CanyonDuelMusicType", 16) % 0xFF;
	CanyonSprintMusicType = inireader.ReadInteger("MusicTypes", "CanyonSprintMusicType", 16) % 0xFF;
	CrewSprintMusicType = inireader.ReadInteger("MusicTypes", "CrewSprintMusicType", 8) % 0xFF;
	CrewCircuitMusicType = inireader.ReadInteger("MusicTypes", "CrewCircuitMusicType", 8) % 0xFF;
	PursuitMusicType = inireader.ReadInteger("MusicTypes", "PursuitMusicType", 4) % 0xFF;
	DefaultMusicType = inireader.ReadInteger("MusicTypes", "DefaultMusicType", 1) % 0xFF;

	if (DriftMusicType > 2)
		DriftMusicType = (1 << (DriftMusicType - 1)) % 0x11;

	if (CanyonDuelMusicType > 2)
		CanyonDuelMusicType = (1 << (CanyonDuelMusicType - 1)) % 0x11;

	if (CanyonSprintMusicType > 2)
		CanyonSprintMusicType = (1 << (CanyonSprintMusicType - 1)) % 0x11;

	if (CrewSprintMusicType > 2)
		CrewSprintMusicType = (1 << (CrewSprintMusicType - 1)) % 0x11;

	if (CrewCircuitMusicType > 2)
		CrewCircuitMusicType = (1 << (CrewCircuitMusicType - 1)) % 0x11;

	if (PursuitMusicType > 2)
		PursuitMusicType = (1 << (PursuitMusicType - 1)) % 0x11;

	if (DefaultMusicType > 2)
		DefaultMusicType = (1 << (DefaultMusicType - 1)) % 0x11;

	if (bEnableInteractiveNoding && bInstallerComplete)
	{
		for (unsigned int i = 0; i <= 3; i = i++)
		{
			XNFS_printf(1, "%s Processing NodeID paths for bank %d\n", PRINT_TYPE_INFO, i);
			DoFileNodePathsStruct(NodePathConfigFile[i], i);
		}
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
	NullPathfinder.Unknown10 = 0x00FF0000;
	//NullPathfinder.Unknown10 = 0xFF05CF;
	return 0;
}
