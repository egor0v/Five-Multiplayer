#pragma once

int GetPID(char *pProcessName);
bool foundProcess(DWORD processID);
bool PrintMemoryInfo(DWORD processID);
bool tryInjectDLLIntoGame(char *processName);
void initTimer(void);
void tryRunTasks(void);
int createMenuItem(char *charName, int times, int guitype, char *titleName);
void ClearScreen(void);
float returnProcessMemory(DWORD processID);
void pause();
char* returnProcessPathInfo(DWORD processID);
void startProgram(char *lpApplicationName);
bool Inject(DWORD pId, char *dllName);
int PrintModules(DWORD processID);
HMODULE retModuleHandle(DWORD processID, const char* moduleName);