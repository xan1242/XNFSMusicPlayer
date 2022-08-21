// XNFSMusicPlayerInstaller - the ingame installer!
// by Xanvier / xan1242
// 11/2021 - dropping VGMStream and ASF support...
// TODO: handle application calls DIRECTLY from the code, not from a batch script... this would allow for an on screen percentage of completion (plus cross-platform compat, whatever that means for a Win32 game)

#include "..\includes\IniReader.h"
#include "..\includes\injector\hooking.hpp"
#include <direct.h>
#include <stdlib.h>
#include "XNFSMusicPlayerInstaller.h"
#include "FeDialogScreen_Hook.h"

FILE *fin, *fin2, *batchfileout;
struct stat st = { 0 };

unsigned int StateManager = 0;
unsigned int StateManagerStatus = 0;
unsigned int ButtonPressed = 0;
bool bKillThread = 0;
bool bStateManagerMonitorStarted = 0;

unsigned int ButtonHash = 0;

bool bInstallerCompleted = 0;
bool bInstallerSuccess = 0;
int ShowFileErrorDialog = 0;
bool bScanForAllFiles = 0;
bool bInteractiveMusicFolderExists = 0;
bool bEnableInteractiveNoding = 1;
bool bDialogShownAtLeastOnce = 0;
bool bBreakThreadLoop = 0;
bool bUseOGGenc = 0;
bool bExtractFinished = 0;
float OGGEncQuality = 9.0;
int SystemReturnValue = 0;
bool bNoClean = 0;

const char SXBatchScript[] = "@echo off\n\
for %%f in (InteractiveMusicBank0/*.asf) do (\n\
scripts\\XNFSInstallerTools\\sx.exe -wave \"InteractiveMusicBank0/%%f\" -=\"InteractiveMusicBank0/%%f.wav\"\n\
del InteractiveMusicBank0\\%%f /Q)\n\
for %%f in (InteractiveMusicBank1/*.asf) do (\n\
scripts\\XNFSInstallerTools\\sx.exe -wave \"InteractiveMusicBank1/%%f\" -=\"InteractiveMusicBank1/%%f.wav\"\n\
del InteractiveMusicBank1\\%%f /Q)\n\
for %%f in (InteractiveMusicBank2/*.asf) do (\n\
scripts\\XNFSInstallerTools\\sx.exe -wave \"InteractiveMusicBank2/%%f\" -=\"InteractiveMusicBank2/%%f.wav\"\n\
del InteractiveMusicBank2\\%%f /Q)\n\
for %%f in (InteractiveMusicBank3/*.asf) do (\n\
scripts\\XNFSInstallerTools\\sx.exe -wave \"InteractiveMusicBank3/%%f\" -=\"InteractiveMusicBank3/%%f.wav\"\n\
del InteractiveMusicBank3\\%%f /Q)\n\
exit /b 69";

const char OGGEncBatchScript[] = "@echo off\n\
for %%%%f in (InteractiveMusicBank0/*.wav) do (\n\
scripts\\XNFSInstallerTools\\oggenc2.exe -q %.2f \"InteractiveMusicBank0/%%%%f\"\n\
del InteractiveMusicBank0\\%%%%f /Q)\n\
for %%%%f in (InteractiveMusicBank1/*.wav) do (\n\
scripts\\XNFSInstallerTools\\oggenc2.exe -q %.2f \"InteractiveMusicBank1/%%%%f\"\n\
del InteractiveMusicBank1\\%%%%f /Q)\n\
for %%%%f in (InteractiveMusicBank2/*.wav) do (\n\
scripts\\XNFSInstallerTools\\oggenc2.exe -q %.2f \"InteractiveMusicBank2/%%%%f\"\n\
del InteractiveMusicBank2\\%%%%f /Q)\n\
for %%%%f in (InteractiveMusicBank3/*.wav) do (\n\
scripts\\XNFSInstallerTools\\oggenc2.exe -q %.2f \"InteractiveMusicBank3/%%%%f\"\n\
del InteractiveMusicBank3\\%%%%f /Q)\n\
exit /b 68";

