#include "stdafx.h"
#include "stdInc.h"
#include "modbaserequired.h"
#include "ResourceMissionCleanup_Minimal.h"
#include "GameInit.h"
#include "EnableMPMapData.h"

extern CChatWindow   *pChatWindow;
bool modbaseRequiredStarted = false;

int freezePlayer[MAX_PLAYERS+1];
Player moneyPlayer = -1;

GtaThread_VTable gGtaThreadOriginal;
GtaThread_VTable gGtaThreadNew;
BlipList* pBlipList;

HANDLE mainFiber;
DWORD wakeAt;

void WAIT(DWORD ms)
{
#ifndef __DEBUG
	wakeAt = timeGetTime() + ms;
	SwitchToFiber(mainFiber);
#endif
}

void RequestControl(Entity e)
{
	NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(e);
	if (!NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(e))
		WAIT(0);
	NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(e);
}

eThreadState Trampoline(GtaThread* This)
{
	rage::scrThread* runningThread = GetActiveThread();
	SetActiveThread(This);
	#ifdef  __DEBUG
	Run(); //We don't want to also call RunUnlireable, since it's expecting WAIT() to work, which it doesn't in debug mode. #depechemode
	#else
	Tick();
	#endif
	SetActiveThread(runningThread);
	return gGtaThreadOriginal.Run(This);
}

void __stdcall ReliableScriptFunction(LPVOID lpParameter)
{
	try
	{
		while (1)
		{
			Run();
			SwitchToFiber(mainFiber);
		}
	}
	catch (...)
	{
		Log::Fatal("Failed scriptFiber");
	}
}

void __stdcall HeavyWaitFunction(LPVOID lpParameter)
{
	try
	{
		while (1)
		{
			RunUnreliable();
			SwitchToFiber(mainFiber);
		}
	}
	catch (...)
	{
		Log::Fatal("Failed scriptFiber");
	}
}

void Tick()
{
	if (mainFiber == nullptr)
		mainFiber = ConvertThreadToFiber(nullptr);

	static HANDLE reliableFiber;
	if (reliableFiber)
		SwitchToFiber(reliableFiber);
	else
		reliableFiber = CreateFiber(NULL, ReliableScriptFunction, nullptr);

	if (timeGetTime() < wakeAt)
		return;

	static HANDLE scriptFiber;
	if (scriptFiber)
		SwitchToFiber(scriptFiber);
	else
		scriptFiber = CreateFiber(NULL, HeavyWaitFunction, nullptr);
}
void ApplyWorldPatchesOnTick();
void RunUnreliable() //Put functions that don't really need to be run every frame that can cause heavy wait times for the function here.
{
}

void loadSceneAndSpawnPlayer();
void loadSceneAndSpawnPlayer() {
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	SCRIPT::SET_NO_LOADING_SCREEN(true); //Remove that faux loading screen
	GAMEPLAY::SET_FADE_OUT_AFTER_DEATH(false);
	CAM::SET_CAM_FAR_CLIP(CAM::GET_RENDERING_CAM(), 1500.0); //Set the far clip to 1500 for now, less fps lag and cpu usage
	//GAMEPLAY::_DISABLE_AUTOMATIC_RESPAWN(true); //Wait until scripting is implemented
	//Kill these scripts
	detachMissions();

	PLAYER::FORCE_CLEANUP(8);
	PLAYER::FORCE_CLEANUP(64);

	//Load the scene and spawn the player
	STREAMING::LOAD_SCENE(30.6187f, -1877.898f, 22.4998f);
	SCRIPT::SHUTDOWN_LOADING_SCREEN();
	CAM::DO_SCREEN_FADE_IN(500);
	float groundz;
	GAMEPLAY::GET_GROUND_Z_FOR_3D_COORD(30.6187f, -1877.898f, 22.4998f, &groundz, 1);
	ENTITY::SET_ENTITY_COORDS(playerPed, 30.6187f, -1877.898f, groundz + 1.0, true, true, true, true);
	NETWORK::NETWORK_RESURRECT_LOCAL_PLAYER(30.6187f, -1877.898f, groundz + 1.0, 100.0, 1, 1);
	
	ENTITY::FREEZE_ENTITY_POSITION(playerPed, 0);
	ENTITY::SET_ENTITY_VISIBLE(playerPed, true, 0);

	int PedHash = GAMEPLAY::GET_HASH_KEY(pedModels[110]);
	STREAMING::REQUEST_MODEL(PedHash);
	while (!STREAMING::HAS_MODEL_LOADED(PedHash)) WAIT(0);
	PLAYER::SET_PLAYER_MODEL(player, PedHash);
	
	//Reset the ped pointer, in case missions change it.. temporary until work around is found..
	CPlayerPed *m_pPlayerPed = (CPlayerPed*)FindPlayerPed();
	m_pPlayerPed->curPedPtr = (Ped*)PLAYER::GET_PLAYER_PED(0);
	ENTITY::SET_ENTITY_AS_MISSION_ENTITY((Entity)m_pPlayerPed->curPedPtr, 0, 1);

	RevealFullMap(true); //Show the entire map to the player -- crashes everything?
	UI::DISPLAY_RADAR(true);
	UI::DISPLAY_HUD(true);
}
bool playerPatchesApplied = false;
int timeOurScene;
bool camSet = false;

