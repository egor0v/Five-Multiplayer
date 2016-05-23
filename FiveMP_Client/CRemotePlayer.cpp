//----------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2004-2005 SA:MP team
//
// File Author(s): kyeman
// License: See LICENSE in root directory
//
//----------------------------------------------------

#include "stdafx.h"
#include "StdInc.h"
#include "CPlayerPed.h"
#include "CVehicle.h"
#include "natives.h"

//extern CGame		 *pGame;
extern CChatWindow   *pChatWindow;
//extern CCmdWindow	 *pCmdWindow;

using namespace RakNet;
extern CNetworkManager* pNetowkManager;

//----------------------------------------------------

CRemotePlayer::CRemotePlayer()
{
	m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
	m_playerID = INVALID_ENTITY_ID;
	m_bIsActive = FALSE;
	m_bIsWasted = FALSE;
	m_pPlayerPed = NULL;
	//m_wKeys = 0;
	m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
	m_iJustSpawned = 0;
	//m_fRotation = 0.0f;
	m_vehicleID = 0;
	m_bytePassengerSeat = 0;
	m_bHasAim = false;
	m_bShooting = false;
	m_curWeaponHash = 0;
}

//----------------------------------------------------

CRemotePlayer::~CRemotePlayer()
{
	if (m_pPlayerPed) {
#if 0 //TODO ADD THIS
		if (m_pPlayerPed->IsInVehicle()) {
			m_pPlayerPed->RemoveFromVehicleAndPutAt(0.0f, 0.0f, 0.0f);
		}
		m_pPlayerPed->SetHealth(0.0f);
#endif
		m_pPlayerPed->Destroy();
		m_pPlayerPed = nullptr;
		//delete m_pPlayerPed;
	}
}

//----------------------------------------------------

void CRemotePlayer::Process()
{
	CPlayerManager *pPool = pNetowkManager->GetPlayerManager();
	CVehicleManager *pVehicleManager = pNetowkManager->GetVehicleManager();
	CLocalPlayer *pLocalPlayer = pPool->GetLocalPlayer();

	if (m_bIsActive)
	{
		if (m_pPlayerPed != NULL)
		{
			// skip these frames if they've only just spawned.
			if (m_iJustSpawned != 0) {
				m_iJustSpawned--;
				return;
			}

			HandleVehicleEntryExit();

			if (m_byteUpdateFromNetwork == UPDATE_TYPE_FULL_ONFOOT)
			{
				//m_pPlayerPed->CheckAndRepairInvisProblems();

				// Handle dead stuff
				if (m_byteHealth == 0) {
					pChatWindow->AddDebugMessage("%s has 0 health so killing\n", pNetowkManager->GetPlayerManager()->GetAt(m_playerID));
					//m_wKeys = 0;
					m_vehicleID = 0;
					//m_pPlayerPed->SetKeys(m_wKeys);
					m_pPlayerPed->SetHealth(0);
					m_bIsWasted = TRUE;
					m_bIsActive = FALSE;
					return;
				}

				// Update the ingame player.
				UpdateOnFootPosition(m_matWorld.vPos);

				// update current weapon
				if (m_pPlayerPed->GetCurrentWeapon() != m_curWeaponHash) {
					m_pPlayerPed->SetArmedWeapon(m_curWeaponHash);
					//WEAPON::GIVE_WEAPON_TO_PED((Ped)m_pPlayerPed->curPedPtr, m_curWeaponHash, -1, true, true);
					//m_oldWeaponHash = m_curWeaponHash;
				}
#if 0
				// update current weapon
				if (m_pPlayerPed->GetCurrentWeapon() != m_byteCurrentWeapon) {
					m_pPlayerPed->SetArmedWeapon(m_byteCurrentWeapon);

					// they don't have the new weapon, give it to them
					if (m_pPlayerPed->GetCurrentWeapon() != m_byteCurrentWeapon) {
						m_pPlayerPed->GiveWeapon(m_byteCurrentWeapon, 9999);
						m_pPlayerPed->SetArmedWeapon(m_byteCurrentWeapon);
					}
				}

				m_pPlayerPed->SetShootingFlags(m_byteShootingFlags);
#endif
				m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
			}
			else if (m_byteUpdateFromNetwork == UPDATE_TYPE_FULL_INCAR)
			{
				CVehicle *pVehicle = pVehicleManager->GetAt(m_vehicleID);

				if (pVehicle) {
					UpdateInCarMatrixAndSpeed(&m_matWorld, &m_vecMoveSpeed, &m_vecTurnSpeed);
					pVehicle->SetHealth(m_fVehicleHealth); //TODO: gotta add this
				}

				m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
			}
			//m_pPlayerPed->SetRotation(0, matrix.quat[2], 0); //quats broke everything.. TODO: Fix
			m_pPlayerPed->SetHealth(200); //In GTA V max health is 200
			m_pPlayerPed->SetArmour(200); //Max armour 200 as well? idk...
		}
	}
	else {
		m_vehicleID = 0; // make sure that's always reset.
	}
}