char* FilenameFormat;
char TempBuffer[1024];
char TempBuffer2[64];

char *BatchScriptBuffer;
char OGGEncMessageBuffer[127];

char PrintfOutputBuffer1[256];
char PrintfOutputBuffer2[256];
bool bSecondPrintLine;

int UpdateDialogCharBuffer(const char* format, ...)
{
	char TempBuf[255];
	va_list Args;
	va_start(Args, format);
	if (bConsoleExists)
		vfprintf(stdout, format, Args);
	vsprintf(TempBuf, format, Args);
	va_end(Args);

	if (bSecondPrintLine)
		strcpy(PrintfOutputBuffer1, TempBuf);
	else
		strcpy(PrintfOutputBuffer2, TempBuf);
	bSecondPrintLine = !bSecondPrintLine;

	CurrentDialog_FEPrintf(MESSAGEOBJECTHASH, "%s%s", PrintfOutputBuffer1, PrintfOutputBuffer2);
	return 0;
}

unsigned int ReadIDFromBank(FILE *fp)
{
	long int OldOffset = 0;
	unsigned int ID = 0;
	OldOffset = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(&ID, 4, 1, fp);
	fseek(fp, OldOffset, SEEK_SET);
	return ID;
}

long int SearchForBankIDOffset(FILE *fp, unsigned int IDtoSearch)
{
	long int OldOffset = 0;
	unsigned int CurrentID = 0;
	unsigned int IDOffset = 0;
	OldOffset = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	UpdateDialogCharBuffer("Searching for ID: %x\n", IDtoSearch);
	while (!feof(fp))
	{
		fread(&CurrentID, 4, 1, fp);
		if (CurrentID == IDtoSearch)
		{
			fseek(fp, -4, SEEK_CUR);
			IDOffset = ftell(fp);
			UpdateDialogCharBuffer("Found at %x\n", IDOffset);
			break;
		}
	}
	if (IDOffset == 0)
		UpdateDialogCharBuffer("ERROR: ID %x not found.\n", IDtoSearch);
	fseek(fp, OldOffset, SEEK_SET);
	return IDOffset;
}

int OutputNodeInfoToFile(FILE *fp, long OffsetInMPF, const char* OutTxtFileName, const char* OverrideTxtFileName, const char* BaseFileNameFormat, int BankNum)
{
	FILE *fout = fopen(OutTxtFileName, "w");
	FILE *fout2;

	if (fout == NULL /*|| fout2 == NULL*/)
	{
		perror("ERROR");
		return (-1);
	}

	fprintf(fout, "# Interactive music NodeID definition file: Bank %d\n", BankNum);
	int i = 0;
	unsigned int NodeID = 0;
	//unsigned int Time = 0;
	fseek(fp, OffsetInMPF + 0xC, SEEK_SET);
	while (!feof(fp))
	{
		if (fread(&NodeID, 4, 1, fp))
		{
			if ((i > 30 && i != 302) || BankNum)
			{
				fprintf(fout, "%X = ", NodeID);
				fprintf(fout, BaseFileNameFormat, i);
				fprintf(fout, "\n");
			}
		}
		else
			break;
		//fread(&Time, 4, 1, fp); // leftover from other project
		fseek(fp, 4, SEEK_CUR);
		i++;
	}

	if (!BankNum)
	{
		fout2 = fopen(OverrideTxtFileName, "w");

		fputs("# Title screen EventID\n", fout2);
		fprintf(fout2, "%X = ", TITLESCREENEVENTID);
		fprintf(fout2, BaseFileNameFormat, 0);
		fprintf(fout2, "\n");

		fputs("# Final race\n", fout2);
		fprintf(fout2, "%X = ", FINALRACEEVENTID);
		fprintf(fout2, BaseFileNameFormat, 302);
		fprintf(fout2, "\n");
		
		fputs("# Darius loss\n", fout2);
		fprintf(fout2, "%X = ", DARIUSLOSSEVENTID);
		fprintf(fout2, BaseFileNameFormat, 297);
		fprintf(fout2, "\n");

		fputs("# Kenji loss\n", fout2);
		fprintf(fout2, "%X = ", KENJILOSSEVENTID);
		fprintf(fout2, BaseFileNameFormat, 299);
		fprintf(fout2, "\n");

		fputs("# Wolf loss\n", fout2);
		fprintf(fout2, "%X = ", WOLFLOSSEVENTID);
		fprintf(fout2, BaseFileNameFormat, 300);
		fprintf(fout2, "\n");

		fputs("# Angie loss\n", fout2);
		fprintf(fout2, "%X = ", ANGIELOSSEVENTID);
		fprintf(fout2, BaseFileNameFormat, 301);
		fprintf(fout2, "\n");

		fclose(fout2);
	}
	if (!feof(fp))
	{
		fclose(fout);
		return 0;
	}
	XNFS_installer_printf("Total number of nodes: %d\n", i);

	fclose(fout);
	return 1;
}

