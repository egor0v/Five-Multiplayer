#include "stdafx.h"
#include "modbaserequired.h"
#include "StdInc.h"
#include <thread>
#include "GameInit.h"

CChatWindow				*pChatWindow = 0;
CNetworkManager			*pNetowkManager;
CMDWindow				*pCmdWindow;

void initModBaseRequired() {
	pChatWindow = new CChatWindow(); //Init the chat, set the pointer to it, w/e
	pCmdWindow = new CMDWindow();
	modbaseRequiredStarted = true;
}
void initConnection() {
	pNetowkManager = new CNetworkManager("127.0.0.1", atoi("7777"), "colistro123", ""); //Start the game
}
void connectToServer() {
	std::thread t1(&initConnection);
	t1.join();
}
bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask)
{
	for (; *mask; ++mask, ++data, ++pattern)
	{
		if (*mask == 'x' && *data != *pattern)
		{
			return false;
		}
	}
	return (*mask) == NULL;
}
UINT64 FindPattern(char *pattern, char *mask)
{	//Edited, From YSF by Kurta999
	UINT64 i;
	UINT64 size;
	UINT64 address;

	MODULEINFO info = { 0 };

	address = (UINT64)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	size = (UINT64)info.SizeOfImage;

	for (i = 0; i < size; ++i)
	{
		if (memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask))
		{
			return (UINT64)(address + i);
		}
	}
	return 0;
}