//----------------------------------------------------

void CRemotePlayer::HandleVehicleEntryExit()
{
	//TODO: Add GetHealth, IsInVehicle and whatever is in red..
#if 0
	CVehicleManager *pVehicleManager = pNetowkManager->GetVehicleManager();

	if (m_vehicleID == 0 && m_pPlayerPed->IsInVehicle())
	{
		m_pPlayerPed->RemoveFromVehicleAndPutAt(m_matWorld.vPos.X,
			m_matWorld.vPos.Y, m_matWorld.vPos.Z);
	}
	else if ((m_vehicleID != 0) && !m_pPlayerPed->IsInVehicle())
	{
		// must force in
		CVehicle * pVehicle = pVehicleManager->GetAt(m_vehicleID);

		if (pVehicle && pVehicle->GetHealth() > 0.0f) {
			if (!m_bIsAPassenger) {
				m_pPlayerPed->PutDirectlyInVehicle(pVehicleManager->FindGtaIDFromID(m_vehicleID));
			}
			else {
				m_pPlayerPed->ForceIntoPassengerSeat(pVehicleManager->FindGtaIDFromID(m_vehicleID), m_bytePassengerSeat);
			}
		}
	}

	}
#endif
}
void CRemotePlayer::RemotePlayerDecideShoot(Vector3 vPos, Vector3 Position, float dist, DWORD timeTakes) {
	//FiringPattern.FullAuto = 0xC6EE6B4C
	if (dist < 0.25 && (m_pPlayerPed->GetVelocity().x == 0 && m_pPlayerPed->GetVelocity().y == 0 && m_pPlayerPed->GetVelocity().z == 0)) {
		if (!m_bShooting) {
			AI::TASK_AIM_GUN_AT_COORD((Ped)m_pPlayerPed->curPedPtr, m_vecAimPos.x, m_vecAimPos.y, m_vecAimPos.z, timeTakes, 0, 0);
		} else {
			AI::TASK_SHOOT_AT_COORD((Ped)m_pPlayerPed->curPedPtr, m_vecAimPos.x, m_vecAimPos.y, m_vecAimPos.z, timeTakes, 0xC6EE6B4C);
		}
	} else {
		if (dist > 0.25) {
			AI::TASK_GO_TO_COORD_WHILE_AIMING_AT_COORD((Ped)m_pPlayerPed->curPedPtr, vPos.x, vPos.y, vPos.z, m_vecAimPos.x, m_vecAimPos.y, m_vecAimPos.z, 
				GetSpeedFloat(), m_bShooting, GetSpeedFloat(), 0.5, false, 0 /* flags were 512*/, 0, 0xC6EE6B4C);
		}
	}
	if (m_bShooting) GAMEPLAY::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(Position.x, Position.y, Position.z, m_vecAimPos.x, m_vecAimPos.y, m_vecAimPos.z, 30, true, m_pPlayerPed->GetCurrentWeapon(), (Ped)m_pPlayerPed->curPedPtr, TRUE, FALSE, 999);
	m_bHasAim = false;
	m_bShooting = false;
}
//----------------------------------------------------
//#include "CMaths.h"
void CRemotePlayer::UpdateOnFootPosition(Vector3 vPos)
{
	//Call setPosition here with the ped pointer I guess TODO, should add a switch for parachuting here too
	if (!m_pPlayerPed) return;
	//CPlayerManager * pPlayerManager = pNetowkManager->GetPlayerManager();
	float quat[4];
	m_pPlayerPed->GetQuaternion(quat); //Seems to be a broken implementation.......
	Vector3 Position, FacingCoords;
	m_pPlayerPed->GetPosition(&Position);
	//We really need a switch case here for aim data, foot data, etc
	float dist = Math::GetDistanceBetweenPoints3D(Position.x, Position.y, Position.z, vPos.x, vPos.y, vPos.z);
	float heading = atan2f(vPos.y, vPos.x) - atan2f(Position.y, Position.x);
	DWORD timeTakes = GetTickCount() - m_pPlayerPed->GetTickNextAction();

	if (m_bHasAim || m_bShooting) {
		// Update aiming...
		RemotePlayerDecideShoot(vPos, Position, dist, timeTakes);
	} else {
		if (dist < 0.5 && (m_pPlayerPed->GetVelocity().x == 0 && m_pPlayerPed->GetVelocity().y == 0 && m_pPlayerPed->GetVelocity().z == 0)) {
			FacingCoords = m_pPlayerPed->GetCoordsFromVecForUsingVec(m_vecForward, 5.0);
			AI::TASK_TURN_PED_TO_FACE_COORD((Ped)m_pPlayerPed->curPedPtr, FacingCoords.x, FacingCoords.y, FacingCoords.z, 5000); //What's that last param? Maybe it's the turning speed or something?
		} else {
			if (dist > 0.5) {
				//Just clamp the velocity if it's greater..
				if (m_moveState == 1) {
					if (STREAMING::HAS_ANIM_DICT_LOADED("move_p_m_zero")) {
						if (GetTickCount() > m_pPlayerPed->GetTickNextSprint()) {
							AI::TASK_PLAY_ANIM((Ped)m_pPlayerPed->curPedPtr, "move_p_m_zero", "sprint", 8.0f, 0.0f, -1, 32, 0, 0, 0, 0);
							m_pPlayerPed->SetTickNextSprint(GetTickCount() + 1000.0); //Allow a second..
						}
					}
					m_pPlayerPed->SetRotation(m_playerRot.y, m_playerRot.z, m_playerRot.x);
					m_pPlayerPed->SetPedSpeed((Entity)m_pPlayerPed->curPedPtr, m_speed); //Set extra velocity, so they actually "run".
				} else {
					AI::TASK_GO_STRAIGHT_TO_COORD((Ped)m_pPlayerPed->curPedPtr, vPos.x, vPos.y, vPos.z, GetSpeedFloat(), timeTakes, 0.0/* heading */, 0.0f);
				}
			}
			if (dist > 30.0) {
				vPos.z -= 1.0f;
				m_pPlayerPed->SetPosition(vPos);
				m_pPlayerPed->SetRotation(0, m_playerRot.z, 0);
			}
		}
	}
	if (!m_speed.x && !m_speed.y && !m_speed.z) {
		AI::TASK_PLAY_ANIM((Ped)m_pPlayerPed->curPedPtr, "move_p_m_zero", "idle", 8.0f, 0.0f, -1, 32, 0, 0, 0, 0);
	}
	m_pPlayerPed->SetTickNextAction(GetTickCount());
}
//----------------------------------------------------

