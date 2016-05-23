#include <stdafx.h>
#include "modbaserequired.h"
#include "natives.h"
#include "Hooking.h"
#include "EventCore.h"
#include <iostream>
#include <thread>
#include <GameInit.h>

MODULEINFO g_MainModuleInfo = { 0 };
void ApplyPatches();
void DisableLegalMessagesCompletely();

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		ExceptionDebuggerThing();

		Log::Init(hModule);
		//DEBUGOUT("m0d-s0beit-v loaded"); 

		if (!GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &g_MainModuleInfo, sizeof(g_MainModuleInfo))) {
			Log::Fatal("Unable to get MODULEINFO from GTA5.exe");
		}

		DEBUGOUT("GTA5 [0x%I64X][0x%X]", g_MainModuleInfo.lpBaseOfDll, g_MainModuleInfo.SizeOfImage);

		//DisableLegalMessagesCompletely();
		//NoIntro();
		ApplyPatches(); //Apply patches, disable loading screens, etc this was on before (Memory writes)
		SpawnScriptHook(); //Hook that.
		
		//BypassOnlineModelRequestBlock(); //This allows us to spawn models on the LE XD INTERNET
	}

	return TRUE;
}

#include "GameInit.h"

static int CustomGameElementCall(char* element)
{
	uint32_t hash = *(uint32_t*)(element + 16);

	trace("Entered game element %08x.\n", hash);

	uintptr_t func = *(uintptr_t*)(element + 32);
	int retval = ((int(*)())func)();

	trace("Exited game element %08x.\n", hash);

	return retval;
}

void ApplyPatches() {
	//Disable logos since they add loading time
	UINT64 logos = FindPattern("platform:/movies/rockstar_logos", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	if (logos != 0)
	{
		//memset((void*)(logos + 0x11), 0x00, 0x0E);
		memcpy((void*)logos, "./nonexistingfilenonexistingfil", 32);

		//DisableLegalMessagesCompletely();
		NoIntro();

#ifdef HOOKS_ENABLED
		InitFunctionBase::RunAll();
		HookFunction::RunAll();
#endif
#if 0
		// game elements for crash handling purposes
		char* vtablePtrLoc = hook::pattern("41 89 40 10 49 83 60 18 00 48 8D 05").count(1).get(0).get<char>(12);
		void* vtablePtr = (void*)(*(int32_t*)vtablePtrLoc + vtablePtrLoc + 4);
		hook::put(&((uintptr_t*)vtablePtr)[1], CustomGameElementCall);
#endif
#if 0
		// ignore steam requirement
		auto pattern = hook::pattern("FF 15 ? ? ? ? 84 C0 74 0C B2 01 B9 91 32 25");// 31 E8");
		if (pattern.size() > 0)
		{
			hook::nop(pattern.get(0).get<void>(0), 6);
			hook::put<uint8_t>(pattern.get(0).get<void>(8), 0xEB);
		}
#endif
		//Start loading..
	}
}

void DisableLegalMessagesCompletely()
{
	char* addr[32][256][64] = {
		{ "\x72\x1F\xE8\x12\x8D\xFB\x00\x8B\x0D\x34\x7F\xC3\x01\xFF\xC1\x48", "xxx ? ? ? xxx ? ? ? xxxx"},
		{ "\x48\x85\xC9\x0F\x84\xED\x00\x00\x00\x48\x8D\x55\xA7\xE8", "xxx ? ? ? xxx ? ? ? xxxx"}
	};

	UINT64 address[32];
	for (int i = 0; i < 32; i++) {
		address[i] = FindPattern((char*)addr[i][0], (char*)addr[i][1]);

		if (address[i] != 0)
		{
			unsigned long dwProtect;
			unsigned long dwProtect2;

			VirtualProtect((LPVOID)address[i], 2, PAGE_EXECUTE_READWRITE, &dwProtect);
			*(unsigned short*)(address[i]) = 0x9090;
			VirtualProtect((LPVOID)address[i], 2, dwProtect, &dwProtect2);
		}
	}
}