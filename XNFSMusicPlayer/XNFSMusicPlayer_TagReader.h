#pragma once
#include <stdio.h>
#include <string.h>
#include "bass.h"
#include "XNFSMusicPlayer.h"

#define CHUNKID_RIFF_INFO 0x4F464E49
#define CHUNKID_RIFF_LIST 0x5453494C
#define CHUNKID_RIFF_INAM 0x4D414E49
#define CHUNKID_RIFF_TITL 0x4C544954
#define CHUNKID_RIFF_IPRD 0x44525049
#define CHUNKID_RIFF_IART 0x54524149
#define CHUNKID_RIFF_ID3 0x20336469
#define CHUNKID_RIFF_RIFF 0x46464952
#define CHUNKID_ID3 0x03334449

void ReadFilenameAsTrackName(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	char JukeStuff[255];
	const char *pch1;
	const char *pch2;

	memset(JukeStuff, 0, 255);

	pch1 = strrchr(FilePath, '\\');
	pch2 = strrchr(FilePath, '.');

	if (pch1 == NULL)
		pch1 = FilePath - 1;

	strncpy(JukeStuff, pch1 + 1, pch2 - pch1 - 1);
	EATrax[TrackNumber].TrackName = (char*)malloc(sizeof(JukeStuff) + 1);
	strcpy(EATrax[TrackNumber].TrackName, JukeStuff);
}

void ReadLastFolderAsAlbum(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	char JukeStuff[255];
	const char *pch1;
	const char *pch2;
	char Buffer2[255];
	const char *pch3;

	pch1 = strrchr(FilePath, '\\');
	pch2 = strrchr(FilePath, '.');

	memset(JukeStuff, 0, 255);
	if ((pch1 == NULL) || (pch2 == NULL))
		strcpy(JukeStuff, " \0");
	else
	{

		memset(Buffer2, 0, 255);
		strncpy(Buffer2, FilePath, pch1 - FilePath);
		pch3 = strrchr(Buffer2, '\\');
		if (pch3 == NULL)
		{
			strncpy(JukeStuff, FilePath, pch1 - FilePath);
		}
		else
			strcpy(JukeStuff, pch3 + 1);
	}
	EATrax[TrackNumber].TrackAlbum = (char*)malloc(strlen(JukeStuff) + 1);
	strcpy(EATrax[TrackNumber].TrackAlbum, JukeStuff);
}

void ReadTrackNumberAsArtist(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	char JukeStuff[255];

	memset(JukeStuff, 0, 255);

	sprintf(JukeStuff, "Track %d\0", TrackNumber + 1);
	EATrax[TrackNumber].TrackArtist = (char*)malloc(strlen(JukeStuff) + 1);
	strcpy(EATrax[TrackNumber].TrackArtist, JukeStuff);
}

