// Macro definitions for XNFSMusicPlayer
#pragma once
#pragma warning(disable:4800 4244 4305)
#include "bass.h"
#include "stdafx.h"
#include "stdio.h"

#ifdef GAME_MW
#include "MW_MusicAddresses.h"
#endif

#ifdef GAME_CARBON
#include "Carbon_MusicAddresses.h"
#endif

#ifdef XNFSMP_TESTBUILD
#define XNFS_TESTBUILD
#endif

#define BUILD_VER_MAJOR 1
#define BUILD_VER_MINOR 8

#define FILE_TYPE_TRACKER 1
#define FILE_TYPE_STREAM 2
#define FILE_TYPE_ONLINESTREAM 3
//#define FILE_TYPE_VGMSTREAM 4
#define FILE_TYPE_FLAC 5
#define FILE_TYPE_MIDI 6
#define FILE_TYPE_OGG 7

#define PRINT_TYPE_INFO "INFO"
#define PRINT_TYPE_WARNING "WARNING"
#define PRINT_TYPE_ERROR "ERROR"

#define CRC24_INIT 0xB704CEL
#define CRC24_POLY 0x1864CFBL

#define DEFAULT_PLAYLIST_PATH "scripts\\XNFSMusicPlayer\\Playlist.m3u"
#define DEFAULT_PLAYLISTCONFIG_PATH "scripts\\XNFSMusicPlayer\\PlaylistConfig.ini"

#define DEFAULT_OVERRIDES_PATH "scripts\\XNFSMusicPlayer\\Overrides.txt"
#define DEFAULT_FILE_PATH "scripts\\XNFSMusicPlayer\\default.ogg"
#define DEFAULT_LOG_PATH "scripts\\XNFSMusicPlayer\\XNFSMusicPlayer.log"

#define INSTALL_MUSICID 0x494E53
#define INSTALL_MUSIC_FILE "scripts\\XNFSMusicPlayer\\install.ogg"

/*#ifdef BASS_USE_VGMSTREAM
#include "bass_vgmstream.h"
#endif*/

#ifdef BASS_USE_FLAC
#include "bassflac.h"
#endif

#ifdef BASS_USE_MIDI
#include "bassmidi.h"
//#include "sflist.h" // Needs work.
char* SoundFontFilePath = "default.sf2";
bool bPreloadSoundfont = 0;
//sflist_presets * presetlist;
BASS_MIDI_FONT fontstruct[1];
HSOUNDFONT fonthandle;
//bool bEnableBASSMIDI = 1;
#endif

#include "stdio.h"

struct PathfinderStruct
{
	unsigned int Unknown1;
	unsigned int Unknown2;
	unsigned int Unknown3;
	unsigned int EventID;
	unsigned int Unknown4;
	unsigned int Unknown5; // 0x11000001
	unsigned int Unknown6; // 0x000F0400
	unsigned int Unknown7; // 0x01FFFFFF
	unsigned int Unknown8; // = unk5
	unsigned int Unknown9; // 0x00030400
	unsigned int Unknown10;// 0x00FF05CF
};

struct TrackStruct
{
	unsigned int PathfinderID;
	unsigned int FileType;
	int Loop;
	unsigned int LoopStart;
	unsigned int LoopEnd;
	float ModuleAmpLevel;
	char* FilePath;
};

struct NodeStruct
{
	unsigned int PathfinderID;
	unsigned int FileType;
	char* FilePath;
};

struct JukeboxTrack
{
	char* TrackName;
	char* TrackAlbum;
	char* TrackArtist;
	char* PlaybackMode;
	unsigned int EventID; //01EAF18B
};

struct SaveGameTrax
{
	unsigned int TrackNumber;
	unsigned char TrackMode;
	unsigned char UnknownCD[3]; // 0xCD x3
};

unsigned int VerbosityLevel;
bool bLogFileEnabled = 0;
bool bLogFileCreated = 0;
char* LogFilePath;
FILE* LogFile;

const char SupportedTrackerFormats[14][5] =
{
	{ ".xm" },{ ".mo3" },{ ".mod" },{ ".s3m" },{ ".it" },{ ".mtm" },{ ".umx" },
	{ ".XM" },{ ".MO3" },{ ".MOD" },{ ".S3M" },{ ".IT" },{ ".MTM" },{ ".UMX" }
};

const char SupportedStreamingFormats[14][6] =
{
	{ ".ogg" },{ ".mp3" },{ ".wav" },{ ".flac" },{ ".mp2" },{ ".mp1" },{ ".aiff" },
	{ ".OGG" },{ ".MP3" },{ ".WAV" },{ ".FLAC" },{ ".MP2" },{ ".MP1" },{ ".AIFF" }
};

