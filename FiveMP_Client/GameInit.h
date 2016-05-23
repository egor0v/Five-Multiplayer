/*
* This file is part of the CitizenFX project - http://citizen.re/
*
* See LICENSE and MENTIONS in the root of the source tree for information
* regarding licensing.
*/
#ifndef GAMEINIT_H
#define GAMEINIT_H

#include <scrThread.h>
#include <stdint.h>
#include <ICoreGameInit.h>

// BLIP_8 in global.gxt2 -> 'Waypoint'
#define BLIP_WAYPOINT 8

extern fwEvent<> OnGameFrame;
extern fwEvent<> OnGameFinalizeLoad;
extern fwEvent<> OnGameRequestLoad;

static DWORD WINAPI StaticThreadStart(void* Param);

/*
* This file is part of the CitizenFX project - http://citizen.re/
*
* See LICENSE and MENTIONS in the root of the source tree for information
* regarding licensing.
*/

struct ScInAddr
{
	uint64_t unkKey1;
	uint64_t unkKey2;
	uint32_t secKeyTime; // added in 393
	uint32_t ipLan;
	uint16_t portLan;
	uint32_t ipUnk;
	uint16_t portUnk;
	uint32_t ipOnline;
	uint16_t portOnline;
	uint16_t pad3;
	uint32_t newVal; // added in 372
	uint64_t rockstarAccountId; // 463/505 addition - really R*? given this field one could easily replace everything with a Steam-like implementation only passing around user IDs...
};

struct ScSessionAddr
{
	uint8_t sessionId[16];
	ScInAddr addr;
};

class
#ifdef COMPILING_GTA_CORE_FIVE
	__declspec(dllexport)
#else
	__declspec(dllimport)
#endif
FiveGameInit: public ICoreGameInit
{
private:
	bool m_gameLoaded;

public:
	virtual bool GetGameLoaded() override;

	virtual void KillNetwork(const wchar_t* errorString) override;

	virtual bool TryDisconnect() override;

	virtual void SetPreventSavePointer(bool* preventSaveValue) override;

	virtual void LoadGameFirstLaunch(bool(*callBeforeLoad)()) override;

	virtual void ReloadGame() override;

	void SetGameLoaded();
};

extern
#ifdef COMPILING_GTA_CORE_FIVE
__declspec(dllexport)
#else
__declspec(dllimport)
#endif
FiveGameInit g_gameInit;

extern
#ifdef COMPILING_GTA_CORE_FIVE
__declspec(dllexport)
#else
__declspec(dllimport)
#endif
fwEvent<const char*> OnKillNetwork;
void ExceptionDebuggerThing();
#include <Hooking.h>
static hook::cdecl_stub<void()> doPresenceStuff([]()
{
	return hook::pattern("32 DB 38 1D ? ? ? ? 75 24 E8").count(1).get(0).get<void>(-6);
});
static hook::cdecl_stub<void()> shutdownScene([]()
{
	return hook::pattern("BB 01 00 00 00 8B CB E8 ? ? ? ? E8").count(1).get(0).get<void>(-0x4A);
});
static hook::cdecl_stub<void()> initScene([]()
{
	return hook::pattern("BF 01 00 00 00 48 8D 0D ? ? ? ? 8B D7 E8").count(1).get(0).get<void>(-0x31);
});
static hook::cdecl_stub<void()> initStreamingInterface([]()
{
	return hook::get_call(hook::pattern("41 8B CE E8 ? ? ? ? 48 8B 0D ? ? ? ? 48 8D").count(1).get(0).get<void>(3));
});
static void* getNetworkManager()
{
	static void** networkMgrPtr = nullptr;

	if (networkMgrPtr == nullptr)
	{
		char* func = (char*)hook::get_call(hook::pattern("74 50 E8 ? ? ? ? 84 C0 75 10 48").count(1).get(0).get<void>(2));
		func += 9;

		networkMgrPtr = (void**)(func + *(int32_t*)func + 4);
	}

	return *networkMgrPtr;
}
static hook::cdecl_stub<void(void*, ScSessionAddr*, int64_t, int)> joinGame([]()
{
	return hook::pattern("E8 ? ? ? ? 0F 10 87 ? ? ? ? 88 86 ? ? ? ? F3 0F 7F 86 ? ? ? ? 8A D8").count(1).get(0).get<void>(0);
});
static hook::cdecl_stub<void(int, int, int)> hostGame([]() -> void*
{
	// below is original pattern, obfuscated since 372, so will differ per EXE now
	//return hook::get_call(hook::pattern("BA 01 00 00 00 41 B8 05 01 00 00 8B 08 E9").count(1).get(0).get<void>(13));
	//return hook::get_call(hook::pattern("48 8B 41 10 BA 01 00 00 00 41 B8 05 01 00 00").count(1).get(0).get<void>(0x11));

	// 505 has it be a xchg-type jump
	uint8_t* loc = hook::pattern("BA 01 00 00 00 41 B8 05 01 00 00").count(1).get(0).get<uint8_t>(11);

	if (*loc == 0xE9)
	{
		loc = hook::get_call(loc);
	}

	return loc + 2;
});

static bool* didPresenceStuff;
#endif