bool ReadEATraxTags_RIFFInfo(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	char PathBuffer[1024]; // try to make a function out of this later
	strcpy(PathBuffer, FilePath);
	FILE *fin = fopen(PathBuffer, "rb");
	if (fin == NULL)
	{
		XNFS_printf(1, "%s: Couldn't open %s\n", PRINT_TYPE_ERROR, PathBuffer);
		strcpy(PathBuffer, "..\\");
		strcat(PathBuffer, FilePath);
		fin = fopen(PathBuffer, "rb");
	}
	if (fin == NULL)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		XNFS_printf(1, "%s: Couldn't open %s\n", PRINT_TYPE_ERROR, PathBuffer);
		return 0;
	}

	bool bFoundTitle = 0;
	bool bFoundAlbum = 0;
	bool bFoundArtist = 0;
	bool bFoundList = 0;

	unsigned int ChunkSize = 0;
	unsigned int ChunkID = 0;
	unsigned int FileSize = 0;
	unsigned int ListPos = 0;
	unsigned int RelativeChunkEnd = 0;
	
	// first we find the "LIST" chunk - we do that by skipping chunks until we find it...
	fseek(fin, 4, SEEK_SET);
	fread(&FileSize, sizeof(int), 1, fin);
	FileSize += 8;
	// skip header info
	fseek(fin, 8, SEEK_CUR);
	fread(&ChunkSize, sizeof(int), 1, fin);
	fseek(fin, ChunkSize, SEEK_CUR);

	// start searching chunks
	while (ftell(fin) < FileSize)
	{
		fread(&ChunkID, sizeof(int), 1, fin);
		fread(&ChunkSize, sizeof(int), 1, fin);
		if (ChunkID == CHUNKID_RIFF_LIST)
		{
			bFoundList = true;
			break;
		}
		fseek(fin, ChunkSize, SEEK_CUR);
	}

	if (ftell(fin) < FileSize)
	{
		RelativeChunkEnd = ftell(fin) + ChunkSize;
		while (ftell(fin) < RelativeChunkEnd)
		{
			fread(&ChunkID, sizeof(int), 1, fin);
			if (ChunkID != CHUNKID_RIFF_INFO)
			{
				fread(&ChunkSize, sizeof(int), 1, fin);

				if ((ChunkID == CHUNKID_RIFF_INAM) || (ChunkID == CHUNKID_RIFF_TITL) && (!bFoundTitle || !EATrax[TrackNumber].TrackName))
				{
					bFoundTitle = 1;
					EATrax[TrackNumber].TrackName = (char*)malloc(ChunkSize);
					fread(EATrax[TrackNumber].TrackName, ChunkSize, 1, fin);
				}
				else if ((ChunkID == CHUNKID_RIFF_IPRD) && (!bFoundAlbum || !EATrax[TrackNumber].TrackAlbum))
				{
					bFoundAlbum = 1;
					EATrax[TrackNumber].TrackAlbum = (char*)malloc(ChunkSize);
					fread(EATrax[TrackNumber].TrackAlbum, ChunkSize, 1, fin);
				}
				else if ((ChunkID == CHUNKID_RIFF_IART) && (!bFoundArtist || !EATrax[TrackNumber].TrackArtist))
				{
					bFoundArtist = 1;
					EATrax[TrackNumber].TrackArtist = (char*)malloc(ChunkSize);
					fread(EATrax[TrackNumber].TrackArtist, ChunkSize, 1, fin);
				}
				else
					fseek(fin, ChunkSize, SEEK_CUR);

				if (bFoundTitle && bFoundAlbum && bFoundArtist)
					break;
			}
			if (bFoundTitle && bFoundAlbum && bFoundArtist)
				break;
		}
	}

	fclose(fin);

	if ((!bFoundTitle || !bFoundAlbum || !bFoundArtist) && bFoundList)
	{
		if (!bFoundTitle && !EATrax[TrackNumber].TrackName)
			ReadFilenameAsTrackName(FilePath, TrackNumber, EATrax);
		if (!bFoundAlbum && !EATrax[TrackNumber].TrackAlbum)
			ReadLastFolderAsAlbum(FilePath, TrackNumber, EATrax);
		if (!bFoundArtist && !EATrax[TrackNumber].TrackArtist)
			ReadTrackNumberAsArtist(FilePath, TrackNumber, EATrax);
	}

	return bFoundList;
}