#define 	INT32_MAX   0x7fffffffL
void ApplyWorldPatchesOnTick() {
	VEHICLE::SET_GARBAGE_TRUCKS(false);
	VEHICLE::SET_RANDOM_BOATS(false);
	VEHICLE::SET_RANDOM_TRAINS(false);
	VEHICLE::SET_FAR_DRAW_VEHICLES(false);

	VEHICLE::SET_RANDOM_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);
	VEHICLE::SET_NUMBER_OF_PARKED_VEHICLES(-1);
	VEHICLE::SET_ALL_LOW_PRIORITY_VEHICLE_GENERATORS_ACTIVE(true);
	STREAMING::SET_VEHICLE_POPULATION_BUDGET(0);
	STREAMING::SET_PED_POPULATION_BUDGET(0);
	VEHICLE::SET_VEHICLE_DENSITY_MULTIPLIER_THIS_FRAME(0.0);

	// Remove character selection wheel
	CONTROLS::DISABLE_CONTROL_ACTION(2, 19, true);

	if ((STREAMING::IS_NEW_LOAD_SCENE_ACTIVE() || CUTSCENE::IS_CUTSCENE_ACTIVE()) && playerPatchesApplied && (GetTickCount64() - timeOurScene) > 500) {
		STREAMING::NEW_LOAD_SCENE_STOP();
		CUTSCENE::STOP_CUTSCENE_IMMEDIATELY();
		CUTSCENE::REMOVE_CUTSCENE();
		loadSceneAndSpawnPlayer();
		playerPatchesApplied = false;
		camSet = false;
	}
	if (!camSet) {
		Ped playerPed = PLAYER::PLAYER_PED_ID();
		CAM::DO_SCREEN_FADE_IN(500);
		CAM::RENDER_SCRIPT_CAMS(0, 0, 3000, 1, 1);
		CAM::DESTROY_ALL_CAMS(0);
		camSet = true;
	}
}

bool connected = false;

extern CNetworkManager *pNetowkManager;

void Run() //Only call WAIT(0) here. The Tick() function will ignore wakeAt and call this again regardless of the specified wakeAt time.
{
	#ifdef __DEBUG
	static bool bQuit, F12 = false;
	if (isKeyPressedOnce(F12, VK_F12)){ bQuit = true; }
	if (bQuit) { return; }
	#endif
	//Run your natives here.
	Player player = PLAYER::PLAYER_ID();
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!playerPatchesApplied) {
		loadSceneAndSpawnPlayer();
		playerPatchesApplied = true;
		timeOurScene = GetTickCount64();
	}

	if (!modbaseRequiredStarted)
		initModBaseRequired();

	//OnTick
	if (pChatWindow) pChatWindow->HandleChatOnTick();
	handleKeysOnTick();
	ApplyWorldPatchesOnTick(); //These need to be called on tick (sorry clients)
	hudOnTick();
	if (!connected) {
		connectToServer();
		connected = true;
	}

	// Process the netgame if it's active.
	if (pNetowkManager) pNetowkManager->Process();
	//

	if (ENTITY::DOES_ENTITY_EXIST(playerPed) == TRUE)
	{
		Hash currentWeapon;
		Vehicle playerVeh = NULL;

		if (PED::IS_PED_IN_ANY_VEHICLE(playerPed, FALSE))
			playerVeh = PED::GET_VEHICLE_PED_IS_USING(playerPed);

		//Test that drawing works.
		//draw_menu_line("M5:O Alpha", 15.0f, 50.0f, 570.0f, 6.0f, 5.0f, false, false, false); //drawrect set to true
	}
	return;
}
GtaThread* pThreadCopy;
bool AttemptScriptHook()
{
	
	rage::pgPtrCollection<GtaThread>* threadCollection = NULL;

	while (!threadCollection) {
		threadCollection = GetGtaThreadCollection(pBlipList);
		Sleep(10);
		continue;
	}

	for (UINT16 i = 0; i < threadCollection->count(); i++) {
		GtaThread* pThread = threadCollection->at(i);

		if (!pThread)
			continue;

		//s0biet originally had some junk thread that was called for like 2 seconds then died. This thread is better.
		if (pThread->GetContext()->ScriptHash != MAIN_PERSISTENT) {
			continue;
		}

		// Now what? We need to find a target thread and hook its "Tick" function
		if (gGtaThreadOriginal.Deconstructor == NULL) {
			memcpy(&gGtaThreadOriginal, (DWORD64*)((DWORD64*)pThread)[0], sizeof(gGtaThreadOriginal)); //Create a backup of the original table so we can call the original functions from our hook.
			memcpy(&gGtaThreadNew, &gGtaThreadOriginal, sizeof(GtaThread_VTable)); //Construct our VMT replacement table.

			gGtaThreadNew.Run = Trampoline; //Replace the .Run method in the new table with our method.
		}

		if (((DWORD64*)pThread)[0] != (DWORD64)&gGtaThreadNew) { //If the table is not VMT Hooked.
			DEBUGOUT("Hooking thread [%i] (0x%X)", pThread->GetContext()->ThreadId, pThread->GetContext()->ScriptHash);
			((DWORD64*)pThread)[0] = (DWORD64)&gGtaThreadNew; //Replace the VMT pointer with a pointer to our new VMT.
			DEBUGOUT("Hooked thread [%i] (0x%X)", pThread->GetContext()->ThreadId, pThread->GetContext()->ScriptHash);
			pThreadCopy = pThread;
			return true;
		}
	}
	return false;
}

DWORD WINAPI lpHookScript(LPVOID lpParam) {
	while (!AttemptScriptHook()) {
		Sleep(100);
	}

	return 0; //We no longer need the lpHookScript thread because our Trampoline function will now be the hip and or hop hang out spot for the KewlKidzKlub�.
}

void SpawnScriptHook() {
	CreateThread(0, 0, lpHookScript, 0, 0, 0);
}