int ScanAndWriteASFs(FILE *fp, char* MainFileName, int BankNum)
{
	FILE *fout;
	void *FileBuffer;
	char OutFileName[267];
	unsigned int Magic = 0;
	unsigned int NextMagic = 0;
	unsigned int counter = 0;
	long unsigned int MagicOffset = 0;
	long unsigned int NextMagicOffset = 0;
	long unsigned int size = 0;
	long unsigned int totalsize = 0;
	fseek(fp, 0, SEEK_END);
	totalsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	UpdateDialogCharBuffer("Searching for streams...\n");
	while (!feof(fp))
	{
		fread(&Magic, 4, 1, fp);
		if (Magic == 0x6C484353)
		{
			MagicOffset = ftell(fp) - 4;
			UpdateDialogCharBuffer("Stream %d found at %X\n", counter, MagicOffset);
			while (!feof(fp))
			{
				fread(&NextMagic, 4, 1, fp);
				if (NextMagic == 0x6C484353)
				{
					NextMagicOffset = ftell(fp) - 4;;
					fseek(fp, MagicOffset, SEEK_SET);
					break;
				}
				NextMagicOffset = 0;
			}

			if (NextMagicOffset)
				size = NextMagicOffset - MagicOffset;
			else
			{
				fseek(fp, MagicOffset, SEEK_SET);
				size = totalsize - MagicOffset;
			}
			UpdateDialogCharBuffer("Size: %X\n", size);

			sprintf(OutFileName, MainFileName, counter);
			if (bScanForAllFiles)
			{
				fout = fopen(OutFileName, "wb");
				FileBuffer = malloc(size);
				fread(FileBuffer, size, 1, fp);
				fwrite(FileBuffer, size, 1, fout);
				free(FileBuffer);
				fclose(fout);
			}
			else if ((!counter || counter > 30) || BankNum)
			{
				fout = fopen(OutFileName, "wb");
				FileBuffer = malloc(size);
				fread(FileBuffer, size, 1, fp);
				fwrite(FileBuffer, size, 1, fout);
				free(FileBuffer);
				fclose(fout);
			}
			else
				UpdateDialogCharBuffer("Skipping %d\n", counter);
			if (NextMagicOffset)
				fseek(fp, NextMagicOffset, SEEK_SET);
			else
				break;
			counter++;
		}
	}
	XNFS_installer_printf("Extraction successful!\n");
	return 0;
}

int HandleButtonPressedInteractive()
{
	if (ButtonHash == FEDialogScreen_GetButtonHash(0))
		StateManager = STATEMANAGER_INFO1;
	else
		StateManager = STATEMANAGER_EXIT;
	return 0;
}

