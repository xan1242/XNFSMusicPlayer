#pragma once
#include <iostream>
#include "XNFSMusicPlayer.h"
#include "bass.h"

#define DEFAULT_INTERACTIVE_SAMPLERATE 36000

void* BASSPush_Buffer = NULL;
DWORD BASSPush_DecodeFlags = BASS_STREAM_DECODE;

//unsigned int BASSPush_Position = 0;

unsigned int XNFS_BASS_DecodeData(char* FilePath, DWORD& hHandle)
{
	DWORD decoder = BASS_StreamCreateFile(FALSE, FilePath, 0, 0, BASSPush_DecodeFlags);

	if (!decoder)
	{
		XNFS_printf(1, "%s: Error creating decoder: BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
		return 0;
	}

	unsigned int DecodeSize = BASS_ChannelGetLength(decoder, BASS_POS_BYTE);
	BASSPush_Buffer = malloc(DecodeSize);

	if (!BASS_ChannelGetData(decoder, BASSPush_Buffer, DecodeSize))
		XNFS_printf(1, "%s: Error decoding data: BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());

	float DecodedSampleRate = 0.0;
	if (!BASS_ChannelGetAttribute(decoder, BASS_ATTRIB_FREQ, &DecodedSampleRate)) // doing this here causes a bug where it changes the sample rate before the previous stream finishes...
		XNFS_printf(1, "%s: Error getting sample rate during push decode: BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	else
	{
		if (!BASS_ChannelSetAttribute(hHandle, BASS_ATTRIB_FREQ, DecodedSampleRate))
			XNFS_printf(1, "%s: Error updating sample rate during push decode: BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	}

	if (!BASS_StreamFree(decoder))
		XNFS_printf(1, "%s: Error freeing decoder: BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());

	return DecodeSize;
}

int XNFS_BASS_CreateStreamPushHandle(DWORD& hHandle)
{
	hHandle = BASS_StreamCreate(DEFAULT_INTERACTIVE_SAMPLERATE, 2, 0, STREAMPROC_PUSH, NULL);
	if (!hHandle)
		XNFS_printf(1, "%s: Error creating push stream: BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());
	return 0;
}

int XNFS_BASS_PushFile(char* FilePath, DWORD& hHandle)
{
	unsigned int DecodeSize = 0;

	if (BASSPush_Buffer)
		free(BASSPush_Buffer);

	DecodeSize = XNFS_BASS_DecodeData(FilePath, hHandle);

	if (!BASS_StreamPutData(hHandle, BASSPush_Buffer, DecodeSize))
		XNFS_printf(1, "%s: Error putting file data to stream: BASS Error %d\n", PRINT_TYPE_ERROR, BASS_ErrorGetCode());

	return 0;
}