#pragma once
#include "DialogInterfaceHook.h"
#define CFENGINSTANCE_ADDRESS 0x91CADC
#define CFENGQUEUEMESSAGE_ADDRESS 0x00516BE0
#define EXITTHEGAMEFLAG_ADDRESS 0x9257EC
#define BOOTFLOW1_ADDRESS 0x008F3C58
#define BOOTFLOW2_ADDRESS 0x008F3C78
#define BOOTFLOW3_ADDRESS 0x008F3C98
#define BOOTFLOW4_ADDRESS 0x008F3CB4
#define QUEUEGAMEMESSAGEHOOK_ADDRESS 0x0057F118
#define SOMEBROKENFNG_ADDRESS 0x00894920
#define THEGAMEFLOWMANAGER_ADDRESS 0x00925E90

#define CONTINUELANGHASH 0x8098A54C
#define OKLANGHASH 0x417B2601
#define EXITLANGHASH 0xA9950B93
#define YESLANGHASH 0x70E01038
#define NOLANGHASH 0x417B25E4

#define YES1 0x59455331
#define YES2 0x59455332
#define YES3 0x59455333
#define YES4 0x59455334
#define YES5 0x59455335
#define YES6 0x59455336
#define YES7 0x59455337
#define YES8 0x59455338
#define YES9 0x59455339

#define NO01 0x4E4F3031
#define NO02 0x4E4F3032
#define NO03 0x4E4F3033
#define NO04 0x4E4F3034
#define NO05 0x4E4F3035
#define NO06 0x4E4F3036
#define NO07 0x4E4F3037
#define NO08 0x4E4F3038
#define NO09 0x4E4F3039

#define OK01 0x4F4B3031 
#define OK02 0x4F4B3032
#define OK03 0x4F4B3033
#define OK04 0x4F4B3034
#define OK05 0x4F4B3035
#define OK06 0x4F4B3036
#define OK07 0x4F4B3037
#define OK08 0x4F4B3038
#define OK09 0x4F4B3039

#define USEASF 0x55415346
#define USEWAV 0x55574156
#define USEOGG 0x554F4747

#define TITLESCREENEVENTID 0xC53FC7

#define INTROMESSAGE "Welcome to the XNFSMusicPlayer installer!\
^This will guide you through the basic first time interactive music setup procedure.\
^Before going any further, make sure you have enough disk space!^It is also recommended to run the game in a window as this might take a while."

#define USINGINTERACTIVEQUESTION "Do you want to use interactive music?"
#define INTERACTIVEYES "Use interactive music"
#define INTERACTIVENO "Don't use interactive music"

#define NOTUSINGINTERACTIVEMESSAGE "OK! Not using interactive music!^As this installer is very limited, make sure to further tune your settings in XNFSMusicPlayer.ini which is found in the scripts folder!^Also be sure to add some tracks in the Playlist.m3u file located at scripts\\XNFSMusicPlayer.^The game will now quit. You may start the game normally from now on."

#define STARTMESSAGE "This installer will now extract the necessary data from the game in order to bring compatibility for the BASS library.^By default it will be in the InteractiveMusic directory.^You can change this later in NodePaths.txt which is found in the scripts\\XNFSMusicPlayer folder."

#define FORMATMESSAGE "Which format do you want to use?^OGG - installation takes up to 10min, 300MB required (only during install 1GB additionally required)^WAV - installation takes up to 5min, 1GB required"
#define ASFBUTTON "ASF - fastest install, original format"
#define WAVBUTTON "WAV - no loss and very stable, big files"
#define OGGBUTTON "OGG - lossy, but small and stable (recommended)"

#define EXTRACTIONSTARTMSG "This installer will now proceed to extract streams from Sound\\PFData\\MW_Music.mus."
#define EXTRACTIONBUTTON "Extract!"

#define FOLDERALREADYEXISTSMSG "Folder InteractiveMusic already exists. Assuming all files are extracted already.^If you need to extract again, delete/rename the folder and try again."

#define FOLDERALREADYEXISTSMSG2 "Skipping the decoding since InteractiveMusic already exists.^The installer will now generate a node path definition file."

#define ASFEXTRACTSUCCESS1 "Extraction of MW_Music.mus sucessful!^Will start to generate a definition file after confirmation."
#define ASFEXTRACTSUCCESS2 "Extraction of MW_Music.mus sucessful!^Will start to decode files after confirmation."

#define EASXTOOLMESSAGE "Please wait...^Decoding ASFs to WAV with Sound eXchange..."

#define ASFCLEANUPQUESTION "Decoding complete!^Do you want to clean up the ASF files?"
#define WAVCLEANUPQUESTION "OGG Encoding complete!^Do you want to clean up the WAV files?"

#define ASFCLEANUPCOMPLETE1 "ASF cleanup complete!^The installer will now start the encoding process."
#define ASFCLEANUPCOMPLETE2 "ASF cleanup complete!^The installer will now generate a node path definition file."

#define WAVCLEANUPCOMPLETE "WAV cleanup complete!^The installer will now generate a node path definition file."

#define ASFCLEANUPMESSAGE "Cleaning up ASFs..."
#define WAVCLEANUPMESSAGE "Cleaning up WAVs..."

#define ASFNOTCLEANINGUPMSG1 "OK! Not cleaning up ASFs!^If you have opted to encode to OGG, the process will begin now."
#define ASFNOTCLEANINGUPMSG2 "OK! Not cleaning up ASFs!^The installer will now generate a node path definition file."

