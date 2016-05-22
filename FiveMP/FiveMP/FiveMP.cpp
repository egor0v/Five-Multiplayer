#include "stdafx.h"

using namespace std;

//Entry
int main(void) {
	bool GameThread = false;
	char GamePath[MAX_PATH] = { 0 };
	char GameFullPath[MAX_PATH] = { 0 };
	char Params[] = "";

	if (!SharedUtils::Registry::Read(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\rockstar games\\Grand Theft Auto V", "InstallFolder", GamePath, MAX_PATH))
	{
		// If we cannot find it - display an error
		// and close launcher
		// TODO: Custom game path selector
		MessageBox(NULL, "Cannot find game path in registry!", "Fatal Error", MB_ICONERROR);
		return 0;
	}

	// Format game paths
	sprintf(GamePath, "%s", GamePath);
	sprintf(GameFullPath, "%s\\PlayGTAV.exe", GamePath);

	// Predefine startup and process infos
	STARTUPINFO siStartupInfo;
	PROCESS_INFORMATION piProcessInfo;
	memset(&siStartupInfo, 0, sizeof(siStartupInfo));
	memset(&piProcessInfo, 0, sizeof(piProcessInfo));
	siStartupInfo.cb = sizeof(siStartupInfo);

	// Create game process
	if (!CreateProcess(GameFullPath, Params, NULL, NULL, true, CREATE_SUSPENDED, NULL, GamePath, &siStartupInfo, &piProcessInfo))
	{
		MessageBox(NULL, "Grand Theft Auto V was not able to start.", "Fatal Error", MB_ICONERROR);
		return 0;
	}

	// Resume game main thread
	ResumeThread(piProcessInfo.hThread);

	while (GameThread != true) {
		HWND hWnds = FindWindowA(NULL, "Grand Theft Auto V");

		if (hWnds != NULL) {
			while (!tryInjectDLLIntoGame("GTA5.exe")) {
				printf("Couldn't find Process Name GTA5.exe, please try re-opening it\n");
				Sleep(200);
				ClearScreen();
			}
			GameThread = true;
		}
	}
	printf("DLL injected, closing...\n");
	//FindNativeTableAddress();
	//getch();
}