//----------------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2004-2005 SA:MP team
//
// File Author(s): kyeman
// License: See LICENSE in root directory
//
//----------------------------------------------------------
#include "stdafx.h"
#include "StdInc.h"
#include "GTAControls.h"
#include "GTAClasses.h"

// Sync send rates
#define INCAR_SENDRATE_IDLE 200
#define INCAR_SENDRATE_ACTIVE 66
#define ONFOOT_SENDRATE_IDLE 60
#define ONFOOT_SENDRATE_ACTIVE 30

//extern CGame		 *pGame;
extern CChatWindow   *pChatWindow;
//extern CCmdWindow	 *pCmdWindow;

using namespace RakNet;
extern CNetworkManager* pNetowkManager;
//----------------------------------------------------------
CPlayerPed		*m_pInternalPlayer;

CPlayerPed* FindPlayerPed() {
	if (m_pInternalPlayer == NULL)	m_pInternalPlayer = new CPlayerPed();
	return m_pInternalPlayer;
};

CLocalPlayer::CLocalPlayer()
{
	m_bHasSpawnInfo = FALSE;
	m_pPlayerPed = (CPlayerPed*)FindPlayerPed();
	m_bIsActive = FALSE;
	m_bIsWasted = FALSE;
	m_wLastKeys = 0;
	m_dwLastSendTick = GetTickCount();
	m_dwLastPassengerSendTick = GetTickCount();
}

//----------------------------------------------------------

BOOL CLocalPlayer::DestroyPlayer()
{
	return TRUE;
}

//----------------------------------------------------------
BOOL CLocalPlayer::Process()
{
	CSpawnSelection *pGameLogic;
	CVehicle *pGameVehicle;
	CVehicleManager *pVehicleManager;

	EntityId vehicleID;

	DWORD dwThisTick;

	if (m_bIsActive && !m_bIsWasted && (NULL != m_pPlayerPed))
	{
		// If they're in a vehicle and their health reaches 0 we need to
		// kill them manually or they don't actually die
		if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), true) && PLAYER::IS_PLAYER_DEAD(0)) {
			ENTITY::SET_ENTITY_HEALTH(PLAYER::PLAYER_PED_ID(), 0);
		}

		if ((PLAYER::IS_PLAYER_DEAD(0)) && !m_bIsWasted)
		{
			m_bIsWasted = TRUE;
			m_bIsActive = FALSE;
			SendWastedNotification();
		}
		else
		{
			dwThisTick = GetTickCount();

			if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), true))
			{
				if (!m_pPlayerPed->IsAPassenger())
				{
					// DRIVING VEHICLE
#if 0				//GTA V HAS NO BOUND DATA YET (We can add it though..)
					// VEHICLE WORLD BOUNDS STUFF
					pVehicleManager = pNetowkManager->GetVehicleManager();
					vehicleID = pVehicleManager->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
					if (vehicleID != INVALID_ENTITY_ID) {
						pGameVehicle = pVehicleManager->GetAt(vehicleID);
						pGameVehicle->EnforceWorldBoundries(
							pNetowkManager->m_WorldBounds[0], pNetowkManager->m_WorldBounds[1],
							pNetowkManager->m_WorldBounds[2], pNetowkManager->m_WorldBounds[3]);
					}
#endif
					if ((dwThisTick - m_dwLastSendTick) > (UINT)GetOptimumInCarSendRate()) {
						m_dwLastSendTick = GetTickCount();

						// send nothing while we're getting out.
						if (!PED::IS_PED_JUMPING_OUT_OF_VEHICLE(PLAYER::PLAYER_PED_ID())) {
							SendInCarFullSyncData();
						}
					}

					//pGame->DisablePassengerEngineAudio();
				}
				else
				{
					// PASSENGER

					if ((dwThisTick - m_dwLastPassengerSendTick) > 1000) {
						m_dwLastPassengerSendTick = GetTickCount();
						SendInCarPassengerData();
					}

					//pGame->EnablePassengerEngineAudio();
				}
			}
			else
			{
				// ONFOOT

				if ((dwThisTick - m_dwLastSendTick) > (UINT)GetOptimumOnFootSendRate()) {
					m_dwLastSendTick = GetTickCount();

					// send nothing while we're getting in
					if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), true)) //IsAPassenger()
					{
						SendOnFootFullSyncData();
					}
				}

				//pGame->DisablePassengerEngineAudio();
			}
		}
	}
	else if ((m_bIsWasted == TRUE) &&
		(!PLAYER::IS_PLAYER_DEAD(0))) //m_pPlayerPed->GetAction() != ACTION_WASTED
	{
		m_bIsWasted = FALSE;
		pNetowkManager->GetGameLogic()->HandleClassSelection(this);
		return TRUE;
	}
	if (UI::IS_PAUSE_MENU_ACTIVE())
	{
		m_iPause = UI::IS_PAUSE_MENU_ACTIVE();
		SendPauseNotification(m_iPause);
	}

	pGameLogic = pNetowkManager->GetGameLogic();

	if (pGameLogic) pGameLogic->ProcessLocalPlayer(this);

	return TRUE;
}