void CRemotePlayer::StoreOnFootFullSyncData(PLAYER_SYNC_DATA * pPlayerSyncData)
{
	m_vehicleID = 0;
	//m_wKeys = pPlayerSyncData->wKeys;
	memcpy(&m_matWorld.vPos, &pPlayerSyncData->vecPos, sizeof(Vector3));
	//m_pPlayerPed->SetQuaternion(pPlayerSyncData->matrix.quat);
	m_vecForward = pPlayerSyncData->m_vecForward;
	//m_fRotation = pPlayerSyncData->matrix.quat[2];
	m_byteCurrentWeapon = pPlayerSyncData->byteCurrentWeapon;
	m_byteShootingFlags = pPlayerSyncData->byteShootingFlags;
	m_byteHealth = pPlayerSyncData->byteHealth;
	m_byteArmour = pPlayerSyncData->byteArmour;
	
	//must reenable
	m_bHasAim = pPlayerSyncData->m_bHasAim;
	m_bShooting = pPlayerSyncData->m_bShooting;
	m_curWeaponHash = pPlayerSyncData->m_curWeaponHash;
	memcpy(&m_vecAimPos, &pPlayerSyncData->m_vecAimPos, sizeof(Vector3));
	m_speed = pPlayerSyncData->m_speed;
	m_moveState = pPlayerSyncData->m_moveState;
	memcpy(&m_playerRot, &pPlayerSyncData->m_playerRot, sizeof(Vector3));
	m_byteUpdateFromNetwork = UPDATE_TYPE_FULL_ONFOOT;
}

