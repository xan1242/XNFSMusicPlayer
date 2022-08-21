// XNFSMusicPlayer
// ULTRA MESSY CODE
// TODO: Common code abstraction
// since 1.8 WIP: Carbon port, code splitting // 03/2018
// 11/2021 - dropped VGMStream support

#ifdef GAME_MW
#include "XNFSMusicPlayer\XNFSMusicPlayer_MW.h"
#endif

#ifdef GAME_CARBON
#include "XNFSMusicPlayer\XNFSMusicPlayer_Carbon.h"
#endif

#pragma warning(disable:4800 4244 4305)

bool separate_console(void)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
	{
		return FALSE;
	}

	// if cursor position is (0,0) then we were launched in a separate console
	return ((!csbi.dwCursorPosition.X) && (!csbi.dwCursorPosition.Y));
}

int Init()
{
	// for some reason this part never gets properly shown
	if (HIWORD(BASS_GetVersion()) != BASSVERSION)
	{
		MessageBox(0, "An incorrect version of BASS.DLL was loaded.", 0, MB_ICONERROR);
		return FALSE;
	}

#ifdef XNFS_TESTBUILD
	XNFS_printf(1, "%s: XNFSMusicPlayer %d.%d WIP init\n", PRINT_TYPE_INFO, BUILD_VER_MAJOR, BUILD_VER_MINOR);
#else
	XNFS_printf(1, "%s: XNFSMusicPlayer %d.%d init\n", PRINT_TYPE_INFO, BUILD_VER_MAJOR, BUILD_VER_MINOR);
#endif
	if (!InitConfig())
	{
		XNFS_printf(1, "%s: XNFSMusicPlayer init failed: Installer not completed. To ignore, set InstallerCompleted to 1 in settings ini.\n", PRINT_TYPE_ERROR);
		return 0;
	}

	InjectCode();

	
#ifdef XNFS_TESTBUILD
	XNFS_printf(1, "%s: XNFSMusicPlayer %d.%d WIP init complete\n", PRINT_TYPE_INFO, BUILD_VER_MAJOR, BUILD_VER_MINOR);
#else
	XNFS_printf(1, "%s: XNFSMusicPlayer %d.%d init complete\n", PRINT_TYPE_INFO, BUILD_VER_MAJOR, BUILD_VER_MINOR);
#endif
	return 1;
}

void FixWorkingDirectory()
{
	char ExecutablePath[_MAX_PATH];

	GetModuleFileName(GetModuleHandle(""), ExecutablePath, _MAX_PATH);
	ExecutablePath[strrchr(ExecutablePath, '\\') - ExecutablePath] = 0;
	SetCurrentDirectory(ExecutablePath);
}

BOOL APIENTRY DllMain(HMODULE /*hModule*/, DWORD reason, LPVOID /*lpReserved*/)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		bConsoleExists = separate_console();

		if (bConsoleExists)
		{
			freopen("CON", "w", stdout);
			freopen("CON", "w", stderr);
			freopen("CON", "r", stdin);
		}

		// fix the working directory to be in the executable folder!
		FixWorkingDirectory();

		if (!Init())
			return TRUE; // bugfix for some stupid OS errors (from FALSE to TRUE)
	}
	return TRUE;
}