#define WAVNOTCLEANINGUPMSG "OK! Not cleaning up WAVs!^The installer will now generate a node path definition file."

#define OGGENCODINGFORMATTER "Please wait...^Encoding WAVs to OGG with %.2f quality..."

#define INSTALLERSUCCESSMSG "Installation successful! Enjoy XNFSMusicPlayer!^As this installer is very limited, make sure to further tune your settings in XNFSMusicPlayer.ini which is found in the scripts folder!^Also be sure to add some tracks in the Playlist.m3u file located at scripts\\XNFSMusicPlayer.^The game will now quit. You may start the game normally from now on."

#define ERRORMSG "Error opening Sound\\PFData\\MW_Music.mus^Check if the file exists and/or if it is accessible by permissions.^The installer will now quit."


char PrintfOutputBuffer1[256];
char PrintfOutputBuffer2[256];
bool bSecondPrintLine;

// check feDialogConfig structure in DialogInterfaceHook.h if these look cryptic
feDialogConfig IntroMessageConfig = { INTROMESSAGE, DLGTITLE_INFO, OKLANGHASH, OK01, 0, 0, 0, 0, 0, 0, NULL, 0, 1};
feDialogConfig InteractiveQuestion = { USINGINTERACTIVEQUESTION, DLGTITLE_SEL, 0, YES1, 0, NO01, 0, 0, 0, 0, NULL, DLG_STACKED, 2 };
feDialogConfig NotInteractiveConfig = { NOTUSINGINTERACTIVEMESSAGE, DLGTITLE_INFO, EXITLANGHASH, OK02, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig StartMessageConfig = { STARTMESSAGE, DLGTITLE_INFO, CONTINUELANGHASH, OK03, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
//feDialogConfig FormatQuestion = { FORMATMESSAGE, DLGTITLE_SEL, 0, USEASF, 0, USEWAV, 0, USEOGG, 0, 0, NULL, DLG_STACKED, 3 };
feDialogConfig FormatQuestion = { FORMATMESSAGE, DLGTITLE_SEL, 0, USEOGG, 0, USEWAV, 0, NULL, 0, 0, NULL, DLG_STACKED, 2 };
feDialogConfig ExtractionMessage = { EXTRACTIONSTARTMSG, DLGTITLE_ATTN, 0, OK04, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig AlreadyExistsConfig = { FOLDERALREADYEXISTSMSG, DLGTITLE_ATTN, CONTINUELANGHASH, OK07, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig ExtractingMessage = { "Extracting...^Please wait.", DLGTITLE_ATTN, 0, 0, 0, 0, 0, 0, 0, 0, 0, DLG_ANIMATING, 0 };
feDialogConfig DecodingAlreadyExists = { FOLDERALREADYEXISTSMSG2, DLGTITLE_ATTN, OKLANGHASH, OK07, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig ExtractSuccess1 = { ASFEXTRACTSUCCESS1, DLGTITLE_ATTN, CONTINUELANGHASH, OK07, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig ExtractSuccess2 = { ASFEXTRACTSUCCESS2, DLGTITLE_ATTN, CONTINUELANGHASH, OK05, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig SXToolDecodingMsg = { EASXTOOLMESSAGE, DLGTITLE_ATTN, 0, 0, 0, 0, 0, 0, 0, 0, 0, DLG_ANIMATING, 0 };
feDialogConfig ASFNotCleaning1 = { ASFNOTCLEANINGUPMSG1, DLGTITLE_ATTN, CONTINUELANGHASH, OK06, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig ASFNotCleaning2 = { ASFNOTCLEANINGUPMSG2, DLGTITLE_ATTN, CONTINUELANGHASH, OK07, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig WAVNotCleaning = { WAVNOTCLEANINGUPMSG, DLGTITLE_ATTN, CONTINUELANGHASH, OK07, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig ASFCleanup = { ASFCLEANUPQUESTION, DLGTITLE_SEL, YESLANGHASH, YES3, NOLANGHASH, NO03, 0, 0, 0, 0, NULL, 0, 2 };
feDialogConfig WAVCleanup = { WAVCLEANUPQUESTION, DLGTITLE_SEL, YESLANGHASH, YES4, NOLANGHASH, NO04, 0, 0, 0, 0, NULL, 0, 2 };
feDialogConfig ASFCleanupDone1 = { ASFCLEANUPCOMPLETE1, DLGTITLE_ATTN, CONTINUELANGHASH, OK06, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig ASFCleanupDone2 = { ASFCLEANUPCOMPLETE2, DLGTITLE_ATTN, CONTINUELANGHASH, OK07, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig WAVCleanupDone = { WAVCLEANUPCOMPLETE, DLGTITLE_ATTN, CONTINUELANGHASH, OK07, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig InstallerSuccessMsg = { INSTALLERSUCCESSMSG, DLGTITLE_INFO, EXITLANGHASH, OK02, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };
feDialogConfig FileOpenErrorMsg = { ERRORMSG, DLGTITLE_INFO, OKLANGHASH, OK02, 0, 0, 0, 0, 0, 0, NULL, 0, 1 };

bool bConsoleExists = false;
int __stdcall XNFS_installer_printf(const char* Format, ...)
{
	va_list ArgList;
	int Result = 0;
	if (bConsoleExists)
	{
		__crt_va_start(ArgList, Format);
		Result = vfprintf(stdout, Format, ArgList);
		__crt_va_end(ArgList);
	}
	return Result;
}