//---------------------------------------------------

//----------------------------------------------------

void CRemotePlayer::UpdateInCarMatrixAndSpeed(MATRIX2X4 * matWorld, Vector3 * vecMoveSpeed, Vector3 * vecTurnSpeed)
{
	MATRIX2X4 matVehicle;
	CVehicle * pVehicle = pNetowkManager->GetVehicleManager()->GetAt(m_vehicleID);
	float fDif;

	if (pVehicle) {
		//We don't have these functions yet, gotta add TODO
		//What does this code even do again?
		//It just seems to normalize the x y z values if any of the values sent is past x amount, pretty much compensates for the client
#if 0
		pVehicle->GetMatrix(&matVehicle);
		//memcpy(&matVehicle.vecRoll, &matWorld->vecRoll, sizeof(float[4*4]));
		//std::copy(matVehicle.vecRoll, matVehicle.vecRoll + 16, matWorld->vecRoll);
		/*
		for (int x = 0; x<4 * 4; x++) {
			matVehicle.vecRoll[x] = matWorld->vecRoll[x];
		}
		*/
		//pVehicle->SetQuaternion(quat);
		//memcpy(&matVehicle.vLookUp, &matWorld->vLookUp, sizeof(Vector3));
		if (matWorld->vPos.x >= matVehicle.vPos.x) {
			fDif = matWorld->vPos.x - matVehicle.vPos.x;
		}
		else {
			fDif = matVehicle.vPos.x - matWorld->vPos.x;
		}

		if (fDif > 0.1f) {
			matVehicle.vPos.x = matWorld->vPos.x;
		}

		if (matWorld->vPos.y >= matVehicle.vPos.y) {
			fDif = matWorld->vPos.y - matVehicle.vPos.y;
		}
		else {
			fDif = matVehicle.vPos.y - matWorld->vPos.y;
		}

		if (fDif > 0.1f) {
			matVehicle.vPos.y = matWorld->vPos.y;
		}

		if (matWorld->vPos.z >= matVehicle.vPos.z) {
			fDif = matWorld->vPos.z - matVehicle.vPos.z;
		}
		else {
			fDif = matVehicle.vPos.z - matWorld->vPos.z;
		}

		if (fDif > 0.1f) {
			matVehicle.vPos.z = matWorld->vPos.z;
		}
#endif
		Vector3 vecPos = { matWorld->vPos.x, matWorld->vPos.z, matWorld->vPos.z };
		pVehicle->SetPos(vecPos);
		pVehicle->SetMoveSpeed(*vecMoveSpeed);
		pVehicle->SetTurnSpeed(*vecTurnSpeed);
		//pVehicle->SetMatrix(matVehicle);
	}
}

//----------------------------------------------------

