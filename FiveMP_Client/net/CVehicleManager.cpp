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
#include "CVehicle.h"

//extern CGame *pGame;
extern CNetworkManager *pNetowkManager;
extern CChatWindow *pChatWindow;

//----------------------------------------------------

CVehicleManager::CVehicleManager()
{
	for(int i = 0; i < MAX_VEHICLES; i++)
	{
		m_bVehicleSlotState[i] = FALSE;
		m_pVehicles[i] = NULL;
	}
}

//----------------------------------------------------

CVehicleManager::~CVehicleManager()
{	
	for (int i = 0; i < MAX_VEHICLES; i++)
	{
		if(m_bVehicleSlotState[i])
		{
			Delete(i);
		}
	}
}

//----------------------------------------------------

BOOL CVehicleManager::New(int vehicleID, BYTE byteVehicleType,
					    Vector3 * vecPos, float fRotation,
					    int iColor1, int iColor2,
					    Vector3 * vecSpawnPos, float fSpawnRotation )
{

	// Setup the spawninfo for the next respawn.
	m_SpawnInfo[vehicleID].byteVehicleType = byteVehicleType;
	m_SpawnInfo[vehicleID].vecPos.x = vecSpawnPos->x;
	m_SpawnInfo[vehicleID].vecPos.y = vecSpawnPos->y;
	m_SpawnInfo[vehicleID].vecPos.z = vecSpawnPos->z;
	m_SpawnInfo[vehicleID].fRotation = fSpawnRotation;
	m_SpawnInfo[vehicleID].iColor1 = iColor1;
	m_SpawnInfo[vehicleID].iColor2 = iColor2;

	// Now go ahead and spawn it at the location we got passed.
	return Spawn(vehicleID,byteVehicleType,vecPos,fRotation,iColor1,iColor2);
}

//----------------------------------------------------

BOOL CVehicleManager::Delete(int vehicleID)
{
	if(!GetSlotState(vehicleID) || !m_pVehicles[vehicleID]) {
		return FALSE;
	}

	m_bVehicleSlotState[vehicleID] = FALSE;
	delete m_pVehicles[vehicleID]; //That's one heinous act not really lol
	m_pVehicles[vehicleID] = NULL;

	return TRUE;
}

//----------------------------------------------------

BOOL CVehicleManager::Spawn(int vehicleID, int VehicleType,
					      Vector3 * vecPos, float fRotation,
					      int iColor1, int iColor2 )
{	
	if(m_pVehicles[vehicleID] != NULL) {
		Delete(vehicleID);
	}

	m_pVehicles[vehicleID] =  new CVehicle(VehicleType,
		vecPos->x,vecPos->y,vecPos->z,fRotation);

	if(m_pVehicles[vehicleID])
	{	
		if(iColor1 != (-1)) {
			//Write setcolor thing here inb4 no fucking pointer
			//m_pVehicles[vehicleID]->SetColor(iColor1,iColor2);
			VEHICLE::SET_VEHICLE_COLOURS((Vehicle)m_pVehicles[vehicleID]->pVehiclePtr, iColor1, iColor2);
		}

		m_bVehicleSlotState[vehicleID] = TRUE;

		m_bIsActive[vehicleID] = TRUE;
		m_bIsWasted[vehicleID] = FALSE;

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int CVehicleManager::FindGtaIDFromID(int vehicleID)
{
	if(m_pVehicles[vehicleID]) {
		
		//Gotta re-write something here, probably that has to do with natives? I don't know what this is for... TODO
		//return CPools::GetIndexFromVehicle(m_pVehicles[vehicleID]->GetVehicle());
	}

	// not too sure about this
	return 0;
}

//----------------------------------------------------

void CVehicleManager::SendVehicleDeath(int vehicleID)
{
	BitStream bsVehicleDeath;
	bsVehicleDeath.Write(vehicleID);
	pNetowkManager->GetRPC4()->Call("VehicleDeath", &bsVehicleDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, 
		UNASSIGNED_SYSTEM_ADDRESS, true);
}

//----------------------------------------------------

void DeleteVehicle(Entity VehicleHandle)
{
	if (ENTITY::DOES_ENTITY_EXIST(VehicleHandle))
	{
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY(VehicleHandle, 1, 1);
		//VEHICLE::DELETE_VEHICLE(&VehicleHandle);
		ENTITY::DELETE_ENTITY((Entity*)&VehicleHandle);
	}
}

void CVehicleManager::Process()
{
	CVehicle *pVehicle;
	DWORD dwThisTime = GetTickCount();
	//TODO: REWRITE INCOMING!!
	//#if 0
	//Now just parse the server vehicles
	for(int x = 0; x < MAX_VEHICLES; x++) {
		if(GetSlotState(x)) { // It's inuse.
			pVehicle = m_pVehicles[x];
			if(m_bIsActive[x]) {
				/*
				if((pVehicle->GetHealth() == 0.0f) || 
					(pVehicle->GetSubtype() != VEHICLE_SUBTYPE_BOAT &&
					pVehicle->GetSubtype() != VEHICLE_SUBTYPE_PLANE &&
					pVehicle->HasSunk())) { // It's dead or its not a boat and it has sunk
					if(!pVehicle->IsDead()) {
						SendVehicleDeath(x);
						pVehicle->SetDead(TRUE);
					}
				}
				else 
				{
					if(pVehicle->IsDriverLocalPlayer()) {
						pVehicle->SetInvulnerable(FALSE);
					} else {
						pVehicle->SetInvulnerable(TRUE);
					}

					if(pVehicle->GetSubtype() == VEHICLE_SUBTYPE_BIKE) {
						pVehicle->VerifyControlState();
					}		
				}
				*/
			}
		}
	}
//#endif
}
EntityId CVehicleManager::FindIDFromGtaPtr(Vehicle* pGtaVehicle)
{
	for (EntityId i = 0; i < MAX_VEHICLES; i++)
	{
		if (m_bVehicleSlotState[i])
		{
			if (pGtaVehicle == m_pVehicles[i]->pVehiclePtr)
			{
				return i;
			}
		}
	}
	return INVALID_ENTITY_ID;
}
bool CVehicleManager::vehInPool(Vehicle veh) {
	CVehicle *pVehicle;
	for (int x = 0; x < MAX_VEHICLES; x++) { //retarded loop
		if (GetSlotState(x)) { // It's inuse.
			pVehicle = m_pVehicles[x];
			if (m_bIsActive[x]) {
				if ((Vehicle)pVehicle->pVehiclePtr == veh) {
					return true;
				}
			}
		}
	}
	return false;
}
//----------------------------------------------------