int HandleButtonPressedFormatQ()
{
	if (ButtonHash == FEDialogScreen_GetButtonHash(0))
	{
		bUseOGGenc = 1;
		StateManager = STATEMANAGER_INFO2;
		return 0;
	}
	StateManager = STATEMANAGER_INFO2;
	return 0;
}

void __declspec(naked) ReturnValueCave()
{
	_asm	mov ButtonHash, eax
	switch (StateManagerStatus) // the place where it jumps across states after the button has been pressed
	{
	case STATEMANAGER_INTRO:
		StateManager = STATEMANAGER_INTERACTIVEQ;
		break;
	case STATEMANAGER_INTERACTIVEQ:
		HandleButtonPressedInteractive();
		break;
	case STATEMANAGER_INFO1:
		StateManager = STATEMANAGER_FORMATQ;
		break;
	case STATEMANAGER_EXIT:
		bBreakThreadLoop = 1;
		break;
	case STATEMANAGER_FORMATQ:
		HandleButtonPressedFormatQ();
		break;
	case STATEMANAGER_INFO2:
		StateManager = STATEMANAGER_EXTRACT;
		break;
	case STATEMANAGER_ASFCLEANUP:
		if (bUseOGGenc)
			StateManager = STATEMANAGER_OGGENCODE;
		else
			StateManager = STATEMANAGER_NODEINFO;
		break;
	case STATEMANAGER_WAVCLEANUP:
		StateManager = STATEMANAGER_NODEINFO;
		break;
	case STATEMANAGER_INFO4:
		StateManager = STATEMANAGER_NODEINFO;
		break;
	}
	_asm	pop esi
	_asm	retn 0x10
}


