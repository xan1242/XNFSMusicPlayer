#pragma once

#define STATEMANAGER_INTRO 0
#define STATEMANAGER_INFO1 1
#define STATEMANAGER_EXIT 2
#define STATEMANAGER_FORMATQ 3
#define STATEMANAGER_INFO2 4
#define STATEMANAGER_EXTRACT 5
#define STATEMANAGER_DECODE 6
#define STATEMANAGER_ASFCLEANUP 7
//#define STATEMANAGER_INFO3 8
#define STATEMANAGER_OGGENCODE 9
#define STATEMANAGER_WAVCLEANUP 10
#define STATEMANAGER_INFO4 11
#define STATEMANAGER_NODEINFO 12
#define STATEMANAGER_INTERACTIVEQ 13

#define THEGAMEFLOWMANAGER_ADDRESS 0x00A99BBC
#define EXITTHEGAMEFLAG_ADDRESS 0x00A99560
#define GETBUTTONRETURNEDHASH_ADDRESS 0x005A1BD3

#define TITLESCREENEVENTID 0xC53FC7
#define FINALRACEEVENTID 0xC7EE1C
#define DARIUSLOSSEVENTID 0x279DEA
#define KENJILOSSEVENTID 0x2CF7A1
#define WOLFLOSSEVENTID 0xF290CC
#define ANGIELOSSEVENTID 0xAF43E2

// strings
#define OKSTR "OK"
#define YESSTR "Yes"
#define NOSTR "No"
#define EXITSTR "Exit"
#define QUITSTR "Quit"
#define CONTSTR "Continue"
#define GENSTR "Generate"
#define ENCSTR "Encode"
#define DECODESTR "Decode"
#define FINISHSTR "Finish"

#define FILEERRORFMTSTR "Error opening %s^Check if the file exists and/or if it is accessible by permissions.^The installer will now quit."

#define INTROMESSAGE "Welcome to the XNFSMusicPlayer installer!\
^This will guide you through the basic first time interactive music setup procedure.\
^Before going any further, make sure you have enough disk space!^It is also recommended to run the game in a window as this might take a while."

#define USINGINTERACTIVEQUESTION "Do you want to use interactive music?"
#define INTERACTIVEYES "Use interactive music"
#define INTERACTIVENO "Don't use interactive music"

#define NOTUSINGINTERACTIVEMESSAGE "OK! Not using interactive music!^As this installer is very limited, make sure to further tune your settings in XNFSMusicPlayer.ini which is found in the scripts folder!^Also be sure to add some tracks in the Playlist.m3u file located at scripts\\XNFSMusicPlayer.^The game will now quit. You may start the game normally from now on."

#define STARTMESSAGE "This installer will now extract the necessary data from the game in order to bring compatibility for the BASS library.^By default it will be in the InteractiveMusicBank directories.^You can change this later in NodePaths(num).txt which are found in the scripts\\XNFSMusicPlayer folder."

#define FORMATMESSAGE "Which format do you want to use?^OGG - installation takes up to 10min, 300MB required (only during install 1GB additionally required)^WAV - installation takes up to 5min, 1GB required"
//#define ASFBUTTON "ASF - fastest install, original format"
#define WAVBUTTON "WAV - no loss and very stable, big files"
#define OGGBUTTON "OGG - lossy, but small and stable (recommended)"

#define EXTRACTIONSTARTMSG "This installer will now proceed to extract streams from sound\\PFData\\cb_Mus_X.mus banks."
#define EXTRACTIONBUTTON "Extract!"

#define FOLDERALREADYEXISTSMSG "Folders InteractiveMusicBankX already exist. Assuming all files are extracted already.^If you need to extract again, delete/rename the folders and try again."

#define FOLDERALREADYEXISTSMSG2 "Skipping the decoding since InteractiveMusicBankX already exists.^The installer will now generate the node path definition files."

#define ASFEXTRACTSUCCESS1 "Extraction of cb_Mus banks sucessful!^Will start to generate a definition file after confirmation."
#define ASFEXTRACTSUCCESS2 "Extraction of cb_Mus banks sucessful!^Will start to decode files after confirmation."

#define EASXTOOLMESSAGE "Please wait...^Decoding ASFs to WAV with Sound eXchange..."

#define ASFCLEANUPQUESTION "Decoding complete!^Do you want to clean up the ASF files?"
#define WAVCLEANUPQUESTION "OGG Encoding complete!^Do you want to clean up the WAV files?"

#define ASFCLEANUPCOMPLETE1 "ASF cleanup complete!^The installer will now start the encoding process."
#define ASFCLEANUPCOMPLETE2 "ASF cleanup complete!^The installer will now generate the node path definition files."

#define WAVCLEANUPCOMPLETE "WAV cleanup complete!^The installer will now generate the node path definition files."

#define ASFCLEANUPMESSAGE "Cleaning up ASFs..."
#define WAVCLEANUPMESSAGE "Cleaning up WAVs..."

#define ASFNOTCLEANINGUPMSG1 "OK! Not cleaning up ASFs!^OGG encoding process will begin now."
#define ASFNOTCLEANINGUPMSG2 "OK! Not cleaning up ASFs!^The installer will now generate the node path definition files."

#define WAVNOTCLEANINGUPMSG "OK! Not cleaning up WAVs!^The installer will now generate the node path definition files."

#define OGGENCODINGFORMATTER "Please wait...^Encoding WAVs to OGG with %.2f quality..."

#define INSTALLERSUCCESSMSG "Installation successful! Enjoy XNFSMusicPlayer!^As this installer is very limited, make sure to further tune your settings in XNFSMusicPlayer.ini which is found in the scripts folder!^Also be sure to add some tracks in the Playlist.m3u file located at scripts\\XNFSMusicPlayer.^The game will now quit. You may start the game normally from now on."

#define ERRORMSG "Error opening Sound\\PFData\\MW_Music.mus^Check if the file exists and/or if it is accessible by permissions.^The installer will now quit."

#define ASFSCANWAITMSG "Please wait until the ASF scanner has finished extracting...^Dialog will automatically update as it's happening."

#define NODEGENMSGFMT "Extracting node information from cb_Mus_%d.mpf..."

#define BUTTONCOUNTERSTR "0/4"

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