void CRemotePlayer::StoreInCarFullSyncData(VEHICLE_SYNC_DATA * pVehicleSyncData)
{
	m_vehicleID = pVehicleSyncData->vehicleID;
	//m_wKeys = pVehicleSyncData->wKeys;
	//memcpy(&m_matWorld.vecRoll, &pVehicleSyncData->vecRoll, sizeof(float[4*4]));
	//std::copy(m_matWorld.vecRoll, m_matWorld.vecRoll + 16, pVehicleSyncData->vecRoll);
	CVehicle * pGameVehicle = NULL;
	CVehicleManager * pVehicleManager = pNetowkManager->GetVehicleManager();
	pGameVehicle = pVehicleManager->GetAt(pVehicleSyncData->vehicleID);
	if (!pGameVehicle) return;
	/*
	for (int x = 0; x<4 * 4; x++) {
		m_matWorld.vecRoll[x] = pVehicleSyncData->vecRoll[x];
	}
	*/
	pGameVehicle->SetQuaternion(pVehicleSyncData->matrix.quat);
	//memcpy(&m_matWorld.vLookUp, &pVehicleSyncData->vecDirection, sizeof(float));
	//std::copy(pVehicleSyncData->vecPos, pVehicleSyncData->vecPos + 3, m_matWorld.vPos);
	memcpy(&m_matWorld.vPos, &pVehicleSyncData->vecPos, sizeof(Vector3));
	memcpy(&m_vecMoveSpeed, &pVehicleSyncData->vecMoveSpeed, sizeof(Vector3));
	memcpy(&m_vecTurnSpeed, &pVehicleSyncData->vecTurnSpeed, sizeof(Vector3));
	m_fVehicleHealth = UNPACK_VEHICLE_HEALTH(pVehicleSyncData->byteVehicleHealth);
	m_byteHealth = pVehicleSyncData->bytePlayerHealth;
	m_byteArmour = pVehicleSyncData->bytePlayerArmour;
	m_bIsInVehicle = TRUE;
	m_bIsAPassenger = FALSE;
	m_byteUpdateFromNetwork = UPDATE_TYPE_FULL_INCAR;
}

//----------------------------------------------------

void CRemotePlayer::StorePassengerData(EntityId vehicleID, BYTE byteSeat)
{
	m_bIsInVehicle = TRUE;
	m_bIsAPassenger = TRUE;
	m_vehicleID = vehicleID;
	m_bytePassengerSeat = byteSeat;
}

//----------------------------------------------------
BOOL CRemotePlayer::SpawnPlayer(BYTE byteTeam, BYTE byteSkin,
	Vector3 * vecPos, float fRotation, int iSpawnWeapon1,
	int iSpawnWeapon1Ammo, int iSpawnWeapon2,
	int iSpawnWeapon2Ammo, int iSpawnWeapon3,
	int iSpawnWeapon3Ammo)
{
	if (m_pPlayerPed != NULL) {
		m_pPlayerPed->Destroy();
		delete m_pPlayerPed;
	}

	CPlayerPed *pGamePlayer = new CPlayerPed(1, byteSkin, vecPos->x, vecPos->y, vecPos->z, fRotation);
	//Here's where le magic begins, we need to call createped
	if (pGamePlayer)
	{
		//TODO: Add showmarker
#if 0
		
		if (pNetowkManager->m_byteShowOnRadar) {
			pGamePlayer->ShowMarker(byteTeam);
		}

		

		if (iSpawnWeapon3 != (-1)) {
			pGamePlayer->GiveWeapon(iSpawnWeapon3, 9999);
		}
		if (iSpawnWeapon2 != (-1)) {
			pGamePlayer->GiveWeapon(iSpawnWeapon2, 9999);
		}
		if (iSpawnWeapon1 != (-1)) {
			pGamePlayer->GiveWeapon(iSpawnWeapon1, 9999);
		}
#endif
		m_pPlayerPed = pGamePlayer;
		m_bIsActive = TRUE;
		m_iJustSpawned = 30;
		m_vehicleID = 0;
		//m_fRotation = fRotation;
		//m_pPlayerPed->SetQuaternion(pPlayerSyncData->matrix.quat);
		m_byteTeam = byteTeam;
		m_byteHealth = 200;

		return TRUE;
	}
	return FALSE;
}

//----------------------------------------------------