bool ReadVorbisComments(FILE* finput, int TrackNumber, const char* FilePath, JukeboxTrack* EATrax, TrackStruct* track)
{
	//	bool bFoundAlbumArtist = 0;
	bool bFoundArtist = 0;
	bool bFoundTitle = 0;
	bool bFoundAlbum = 0;
	bool bFoundLoop = 0;
	bool bFoundOneAlready = 0;

	unsigned int vendor_length = 0;
	unsigned int NumberOfComments = 0;
	unsigned int CommentSize = 0;

	char* ReadComment = NULL;
	char* pch1;

	fread(&vendor_length, 4, 1, finput);
	fseek(finput, vendor_length, SEEK_CUR);
	fread(&NumberOfComments, 4, 1, finput);

	if (!NumberOfComments)
		return 0;

	for (unsigned int i = 0; i < NumberOfComments; i++)
	{
		fread(&CommentSize, 4, 1, finput);
		ReadComment = (char*)malloc(CommentSize + 1);
		memset(ReadComment, 0, CommentSize);
		fread(ReadComment, CommentSize, sizeof(char), finput);
		memset(ReadComment + CommentSize, 0, 1);

		pch1 = strchr(ReadComment, '=');

		if ((strncmp(ReadComment, "TITLE", 5) == 0) && !bFoundOneAlready)
		{
			bFoundTitle = 1;
			bFoundOneAlready = 1;
			if ((strcmp(pch1 + 1, "") == 0) || (strncmp(pch1 + 1, "\0", 1) == 0))
			{
				bFoundTitle = 0;
			}
			else
			{
				EATrax[TrackNumber].TrackName = (char*)malloc(strlen(pch1 + 1));
				strcpy(EATrax[TrackNumber].TrackName, pch1 + 1);
				if ((strcmp(EATrax[TrackNumber].TrackName, "") == 0) || (strcmp(EATrax[TrackNumber].TrackName, "\0") == 0))
					bFoundTitle = 0;
			}
		}

		if ((strncmp(ReadComment, "ALBUM=", 6) == 0) && !bFoundOneAlready)
		{
			bFoundAlbum = 1;
			bFoundOneAlready = 1;
			if ((strcmp(pch1 + 1, "") == 0) || (strcmp(pch1 + 1, "\0") == 0))
			{
				bFoundAlbum = 0;
			}
			else
			{
				EATrax[TrackNumber].TrackAlbum = (char*)malloc(strlen(pch1 + 1));
				strcpy(EATrax[TrackNumber].TrackAlbum, pch1 + 1);
				if ((strcmp(EATrax[TrackNumber].TrackAlbum, "") == 0) || (strcmp(EATrax[TrackNumber].TrackAlbum, "\0") == 0))
					bFoundAlbum = 0;
			}
		}

		if ((strncmp(ReadComment, "ARTIST", 6) == 0) && !bFoundOneAlready)
		{
			bFoundArtist = 1;
			bFoundOneAlready = 1;
			if ((strcmp(pch1 + 1, "") == 0) || (strcmp(pch1 + 1, "\0") == 0))
			{
				bFoundArtist = 0;
			}
			else
			{
				EATrax[TrackNumber].TrackArtist = (char*)malloc(strlen(pch1 + 1));
				strcpy(EATrax[TrackNumber].TrackArtist, pch1 + 1);
				if ((strcmp(EATrax[TrackNumber].TrackArtist, "") == 0) || (strcmp(EATrax[TrackNumber].TrackArtist, "\0") == 0))
				{
					bFoundArtist = 0;
				}
			}
		}

		if ((strncmp(ReadComment, "LOOPSTART", 0x9) == 0) && !bFoundOneAlready)
		{
			bFoundLoop = 1;
			bFoundOneAlready = 1;
			if (!((strcmp(pch1 + 1, "") == 0) || (strncmp(pch1 + 1, "\0", 1) == 0)))
				_snscanf(pch1 + 1, ((unsigned int)(pch1 + 1) - CommentSize), "%d", &track[TrackNumber].LoopStart);
		}
		if ((strncmp(ReadComment, "LOOPEND", 0x7) == 0) && !bFoundOneAlready)
		{
			bFoundOneAlready = 1;
			if (!((strcmp(pch1 + 1, "") == 0) || (strncmp(pch1 + 1, "\0", 1) == 0)))
				_snscanf(pch1 + 1, ((unsigned int)(pch1 + 1) - CommentSize), "%d", &track[TrackNumber].LoopEnd);
		}

		if ((strncmp(ReadComment, "LOOP_START", 10) == 0) && !bFoundOneAlready)
		{
			bFoundLoop = 1;
			bFoundOneAlready = 1;
			if (!((strcmp(pch1 + 1, "") == 0) || (strncmp(pch1 + 1, "\0", 1) == 0)))
				_snscanf(pch1 + 1, ((unsigned int)(pch1 + 1) - CommentSize), "%d", &track[TrackNumber].LoopStart);
		}
		if ((strncmp(ReadComment, "LOOP_END", 8) == 0) && !bFoundOneAlready)
		{
			bFoundOneAlready = 1;
			if (!((strcmp(pch1 + 1, "") == 0) || (strncmp(pch1 + 1, "\0", 1) == 0)))
				_snscanf(pch1 + 1, ((unsigned int)(pch1 + 1) - CommentSize), "%d", &track[TrackNumber].LoopEnd);
		}

		if (bFoundLoop)
			track[TrackNumber].Loop = 1;

		bFoundOneAlready = 0;
		free(ReadComment);
	}

	if (!bFoundTitle && !bFoundAlbum && !bFoundArtist)
	{
#ifdef _NDEBUG
		free(ReadComment);
#endif
		return 0;
	}

	if (!bFoundTitle || !bFoundAlbum || !bFoundArtist)
	{
		if (!bFoundTitle)
			ReadFilenameAsTrackName(FilePath, TrackNumber, EATrax);
		if (!bFoundAlbum)
			ReadLastFolderAsAlbum(FilePath, TrackNumber, EATrax);
		if (!bFoundArtist)
			ReadTrackNumberAsArtist(FilePath, TrackNumber, EATrax);
	}

	return 1;
}