DWORD WINAPI InstallerStateManager(LPVOID)
{
	injector::MakeJMP(GETBUTTONRETURNEDHASH_ADDRESS, ReturnValueCave, true);
	while (1)
	{
		Sleep(10);
		if (bBreakThreadLoop)
			break;
		if (*(int*)THEGAMEFLOWMANAGER_ADDRESS == 3 && !bDialogShownAtLeastOnce)
		{
			FEDialogScreen_ShowDialog(INTROMESSAGE, OKSTR, 0, 0);
			StateManagerStatus = STATEMANAGER_INTRO;
			bDialogShownAtLeastOnce = 1;
		}
		if (SystemReturnValue == 69) // after SX
		{
			if (bUseOGGenc)
				StateManager = STATEMANAGER_OGGENCODE;
			else
				StateManager = STATEMANAGER_NODEINFO;
			SystemReturnValue = 0;
		}
		if (SystemReturnValue == 68) // after OGGEnc2
		{
			StateManager = STATEMANAGER_NODEINFO;
			SystemReturnValue = 0;
		}
		if (ShowFileErrorDialog)
		{
			char msg[255];
			sprintf(msg, FILEERRORFMTSTR, TempBuffer);
			FEDialogScreen_ShowDialog(msg, EXITSTR, 0, 0);
			StateManagerStatus = STATEMANAGER_EXIT;
			ShowFileErrorDialog = 0;
		}

		if ((StateManagerStatus == STATEMANAGER_EXTRACT) && bExtractFinished)
		{
			StateManager = STATEMANAGER_DECODE;
		}

		if (bDialogShownAtLeastOnce && *(int*)THEGAMEFLOWMANAGER_ADDRESS == 3)
		{
			switch (StateManager)
			{
			case STATEMANAGER_INTERACTIVEQ:
				FEDialogScreen_ShowDialog(USINGINTERACTIVEQUESTION, INTERACTIVEYES, INTERACTIVENO, 0);
				StateManagerStatus = STATEMANAGER_INTERACTIVEQ;
				break;
			case STATEMANAGER_INFO1:
				FEDialogScreen_ShowDialog(STARTMESSAGE, CONTSTR, 0, 0);
				StateManagerStatus = STATEMANAGER_INFO1;
				break;
			case STATEMANAGER_EXIT:
				FEDialogScreen_ShowDialog(NOTUSINGINTERACTIVEMESSAGE, FINISHSTR, 0, 0);
				bInstallerSuccess = 1;
				bEnableInteractiveNoding = 0;
				StateManagerStatus = STATEMANAGER_EXIT;
				break;
			case STATEMANAGER_FORMATQ:
				FEDialogScreen_ShowDialog(FORMATMESSAGE, OGGBUTTON, WAVBUTTON, NULL);
				StateManagerStatus = STATEMANAGER_FORMATQ;
				break;
			case STATEMANAGER_INFO2:
				FEDialogScreen_ShowDialog(EXTRACTIONSTARTMSG, EXTRACTIONBUTTON, 0, 0);
				StateManagerStatus = STATEMANAGER_INFO2;
				break;
			case STATEMANAGER_EXTRACT:
				StateManagerStatus = STATEMANAGER_EXTRACT;
				if (stat("InteractiveMusicBank0", &st) == -1 || stat("InteractiveMusicBank1", &st) == -1 || stat("InteractiveMusicBank2", &st) == -1 || stat("InteractiveMusicBank3", &st) == -1)
				{
					mkdir("InteractiveMusicBank0");
					mkdir("InteractiveMusicBank1");
					mkdir("InteractiveMusicBank2");
					mkdir("InteractiveMusicBank3");
				}
				else
				{
					bInteractiveMusicFolderExists = 1;
					Sleep(10);
					FEDialogScreen_ShowDialog(FOLDERALREADYEXISTSMSG, CONTSTR, 0, 0);
					StateManagerStatus = STATEMANAGER_INFO4;
					break;
				}
				FEDialogScreen_ShowDialog(ASFSCANWAITMSG, BUTTONCOUNTERSTR, 0, 0);
				for (unsigned int i = 0; i <= 3; i++)
				{
					sprintf(TempBuffer, "sound\\PFData\\CB_Mus_%d.mus", i);
					fin = fopen(TempBuffer, "rb");
					if (fin == NULL)
					{
						ShowFileErrorDialog = 1;
						break;
					}
					sprintf(TempBuffer, "InteractiveMusicBank%d\\CB_Mus_%d.mus", i, i);
					strcat(TempBuffer, "_%d.asf");
					ScanAndWriteASFs(fin, TempBuffer, i);
					CurrentDialog_FEPrintf(BUTTON1OBJECTHASH, "%d/4", i + 1);
					fclose(fin);
				}
				bExtractFinished = 1;
				break;
			case STATEMANAGER_DECODE:
				if (bInteractiveMusicFolderExists)
				{
					FEDialogScreen_ShowDialog(FOLDERALREADYEXISTSMSG2, GENSTR, 0, 0);
					StateManagerStatus = STATEMANAGER_INFO4;
				}
				else
				{
					FEDialogScreen_ShowDialog(EASXTOOLMESSAGE, 0, 0, 0);
					batchfileout = fopen("XNFSTempBatch.bat", "w");
					fwrite(SXBatchScript, strlen(SXBatchScript), 1, batchfileout);
					fclose(batchfileout);
					SystemReturnValue = system("XNFSTempBatch.bat");
					remove("XNFSTempBatch.bat");
					StateManagerStatus = STATEMANAGER_DECODE;
				}
				break;
			case STATEMANAGER_OGGENCODE:
				if (bUseOGGenc)
				{
					sprintf(OGGEncMessageBuffer, OGGENCODINGFORMATTER, OGGEncQuality);
					FEDialogScreen_ShowDialog(OGGEncMessageBuffer, 0, 0, 0);
					BatchScriptBuffer = (char*)malloc(strlen(OGGEncBatchScript));
					sprintf(BatchScriptBuffer, OGGEncBatchScript, OGGEncQuality, OGGEncQuality, OGGEncQuality, OGGEncQuality);
					batchfileout = fopen("XNFSTempBatch_OGG.bat", "w");
					fwrite(BatchScriptBuffer, strlen(BatchScriptBuffer), 1, batchfileout);
					fclose(batchfileout);
					SystemReturnValue = system("XNFSTempBatch_OGG.bat");
					remove("XNFSTempBatch2.bat");
				}
				break;
			case STATEMANAGER_NODEINFO:
				FEDialogScreen_ShowDialog(NODEGENMSGFMT, NULL, NULL, NULL);
				for (unsigned int i = 0; i <= 3; i++)
				{
					/*sprintf(TempBuffer, "Extracting node information from cb_Mus_%d.mpf...", i);
					FEDialogScreen_ShowDialog(TempBuffer, 0, 0, 0);*/
					CurrentDialog_FEPrintf(MESSAGEOBJECTHASH, NODEGENMSGFMT, i);
					sprintf(TempBuffer, "sound\\PFData\\CB_Mus_%d.mus", i);
					fin = fopen(TempBuffer, "rb");
					if (fin == NULL)
					{
						ShowFileErrorDialog = 1;
						break;
					}
					sprintf(TempBuffer, "sound\\PFData\\CB_Mus_%d.mpf", i);
					fin2 = fopen(TempBuffer, "rb");
					if (fin2 == NULL)
					{
						ShowFileErrorDialog = 2;
						break;
					}
					sprintf(TempBuffer, "InteractiveMusicBank%d\\CB_Mus_%d.mus", i, i);
					if (bUseOGGenc)
						strcat(TempBuffer, "_%d.asf.ogg");
					else
						strcat(TempBuffer, "_%d.asf.wav");
					sprintf(TempBuffer2, "scripts\\XNFSMusicPlayer\\NodePaths%d.txt", i);
					OutputNodeInfoToFile(fin2, SearchForBankIDOffset(fin2, ReadIDFromBank(fin)), TempBuffer2, "scripts\\XNFSMusicPlayer\\Overrides.txt", TempBuffer, i);
					fclose(fin);
					fclose(fin2);
				}
				FEDialogScreen_ShowDialog(INSTALLERSUCCESSMSG, FINISHSTR, 0, 0);
				bInstallerSuccess = 1;
				StateManagerStatus = STATEMANAGER_EXIT;
				break;
			}
		}
		StateManager = 0;
	}
	if (bInstallerSuccess)
	{
		CIniReader inireader("XNFSMusicPlayer.ini");
		if (!bEnableInteractiveNoding)
			inireader.WriteInteger("Playback", "EnableInteractiveNoding", 0);
		else
			inireader.WriteInteger("Playback", "EnableInteractiveNoding", 1);
		inireader.WriteInteger("Installer", "InstallerCompleted", 1);
	}
	*(int*)EXITTHEGAMEFLAG_ADDRESS = 1;
	return 0;
}

int init()
{
	CIniReader inireader("XNFSMusicPlayer.ini");
	bInstallerCompleted = inireader.ReadInteger("Installer", "InstallerCompleted", 0);
	bScanForAllFiles = inireader.ReadInteger("Installer", "ScanForAllFiles", 0);
	bNoClean = inireader.ReadInteger("Installer", "NoClean", 0);
	OGGEncQuality = inireader.ReadFloat("Installer", "OGGEncQuality", 9.0);
	return 0;
}

int StartTheStateManager()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&InstallerStateManager, NULL, 0, NULL);
	return 0;
}

bool separate_console(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return FALSE;

	// if cursor position is (0,0) then we were launched in a separate console
	return ((!csbi.dwCursorPosition.X) && (!csbi.dwCursorPosition.Y));
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		init();
		if (!bInstallerCompleted)
		{
			injector::WriteMemory<unsigned int>(0x009D3B3C, (unsigned int)&StartTheStateManager, true);
			if (separate_console())
			{
				bConsoleExists = true;
				freopen("CON", "w", stdout);
				freopen("CON", "w", stderr);
			}
			else
			{
				AttachConsole(ATTACH_PARENT_PROCESS);
				AllocConsole();
			}
		}

	}
	return TRUE;
}