Vector3 CLocalPlayer::Player_ShootLine(Vector3 m_vecPos, float dist) {
	//http://gtaforums.com/topic/821682-c-asi-scripting-in-scripthookv/
	Vector3 farCoords;
	if (m_pPlayerPed) {
		Vector3 camCoords = CAM::GET_GAMEPLAY_CAM_COORD();
		farCoords = getCoordsFromCam(dist); //get coords of point dist from your crosshair
	}
	return farCoords; //Just return this, whatever
}

void CLocalPlayer::SendOnFootFullSyncData()
{
	if (m_pPlayerPed)
	{
		RakNet::BitStream bsPlayerSync;
		PLAYER_SYNC_DATA playerSyncData;

		// write packet id
		bsPlayerSync.Write((MessageID)ID_PLAYER_SYNC);

		// get the player keys
		//playerSyncData.wKeys = 0;

		// get the player position
		m_pPlayerPed->GetPosition(&playerSyncData.vecPos);

		// get the player rotation
		//playerSyncData.fRotation = m_pPlayerPed->GetRotation();

		//m_pPlayerPed->GetQuaternion(playerSyncData.matrix.quat);

		// get player current weapon (casted to a byte to save space)
		playerSyncData.byteCurrentWeapon = 0;

		// get player shooting flags
		playerSyncData.byteShootingFlags = 0;

		// get player health (casted to a byte to save space)
		playerSyncData.byteHealth = (BYTE)ENTITY::GET_ENTITY_HEALTH((Ped)m_pPlayerPed->curPedPtr);

		// get player armour (casted to a byte to save space)
		playerSyncData.byteArmour = (BYTE)PED::GET_PED_ARMOUR((Ped)m_pPlayerPed->curPedPtr);

		//Get the forward vector
		playerSyncData.m_vecForward = m_pPlayerPed->GetForwardVector();

		//must reenable
		//Check if their aim is on / off and send it
		playerSyncData.m_bHasAim = CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, GTA::Control::Aim);

		//Are they shooting?
		playerSyncData.m_bShooting = CONTROLS::IS_DISABLED_CONTROL_PRESSED(0, GTA::Control::Attack);

		//Send the gun we're holding
		//if (myPed && myPed->_WeaponManager->_CurrentWeapon != 0) {
		if (WEAPON::IS_PED_ARMED((Ped)m_pPlayerPed->curPedPtr, 0xFF)) {
			playerSyncData.m_curWeaponHash = (UINT32)WEAPON::GET_SELECTED_PED_WEAPON((Ped)m_pPlayerPed->curPedPtr);
		} else {
			playerSyncData.m_curWeaponHash = 0;
		}

		//Now send the aim data, if we're either aiming or just shooting
		//CPed *myPed = (CPed*)m_pPlayerPed->curPedPtr; 
		if (playerSyncData.m_bHasAim || playerSyncData.m_bShooting) {
			/* if (myPed && myPed->_WeaponManager->_CurrentWeapon != 0) { */ //This crap doesn't even work
			if (WEAPON::IS_PED_ARMED((Ped)m_pPlayerPed->curPedPtr, 0xFF)) {
				playerSyncData.m_vecAimPos = Player_ShootLine(playerSyncData.vecPos, 5000.0f);
			} else {
				playerSyncData.m_vecAimPos = Vector3(); //If we don't have it just send a null vector
			}
		} else { //I smell crap code
			playerSyncData.m_vecAimPos = Vector3(); //If we don't have it just send a null vector
		}

		playerSyncData.m_speed = m_pPlayerPed->GetPedSpeed();

		playerSyncData.m_moveState = m_pPlayerPed->GetMoveState();

		playerSyncData.m_playerRot = m_pPlayerPed->GetRotation();
		// write player sync data struct
		bsPlayerSync.Write((char *)&playerSyncData, sizeof(PLAYER_SYNC_DATA));
		{
			// write a 0 bit to say the bit stream has no aim sync data
			bsPlayerSync.Write0();
		}
		// send sync data
		pNetowkManager->GetRakPeer()->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, TRUE);
	}
}
//----------------------------------------------------------
/*
void CLocalPlayer::SendOnFootFullSyncData()
{
	CLocalPlayer *pLocalPlayer = pNetowkManager->GetPlayerManager()->GetLocalPlayer();
	CPlayerPed *pLocalPlayerPed = (CPlayerPed*)pLocalPlayer->GetPlayerPed();
	if (m_pPlayerPed)
	{
		RakNet::BitStream bsPlayerSync;
		PLAYER_SYNC_DATA playerSyncData;
		//TODO: Add stuff missing here
		// write packet id
		bsPlayerSync.Write((MessageID)ID_PLAYER_SYNC);

		// get the player keys
		playerSyncData.wKeys = 0; //Send zero data for things we don't have

		// get the player position
		pLocalPlayerPed->GetPosition(&playerSyncData.vecPos);

		// get the player rotation
		playerSyncData.fRotation = pLocalPlayerPed->GetRotation();

		// get player current weapon (casted to a byte to save space)
		playerSyncData.byteCurrentWeapon = 0;

		// get player shooting flags
		playerSyncData.byteShootingFlags = 0;

		// get player health (casted to a byte to save space)
		playerSyncData.byteHealth = (BYTE)200;

		// get player armour (casted to a byte to save space)
		playerSyncData.byteArmour = (BYTE)200;

		// write player sync data struct
		bsPlayerSync.Write((char *)&playerSyncData, sizeof(PLAYER_SYNC_DATA));

#if 0
		// send aiming data if player has fire key held down and we have ammo
		if (m_pPlayerPed->IsFiring() && m_pPlayerPed->HasAmmoForCurrentWeapon())
		{
			// write a 1 bit to say the bit stream has aim sync data
			bsPlayerSync.Write1();

			// get the camera pointer
			CAMERA_TYPE * pCamera = pGame->GetCamera()->GetCamera();

			// write the aim sync data
			bsPlayerSync.Write((char *)&pCamera->aim, sizeof(CAMERA_AIM));
		}
		else
#endif
		{
			// write a 0 bit to say the bit stream has no aim sync data
			bsPlayerSync.Write0();
		}

		// send sync data
		pNetowkManager->GetRakPeer()->Send(&bsPlayerSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);
	}
}
*/
//----------------------------------------------------------
void CLocalPlayer::SendInCarFullSyncData()
{
	if(m_pPlayerPed)
	{
		RakNet::BitStream bsVehicleSync;
		VEHICLE_SYNC_DATA vehicleSyncData;
		MATRIX2X4 matVehicle;
		CVehicleManager * pVehicleManager = pNetowkManager->GetVehicleManager();
		CVehicle * pGameVehicle = NULL;

		// write packet id
		bsVehicleSync.Write((MessageID)ID_VEHICLE_SYNC);

		//vehicleSyncData.vehicleID = pVehicleManager->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
		
		Vehicle *vehicleID = (Vehicle*)PED::GET_VEHICLE_PED_IS_IN((Ped)m_pPlayerPed->curPedPtr, true);
		
		vehicleSyncData.vehicleID = pVehicleManager->FindIDFromGtaPtr(vehicleID);
		if (vehicleSyncData.vehicleID == INVALID_ENTITY_ID) return;

		pGameVehicle = pVehicleManager->GetAt(vehicleSyncData.vehicleID); //vehicleSyncData.vehicleID
		pGameVehicle->GetQuaternion(vehicleSyncData.matrix.quat);
		//pQuaternion->operator*(q);
#if 0
		//Simply calculates w for us, thanks chc
		float sum = 1.0f - x * x - y * y - z * z;
		w = sqrtf((sum < 0.0f) ? 0.0f : sum);
		//std::vector(x, y, z);
		Vector3 myVec;
		myVec.x = x;
		myVec.y = y;
		myVec.z = z;
#endif
		// copy the vehicle position
		memcpy(&vehicleSyncData.vecPos, &ENTITY::GET_ENTITY_COORDS((Entity)vehicleID, true), sizeof(Vector3));

		// get the vehicle move speed
		//pGameVehicle->GetMoveSpeed(&vehicleSyncData.vecMoveSpeed);
		vehicleSyncData.vecMoveSpeed = ENTITY::GET_ENTITY_SPEED_VECTOR((Entity)vehicleID, true);

		// get the vehicle turn speed
		vehicleSyncData.vecTurnSpeed = ENTITY::GET_ENTITY_ROTATION_VELOCITY((Entity)vehicleID);
		//pGameVehicle->GetTurnSpeed(&vehicleSyncData.vecTurnSpeed);

		// pack the vehicle health
		vehicleSyncData.byteVehicleHealth = PACK_VEHICLE_HEALTH(VEHICLE::GET_VEHICLE_BODY_HEALTH((Entity)vehicleID));

		// get the player health (casted to a byte to save space)
		vehicleSyncData.bytePlayerHealth = (BYTE)ENTITY::GET_ENTITY_HEALTH((Ped)m_pPlayerPed->curPedPtr);

		// get the player armour (casted to a byte to save space)
		vehicleSyncData.bytePlayerArmour = (BYTE)PED::GET_PED_ARMOUR((Ped)m_pPlayerPed->curPedPtr);

		// write vehicle sync struct to bit stream
		bsVehicleSync.Write((char *)&vehicleSyncData, sizeof(VEHICLE_SYNC_DATA));
		// send sync data
		pNetowkManager->GetRakPeer()->Send(&bsVehicleSync,HIGH_PRIORITY,UNRELIABLE_SEQUENCED,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);
	}
}
//----------------------------------------------------------