bool ReadEATraxTags_OGG(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax, TrackStruct* track)
{
	bool ret = 0;
	unsigned char NumberOfSegments = 1;
	unsigned char SegmentTable[255];
	char VorbisVorbis[6];

	FILE *fin = fopen(FilePath, "rb");
	if (fin == NULL)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		return 0;
	}
	fseek(fin, 0x1A, SEEK_SET);
	fread(&NumberOfSegments, 1, 1, fin);
	fread(&SegmentTable, sizeof(char), NumberOfSegments, fin);

	for (unsigned int i = 0; i < NumberOfSegments; i++) // multi page, even tho it won't work :/
	{
		fseek(fin, SegmentTable[i], SEEK_CUR);
		while (1)
		{
			fread(VorbisVorbis, sizeof(char), 6, fin);
			if ((strncmp(VorbisVorbis, "vorbis", 6) == 0) || feof(fin))
				break;
			fseek(fin, -5, SEEK_CUR);
		}
		//fseek(fin, 6, SEEK_CUR);
		ret = ReadVorbisComments(fin, TrackNumber, FilePath, EATrax, track);
	}

	fclose(fin);
	return ret;
}

bool ReadEATraxTags_FLAC(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax, TrackStruct* track)
{
	FILE *fin = fopen(FilePath, "rb");

	unsigned char BlockType = 0;
	unsigned char TriByte[3];
	unsigned int BlockSize = 0;
	bool ret = 0;

	if (fin == NULL)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		return 0;
	}

	fseek(fin, 4, SEEK_SET);
	while (!feof(fin))
	{
		fread(&BlockType, 1, 1, fin);
		fread(&TriByte, 1, 3, fin);

		BlockSize = TriByte[0] << 16;
		BlockSize += TriByte[1] << 8;
		BlockSize += TriByte[2];

		if (BlockType == 4)
		{
			ret = ReadVorbisComments(fin, TrackNumber, FilePath, EATrax, track);
			break;
		}
		fseek(fin, BlockSize, SEEK_CUR);
	}

	fclose(fin);

	return ret;
}

/*bool __stdcall ReadEATraxTags_Module(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax, DWORD music)
{
	char PathBuffer[1024];
	char* TheBassTag = 0;
	strcpy(PathBuffer, FilePath);
	FILE *fin = fopen(PathBuffer, "rb");
	if (fin == NULL)
	{
		strcpy(PathBuffer, "..\\");
		strcat(PathBuffer, FilePath);
		fin = fopen(PathBuffer, "rb");
	}
	if (fin == NULL)
	{
		perror(PRINT_TYPE_ERROR);
		XNFS_printf(1, "%s: Couldn't open %s\n", PRINT_TYPE_ERROR, PathBuffer);
		return 0;
	}
	fclose(fin);
	music = BASS_MusicLoad(0, PathBuffer, 0, 0, 0, 0);
	if (!music)
	{
		XNFS_printf(1, "%s: Can't load module during tag reading! BASS Error: %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		return 0;
	}
	if (!EATrax[TrackNumber].TrackName)
		TheBassTag = (char*)BASS_ChannelGetTags(music, BASS_TAG_MUSIC_NAME);
	if (!TheBassTag)
		ReadFilenameAsTrackName(FilePath, TrackNumber, EATrax);
	else
	{
		EATrax[TrackNumber].TrackName = (char*)malloc(strlen(TheBassTag));
		strcpy(EATrax[TrackNumber].TrackName, TheBassTag);
	}
	if (!EATrax[TrackNumber].TrackAlbum)
		ReadLastFolderAsAlbum(FilePath, TrackNumber, EATrax);
	if (!EATrax[TrackNumber].TrackArtist)
		TheBassTag = (char*)BASS_ChannelGetTags(music, BASS_TAG_MUSIC_AUTH);
	if (!TheBassTag)
		ReadTrackNumberAsArtist(FilePath, TrackNumber, EATrax);
	else
	{
		EATrax[TrackNumber].TrackArtist = (char*)malloc(strlen(TheBassTag));
		strcpy(EATrax[TrackNumber].TrackArtist, TheBassTag);
	}

	BASS_MusicFree(music);
	return 1;
}*/