void CRemotePlayer::HandleDeath(BYTE byteReason, BYTE byteWhoKilled, BYTE byteScoringModifier)
{

	char * szPlayerName = pNetowkManager->GetPlayerManager()->GetPlayerName(m_playerID);
	char * szWhoKilledName;

	if (byteWhoKilled != INVALID_ENTITY_ID) {
		szWhoKilledName = pNetowkManager->GetPlayerManager()->GetPlayerName(byteWhoKilled);
	}

#if 0
	switch (byteScoringModifier) {

	case VALID_KILL:
		pChatWindow->AddInfoMessage(">> %s was killed by %s", szPlayerName, szWhoKilledName);
		break;

	case TEAM_KILL:
		pChatWindow->AddInfoMessage(">> %s killed team-mate %s", szWhoKilledName, szPlayerName);
		break;

	case SELF_KILL:
		if (byteReason == WEAPON_DROWN) {
			pChatWindow->AddInfoMessage(">> %s drowned", szPlayerName);
		}
		else {
			pChatWindow->AddInfoMessage(">> %s died", szPlayerName);
		}
		break;
	}
#endif
	if (m_vehicleID != 0) {
		m_bIsWasted = TRUE;
		m_bIsActive = FALSE;
		m_vehicleID = 0;
	}
}

//----------------------------------------------------

void CRemotePlayer::Say(char *szText)
{
	char * szPlayerName = pNetowkManager->GetPlayerManager()->GetPlayerName(m_playerID);
	pChatWindow->AddChatMessage(szPlayerName, GetTeamColorAsARGB(), szText);
}


//----------------------------------------------------

float CRemotePlayer::GetDistanceFromRemotePlayer(CRemotePlayer *pFromPlayer)
{
	Vector3 vecThisPlayer{ 0, 0, 0 };
	Vector3 vecFromPlayer{ 0, 0, 0 };
	float  fSX, fSY;

	if (!pFromPlayer->IsActive()) return 10000.0f; // very far away
	if (!m_pPlayerPed) return 10000.0f; // very far away

	m_pPlayerPed->GetPosition(&vecThisPlayer);
	pFromPlayer->GetPlayerPed()->GetPosition(&vecFromPlayer);

	fSX = (vecThisPlayer.x - vecFromPlayer.x) * (vecThisPlayer.x - vecFromPlayer.x);
	fSY = (vecThisPlayer.y - vecFromPlayer.y) * (vecThisPlayer.y - vecFromPlayer.y);

	return (float)sqrt(fSX + fSY);
}

//----------------------------------------------------

float CRemotePlayer::GetDistanceFromLocalPlayer()
{
	Vector3 vecThisPlayer{ 0, 0, 0 };
	Vector3 vecFromPlayer{ 0, 0, 0 };
	float  fSX, fSY;

	CLocalPlayer *pLocalPlayer = pNetowkManager->GetPlayerManager()->GetLocalPlayer();
	CPlayerPed *pLocalPlayerPed = (CPlayerPed*)pLocalPlayer->GetPlayerPed();

	if (!m_pPlayerPed) return 10000.0f; // very far away
	if (!pLocalPlayerPed) return 10000.0f; // very far away
	
	m_pPlayerPed->GetPosition(&vecThisPlayer);
	pLocalPlayerPed->GetPosition(&vecFromPlayer);

	fSX = (vecThisPlayer.x - vecFromPlayer.x) * (vecThisPlayer.x - vecFromPlayer.x);
	fSY = (vecThisPlayer.y - vecFromPlayer.y) * (vecThisPlayer.y - vecFromPlayer.y);

	return (float)sqrt(fSX + fSY);
}

//----------------------------------------------------

DWORD CRemotePlayer::GetTeamColorAsRGBA()
{
	return 0;
	//return TranslateColorCodeToRGBA(m_byteTeam);
}

//----------------------------------------------------

DWORD CRemotePlayer::GetTeamColorAsARGB()
{
	return 0;
	//return (TranslateColorCodeToRGBA(m_byteTeam) >> 8) | 0xFF000000;
}

//----------------------------------------------------

void CRemotePlayer::InflictDamage(bool bPlayerVehicleDamager, EntityId damagerID, int iWeapon, float fUnk, int iPedPieces, BYTE byteUnk)
{
#if 0
	DWORD dwEntity;

	if (bPlayerVehicleDamager)
	{
		dwEntity = (DWORD)pNetowkManager->GetPlayerManager()->GetAt(damagerID)->GetPlayerPed()->GetEntity();
	}
	else
	{
		dwEntity = (DWORD)pNetowkManager->GetVehicleManager()->GetAt(damagerID)->GetVehicle();
	}

	/* TODO: Inflict damage */

	// Disable CPed::InflictDamage for remote players
#endif
}

//----------------------------------------------------