/*const char SupportedVGMStreamFormats[][8] = // Not all play properly (looped ones play infinitely)
{
	{ ".asf" },{ ".aax" },{ ".adx" },{ ".aix" },{ ".lwav" },{ ".ads" },{ ".ass" },{ ".ast" },{ ".bg00" },{ ".bmdx" },
	{ ".ccc" },{ ".cnk" },{ ".dxh" },{ ".enth" },{ ".fag" },{ ".filp" },{ ".gcm" },{ ".gms" },{ ".hgc1" },{ ".ikm" },{ ".ild" },
	{ ".ivb" },{ ".joe" },{ ".kces" },{ ".khv" },{ ".leg" },{ ".mcg" },{ ".mib" },{ ".mic" },{ ".mihb" },{ ".msa" },{ ".msvp" },
	{ ".musc" },{ ".npsf" },{ ".pnb" },{ ".psh" },{ ".rkv" },{ ".rnd" },{ ".rstm" },{ ".rws" },{ ".rxw" },{ ".snd" },{ ".sfs" },
	{ ".sl3" },{ ".smpl" },{ ".ster" },{ ".str" },{ ".sts" },{ ".svag" },{ ".svs" },{ ".tec" },{ ".tk5" },{ ".vas" },{ ".vag" },
	{ ".vgs" },{ ".vig" },{ ".vpk" },{ ".vs" },{ ".vsf" },{ ".wp2" },{ ".xa2" },{ ".xa30" },{ ".aaap" },{ ".agsc" },{ ".amts" },
	{ ".asr" },{ ".bns" },{ ".bo2" },{ ".capdsp" },{ ".cfn" },{ ".dds" },{ ".dsp" },{ ".gca" },{ ".gsp" },{ ".hps" },{ ".idsp" },
	{ ".ish" },{ ".lps" },{ ".mca" },{ ".mpdsp" },{ ".mss" },{ ".mus" },{ ".ndp" },{ ".pdt" },{ ".sdt" },{ ".smp" },{ ".sns" },
	{ ".spt" },{ ".ssm" },{ ".stm" },{ ".swd" },{ ".thp" },{ ".tydsp" },{ ".vjdsp" },{ ".waa" },{ ".wac" },{ ".wad" },{ ".wam" },
	{ ".was" },{ ".wsd" },{ ".wsi" },{ ".ydsp" },{ ".ymf" },{ ".zwdsp" },{ ".asd" },{ ".baka" },{ ".bh2pcm" },{ ".dmsg" },
	{ ".gcsw" },{ ".gcw " },{ ".his" },{ ".int" },{ ".pcm" },{ ".kraw" },{ ".raw" },{ ".rwx" },{ ".sap" },{ ".sps" },{ ".xss" },
	{ ".voi" },{ ".wb" },{ ".zsd" },{ ".matx" },{ ".wavm" },{ ".wvs" },{ ".xmu" },{ ".xvas" },{ ".xwav" },{ ".adpcm" },{ ".spsd" },
	{ ".bar" },{ ".dvi" },{ ".hwas" },{ ".idvi" },{ ".ivaud" },{ ".myspd" },{ ".stma" },{ ".strm" },{ ".aifc" },{ ".as4" },
	{ ".aud" },{ ".aus" },{ ".brstm" },{ ".emff" },{ ".fsb" },{ ".wii" },{ ".genh" },{ ".msf" },{ ".musx" },{ ".nwa" },{ ".psw" },
	{ ".rwar" },{ ".rwsd" },{ ".rsd" },{ ".rrds" },{ ".sad" },{ ".sgd" },{ ".sgb" },{ ".sgx" },{ ".seg" },{ ".sng" },{ ".eam" },
	{ ".ss7" },{ ".swav" },{ ".xwb" },{ ".2dx9" },{ ".acm" },{ ".adp" },{ ".afc" },{ ".baf" },{ ".bgw" },
	{ ".bnsf" },{ ".caf" },{ ".de2" },{ ".hca" },{ ".kcey" },{ ".lsf" },{ ".mwv" },{ ".mtaf" },{ ".p3d" },{ ".rsf" },{ ".sab" },
	{ ".s14" },{ ".sc" },{ ".scd" },{ ".sd9" },{ ".spw" },{ ".stx" },{ ".um3" },{ ".xa" },
};*/

const char SupportedMIDIFormats[6][6] =
{
	{ ".mid" },{ ".midi" },{ ".rmi" },
	{ ".MID" },{ ".MIDI" },{ ".RMI" }
};

const char SupportedOnlineStreamFormats[3][9] =
{
	{ "http://" },
	{ "https://" },
	{ "ftp://" }
};

const char FormatTypeStrings[8][32] =
{
	{ "Unknown" },
	{ "Tracker module" },
	{ "Stream" },
	{ "Online stream" },
	{ "VGMStream stream" },
	{ "FLAC stream" },
	{ "MIDI" },
	{ "OGG stream" }
};


int __stdcall XNFS_printf(unsigned int Level, const char* Format, ...)
{
	va_list ArgList;
	int Result = 0;
	if ((Level <= VerbosityLevel) && VerbosityLevel)
	{
		__crt_va_start(ArgList, Format);
		Result = vfprintf(stdout, Format, ArgList);
		if (bLogFileEnabled)
		{
			if (!bLogFileCreated)
			{
				LogFile = fopen(LogFilePath, "w");
				bLogFileCreated = true;
			}
			else
				LogFile = fopen(LogFilePath, "a");
			if (LogFile == NULL) 
			{
				if (bLogFileCreated)
					bLogFileCreated = false;
				perror(PRINT_TYPE_ERROR);
				__crt_va_end(ArgList);
				return Result;
			}
			vfprintf(LogFile, Format, ArgList);
			if (LogFile)
				fclose(LogFile);
		}
		__crt_va_end(ArgList);
	}
	return Result;
}

typedef long crc24;
crc24 crc_octets(unsigned char *octets, size_t len)
{
	crc24 crc = CRC24_INIT;
	int i;
	while (len--) {
		crc ^= (*octets++) << 16;
		for (i = 0; i < 8; i++) {
			crc <<= 1;
			if (crc & 0x1000000)
				crc ^= CRC24_POLY;
		}
	}
	return crc & 0xFFFFFFL;
}