/*void ID3v1TrackArtistReading(TAG_ID3* id3, const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	EATrax[TrackNumber].TrackArtist = (char*)malloc(30);
	strcpy(EATrax[TrackNumber].TrackArtist, id3->artist);
	if ((strcmp(EATrax[TrackNumber].TrackArtist, "") == 0) || (strcmp(EATrax[TrackNumber].TrackArtist, "\0") == 0))
	{
		free(EATrax[TrackNumber].TrackArtist);
		ReadTrackNumberAsArtist(FilePath, TrackNumber, EATrax);
	}
}

void ID3v1TrackAlbumReading(TAG_ID3* id3, const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	EATrax[TrackNumber].TrackAlbum = (char*)malloc(30);
	strcpy(EATrax[TrackNumber].TrackAlbum, id3->album);
	if ((strcmp(EATrax[TrackNumber].TrackAlbum, "") == 0) || (strcmp(EATrax[TrackNumber].TrackAlbum, "\0") == 0))
	{
		free(EATrax[TrackNumber].TrackAlbum);
		ReadLastFolderAsAlbum(FilePath, TrackNumber, EATrax);
	}
}

void ID3v1TrackNameReading(TAG_ID3* id3, const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	EATrax[TrackNumber].TrackName = (char*)malloc(30);
	strcpy(EATrax[TrackNumber].TrackName, id3->title);
	if ((strcmp(EATrax[TrackNumber].TrackName, "") == 0) || (strcmp(EATrax[TrackNumber].TrackName, "\0") == 0))
	{
		free(EATrax[TrackNumber].TrackName);
		ReadFilenameAsTrackName(FilePath, TrackNumber, EATrax);
	}
}*/

void ConvertWideToMultiByteTag(const char* CurrentBlock, unsigned int &FrameSize, char* TheTagString)
{
	wchar_t *WideString;
	FrameSize &= 0x00FFFFFF;
	FrameSize -= 3;
	WideString = (wchar_t*)malloc(FrameSize + 1);
	memset((void*)WideString, 0, FrameSize + 1);
	memcpy(WideString, CurrentBlock + 0xD, FrameSize);
	wcstombs(TheTagString, WideString, FrameSize);
	free(WideString);
}