void CLocalPlayer::SendInCarPassengerData()
{
	RakNet::BitStream bsPassengerSync;
	Vector3 vPos;
	CVehicleManager *pVehicleManager = pNetowkManager->GetVehicleManager();
//#if 0
	//EntityId vehicleID = pVehicleManager->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
	DWORD vehicleID = PED::GET_VEHICLE_PED_IS_IN((Ped)m_pPlayerPed->curPedPtr, true);
	if(vehicleID == INVALID_ENTITY_ID) return;

	BYTE bytePassengerSeat = m_pPlayerPed->GetPedSeat((Ped)m_pPlayerPed->curPedPtr);
	
	m_pPlayerPed->GetPosition(&vPos);
	
	bsPassengerSync.Write((MessageID)ID_PASSENGER_SYNC);
	bsPassengerSync.Write(vehicleID);
	bsPassengerSync.Write(bytePassengerSeat);
	bsPassengerSync.Write((char *)&vPos, sizeof(Vector3));
//#endif
	pNetowkManager->GetRakPeer()->Send(&bsPassengerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
}

//----------------------------------------------------------

int CLocalPlayer::GetOptimumInCarSendRate()
{
	CVehicleManager *pVehicleManager = pNetowkManager->GetVehicleManager();
	CVehicle	 *pGameVehicle=NULL;
	Vector3		 vecMoveSpeed;
	EntityId		 vehicleID=0;

	if(m_pPlayerPed)
	{
#if 0
		//TODO: Fix all the stuff commented
		//vehicleID = pVehicleManager->FindIDFromGtaPtr(m_pPlayerPed->GetGtaVehicle());
		//pGameVehicle = pVehicleManager->GetAt(vehicleID);

		if(pGameVehicle)
		{
			//pGameVehicle->GetMoveSpeed(&vecMoveSpeed);

			// is the vehicle moving?
			if(!((vecMoveSpeed.x == 0.0f) && (vecMoveSpeed.y == 0.0f)))
			{
				return INCAR_SENDRATE_ACTIVE;
			}
		}
#endif
		return INCAR_SENDRATE_ACTIVE;
	}

	return INCAR_SENDRATE_IDLE;
}

//----------------------------------------------------------

int CLocalPlayer::GetOptimumOnFootSendRate()
{	
	Vector3	 vecMoveSpeed;

	if(m_pPlayerPed)
	{
		//TODO: FIX THIS
		vecMoveSpeed = m_pPlayerPed->GetVelocity();

		// is the player moving?
		if(!((vecMoveSpeed.x == 0.0f) && (vecMoveSpeed.y == 0.0f)))
		{
			return ONFOOT_SENDRATE_ACTIVE;
		}
	}

	return ONFOOT_SENDRATE_IDLE;
}

//----------------------------------------------------------

void CLocalPlayer::SendWastedNotification()
{
	CSpawnSelection *pGameLogic;
	RakNet::BitStream bsPlayerDeath;
	BYTE byteDeathReason;
	BYTE byteWhoWasResponsible;

	//TODO, FIX THIS
	//byteDeathReason = m_pPlayerPed->FindDeathReasonAndResponsiblePlayer(&byteWhoWasResponsible);
	
	bsPlayerDeath.Write(byteDeathReason);
	bsPlayerDeath.Write(byteWhoWasResponsible);
	pNetowkManager->GetRPC4()->Call("Death",&bsPlayerDeath,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);
	
	pGameLogic = pNetowkManager->GetGameLogic();
}

//----------------------------------------------------------

void CLocalPlayer::RequestClass(int iClass)
{
	RakNet::BitStream bsSpawnRequest;
	bsSpawnRequest.Write(iClass);
	pNetowkManager->GetRPC4()->Call("RequestClass",&bsSpawnRequest,HIGH_PRIORITY,RELIABLE,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);
}

//----------------------------------------------------------

void CLocalPlayer::SetSpawnInfo(PLAYER_SPAWN_INFO * pSpawnInfo)
{
	memcpy(&m_SpawnInfo, pSpawnInfo, sizeof(PLAYER_SPAWN_INFO));
	m_bHasSpawnInfo = TRUE;
}

//----------------------------------------------------------

BOOL CLocalPlayer::SpawnPlayer()
{
	return SpawnPlayer(&m_SpawnInfo);
}

//----------------------------------------------------------

BOOL CLocalPlayer::SpawnPlayer(PLAYER_SPAWN_INFO * pSpawnInfo)
{
	//TODO, ADD WHATEVER NEEDS TO BE ADDED HERE

	CPlayerPed *pGamePlayer = (CPlayerPed*)FindPlayerPed();

	if(!pGamePlayer) return FALSE;	

	if(pGamePlayer)
	{
		//pGamePlayer->RestartIfWastedAt(&pSpawnInfo->vecPos, pSpawnInfo->fRotation);

		m_pPlayerPed = (CPlayerPed*)pGamePlayer;
#if 0
		// Set skin stuff.. logic is because of temperament
		if(pGamePlayer->GetModelIndex() != pSpawnInfo->byteSkin && (pSpawnInfo->byteSkin < 107)) {
			if(!pGame->IsModelLoaded(pSpawnInfo->byteSkin)) {
				pGame->RequestModel(pSpawnInfo->byteSkin);
				pGame->LoadRequestedModels();
				while(!pGame->IsModelLoaded(pSpawnInfo->byteSkin)) { Sleep(1); }
			}
			pGamePlayer->SetModel(pSpawnInfo->byteSkin);
		}

		pGamePlayer->ClearAllWeapons();

		if(pSpawnInfo->iSpawnWeapons[3] != -1) {
			pGamePlayer->GiveWeapon(pSpawnInfo->iSpawnWeapons[3], pSpawnInfo->iSpawnWeaponsAmmo[3]);
		}
		if(pSpawnInfo->iSpawnWeapons[2] != -1) {
			pGamePlayer->GiveWeapon(pSpawnInfo->iSpawnWeapons[2], pSpawnInfo->iSpawnWeaponsAmmo[2]);
		}		
		if(pSpawnInfo->iSpawnWeapons[1] != -1) {
			pGamePlayer->GiveWeapon(pSpawnInfo->iSpawnWeapons[1], pSpawnInfo->iSpawnWeaponsAmmo[1]);
		}

		pGamePlayer->Teleport(pSpawnInfo->vecPos.X, pSpawnInfo->vecPos.Y, pSpawnInfo->vecPos.Z);
		pGamePlayer->SetRotation(pSpawnInfo->fRotation);
		pGamePlayer->SetInitialState();
#endif
		m_bIsWasted = FALSE;
		m_bIsActive = TRUE;
	}
	// Let the rest of the network know we're spawning.
	RakNet::BitStream bsSendSpawn;
	pNetowkManager->GetRPC4()->Call("Spawn",&bsSendSpawn,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);

	return TRUE;
}

//----------------------------------------------------------

void CLocalPlayer::Say(PCHAR szText)
{
	if(!pNetowkManager->IsConnected()) {
		//pChatWindow->AddInfoMessage("Not connected");
		return;
	}

	BYTE byteTextLen = strlen(szText);

	RakNet::BitStream bsSend;
	bsSend.Write(byteTextLen);
	bsSend.Write(szText,byteTextLen);

	pNetowkManager->GetRPC4()->Call("Chat",&bsSend,HIGH_PRIORITY,RELIABLE,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);
}

//----------------------------------------------------------

void CLocalPlayer::SendEnterVehicleNotification(EntityId vehicleID, bool bPassenger)
{
	RakNet::BitStream bsSend;
	bsSend.Write(vehicleID);
	bsSend.Write(bPassenger);
	pNetowkManager->GetRPC4()->Call("EnterVehicle",&bsSend,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);
}

//----------------------------------------------------------

void CLocalPlayer::SendExitVehicleNotification(EntityId vehicleID)
{
	RakNet::BitStream bsSend;
	bsSend.Write(vehicleID);
	pNetowkManager->GetRPC4()->Call("ExitVehicle",&bsSend,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);
}

//----------------------------------------------------

DWORD CLocalPlayer::GetTeamColorAsRGBA()
{
	return 0;
	//return TranslateColorCodeToRGBA(m_SpawnInfo.byteTeam);
}

//----------------------------------------------------

DWORD CLocalPlayer::GetTeamColorAsARGB()
{
	return 0;
	//return (TranslateColorCodeToRGBA(m_SpawnInfo.byteTeam) >> 8) | 0xFF000000;	
}

//----------------------------------------------------------

void CLocalPlayer::SendInflictedDamageNotification(EntityId playerID, EntityId vehicleID, int iWeapon, float fUnk, int iPedPieces, BYTE byteUnk)
{
	BitStream bsSend;

	// Is the damager a player or a vehicle?
	bool bPlayerVehicleDamager = (playerID != INVALID_ENTITY_ID) ? true : false;

	// If its a player damager write a 1 and the player id
	if(bPlayerVehicleDamager)
	{
		bsSend.Write1();
		bsSend.Write(playerID);
	}
	else
	{
		// If its a vehicle damager write a 0 and the vehicle id
		bsSend.Write0();
		bsSend.Write(vehicleID);
	}

	bsSend.Write(iWeapon);
	bsSend.Write(fUnk);
	bsSend.Write(iPedPieces);
	bsSend.Write(byteUnk);
	//pNetowkManager->GetRPC4()->Call("InflictDamage", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
}

//----------------------------------------------------------

void CLocalPlayer::SendPauseNotification(int bPause)
{
	RakNet::BitStream bsSend;
	bsSend.Write(bPause);
	pNetowkManager->GetRPC4()->Call("Pause",&bsSend,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,UNASSIGNED_SYSTEM_ADDRESS,TRUE);
}

//----------------------------------------------------