bool ReadEATraxTags_ID3v2(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	const char* id3v2 = NULL;
	const char id3v2_temp[10] = { 0 };

	unsigned int ID3Magic = 0;
	unsigned int ReadFrame = 0;
	unsigned int FrameSize = 0;
	unsigned int id3v2size = 0;

	bool bFoundID3 = 0;
	bool bFoundTitle = 0;
	bool bFoundAlbum = 0;
	bool bFoundArtist = 0;

	const char *CurrentBlock, *EndPointer = NULL;

	unsigned int ChunkSize = 0;
	unsigned int ChunkID = 0;
	unsigned int FileSize = 0;

	char PathBuffer[1024];
	strcpy(PathBuffer, FilePath);
	FILE *fin = fopen(PathBuffer, "rb");

	if (fin == NULL)
	{
		strcpy(PathBuffer, "..\\");
		strcat(PathBuffer, FilePath);
		fin = fopen(PathBuffer, "rb");
	}
	if (fin == NULL)
	{
		if (bConsoleExists)
			perror(PRINT_TYPE_ERROR);
		XNFS_printf(1, "%s: Couldn't open %s\n", PRINT_TYPE_ERROR, PathBuffer);
		return 0;
	}

	// check if we're in a RIFF file first...
	fread(&ChunkID, sizeof(int), 1, fin);

	if (ChunkID == CHUNKID_RIFF_RIFF)
	{
		// looking for RIFF id3 chunk...
		fseek(fin, 4, SEEK_SET);
		fread(&FileSize, sizeof(int), 1, fin);
		FileSize += 8;
		// skip header info
		fseek(fin, 8, SEEK_CUR);
		fread(&ChunkSize, sizeof(int), 1, fin);
		fseek(fin, ChunkSize, SEEK_CUR);

		// start searching chunks
		while (ftell(fin) < FileSize)
		{
			fread(&ChunkID, sizeof(int), 1, fin);
			fread(&ChunkSize, sizeof(int), 1, fin);
			if (ChunkID == CHUNKID_RIFF_ID3)
			{
				bFoundID3 = true;
				break;
			}
			fseek(fin, ChunkSize, SEEK_CUR);
		}

		if (bFoundID3)
		{
			id3v2 = (char*)malloc(ChunkSize);
			fread((void*)id3v2, ChunkSize, 1, fin);
		}
	}
	else if (ChunkID == CHUNKID_ID3)
	{
		bFoundID3 = true;
		fseek(fin, 0, SEEK_SET);
		fread((void*)id3v2_temp, sizeof(char), 10, fin);
		int Footer = id3v2_temp[5] & 0x10;
		id3v2size = id3v2_temp[6] << 21;
		id3v2size += id3v2_temp[7] << 14;
		id3v2size += id3v2_temp[8] << 7;
		id3v2size += id3v2_temp[9];
		id3v2size += 10;
		if (Footer)
			id3v2size += 10;

		id3v2 = (char*)malloc(id3v2size);
		fseek(fin, 0, SEEK_SET);
		fread((void*)id3v2, id3v2size, 1, fin);
	}

	fclose(fin);

	if (id3v2)
	{
		int Footer = id3v2[5] & 0x10;
		id3v2size = id3v2[6] << 21;
		id3v2size += id3v2[7] << 14;
		id3v2size += id3v2[8] << 7;
		id3v2size += id3v2[9];
		id3v2size += 10;
		if (Footer)
			id3v2size += 10;

		EndPointer = (const char*)((unsigned int)id3v2 + (unsigned int)id3v2size);
	}
	else
		return 0;

	if (id3v2 && (!EATrax[TrackNumber].TrackName || !EATrax[TrackNumber].TrackAlbum || !EATrax[TrackNumber].TrackArtist))
	{
		ID3Magic = *(unsigned int*)id3v2;

		if (ID3Magic == CHUNKID_ID3)
		{
			CurrentBlock = id3v2 + 0xA;
			while (CurrentBlock < EndPointer)
			{
				ReadFrame = *(unsigned int*)CurrentBlock;

				if ((ReadFrame == 0x32544954) && !EATrax[TrackNumber].TrackName)
				{
					FrameSize = *(unsigned int*)(CurrentBlock + 7);
					if (FrameSize & 0xFF000000)
					{
						EATrax[TrackNumber].TrackName = (char*)malloc((FrameSize - 3));
						ConvertWideToMultiByteTag(CurrentBlock, FrameSize, EATrax[TrackNumber].TrackName);
					}
					else
					{
						EATrax[TrackNumber].TrackName = (char*)malloc(FrameSize);
						FrameSize--;
						strncpy(EATrax[TrackNumber].TrackName, CurrentBlock + 0xB, FrameSize);
					}
					EATrax[TrackNumber].TrackName[FrameSize] = 0;
					bFoundTitle = 1;
					break;
				}
				CurrentBlock++;
			}
			CurrentBlock = id3v2 + 0xA;

			while (CurrentBlock < EndPointer)
			{
				ReadFrame = *(unsigned int*)CurrentBlock;

				if ((ReadFrame == 0x424C4154) && !EATrax[TrackNumber].TrackAlbum)
				{
					FrameSize = *(unsigned int*)(CurrentBlock + 7);
					if (FrameSize & 0xFF000000)
					{
						EATrax[TrackNumber].TrackAlbum = (char*)malloc((FrameSize - 3));
						ConvertWideToMultiByteTag(CurrentBlock, FrameSize, EATrax[TrackNumber].TrackAlbum);
					}
					else
					{
						EATrax[TrackNumber].TrackAlbum = (char*)malloc(FrameSize);
						FrameSize--;
						strncpy(EATrax[TrackNumber].TrackAlbum, CurrentBlock + 0xB, FrameSize);
					}
					EATrax[TrackNumber].TrackAlbum[FrameSize] = 0;
					bFoundAlbum = 1;
					break;
				}
				CurrentBlock++;
			}

			CurrentBlock = id3v2 + 0xA;
			while (CurrentBlock < EndPointer)
			{
				ReadFrame = *(unsigned int*)CurrentBlock;

				if ((ReadFrame == 0x31455054) && !EATrax[TrackNumber].TrackArtist)
				{
					FrameSize = *(unsigned int*)(CurrentBlock + 7);
					if (FrameSize & 0xFF000000)
					{
						EATrax[TrackNumber].TrackArtist = (char*)malloc((FrameSize - 3));
						ConvertWideToMultiByteTag(CurrentBlock, FrameSize, EATrax[TrackNumber].TrackArtist);
					}
					else
					{
						EATrax[TrackNumber].TrackArtist = (char*)malloc(FrameSize);
						FrameSize--;
						strncpy(EATrax[TrackNumber].TrackArtist, CurrentBlock + 0xB, FrameSize);
					}
					EATrax[TrackNumber].TrackArtist[FrameSize] = 0;
					bFoundArtist = 1;
					break;
				}
				CurrentBlock++;
			}
		}
		if (ID3Magic == 0x02334449)
		{
			XNFS_printf(2, "%s: Sorry, ID3v2 before 2.3 not supported yet... Send me a file with it and I'll add it. :/\n", PRINT_TYPE_WARNING);
		}
		free((void*)id3v2);
	}

	if ((!bFoundTitle || !bFoundAlbum || !bFoundArtist) && bFoundID3)
	{
		if (!bFoundTitle && !EATrax[TrackNumber].TrackName)
			ReadFilenameAsTrackName(FilePath, TrackNumber, EATrax);
		if (!bFoundAlbum && !EATrax[TrackNumber].TrackAlbum)
			ReadLastFolderAsAlbum(FilePath, TrackNumber, EATrax);
		if (!bFoundArtist && !EATrax[TrackNumber].TrackArtist)
			ReadTrackNumberAsArtist(FilePath, TrackNumber, EATrax);
	}

	return bFoundID3;
}

/*bool ReadEATraxTags_ID3v1(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax, DWORD music)
{
	char PathBuffer[1024];
	strcpy(PathBuffer, FilePath);
	FILE *fin = fopen(PathBuffer, "rb");
	if (fin == NULL)
	{
		strcpy(PathBuffer, "..\\");
		strcat(PathBuffer, FilePath);
		fin = fopen(PathBuffer, "rb");
	}
	if (fin == NULL)
	{
		perror(PRINT_TYPE_ERROR);
		XNFS_printf(1, "%s: Couldn't open %s\n", PRINT_TYPE_ERROR, PathBuffer);
		return 0;
	}
	fclose(fin);

	bool ret = 0;
	music = BASS_StreamCreateFile(FALSE, PathBuffer, 0, 0, BASS_STREAM_DECODE);

	TAG_ID3 *id3 = (TAG_ID3*)BASS_ChannelGetTags(music, BASS_TAG_ID3);

	if (id3)
	{
		ret = 1;
		if (!EATrax[TrackNumber].TrackName)
			ID3v1TrackNameReading(id3, FilePath, TrackNumber, EATrax);
		if (!EATrax[TrackNumber].TrackAlbum)
			ID3v1TrackAlbumReading(id3, FilePath, TrackNumber, EATrax);
		if (!EATrax[TrackNumber].TrackArtist)
			ID3v1TrackArtistReading(id3, FilePath, TrackNumber, EATrax);
	}

	BASS_StreamFree(music);
	return ret;
}*/

void ReadEATraxTags_Online(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	if (!EATrax[TrackNumber].TrackName)
	{
		EATrax[TrackNumber].TrackName = (char*)malloc(strlen(FilePath) + 1);
		strcpy(EATrax[TrackNumber].TrackName, FilePath);
	}
	if (!EATrax[TrackNumber].TrackAlbum)
	{
		EATrax[TrackNumber].TrackAlbum = (char*)malloc(14);
		strcpy(EATrax[TrackNumber].TrackAlbum, "Online stream");
	}
	if (!EATrax[TrackNumber].TrackArtist)
		ReadTrackNumberAsArtist(FilePath, TrackNumber, EATrax);
}

void ReadEATraxTags_Unknown(const char* FilePath, int TrackNumber, JukeboxTrack* EATrax)
{
	if (!EATrax[TrackNumber].TrackName)
		ReadFilenameAsTrackName(FilePath, TrackNumber, EATrax);
	if (!EATrax[TrackNumber].TrackAlbum)
		ReadLastFolderAsAlbum(FilePath, TrackNumber, EATrax);
	if (!EATrax[TrackNumber].TrackArtist)
		ReadTrackNumberAsArtist(FilePath, TrackNumber, EATrax);
}
