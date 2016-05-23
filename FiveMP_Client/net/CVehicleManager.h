//---------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2004-2005 SA:MP team
//
// File Author(s): kyeman
// License: See LICENSE in root directory
//
//---------------------------------------------------
#ifndef CVEHICLEMANAGER_H
#define CVEHICLEMANAGER_H

#pragma once
#include "StdInc.h"
#include "CVehicle.h"

#define MAX_VEHICLES 2000

typedef struct _VEHICLE_SPAWN_INFO
{
	BYTE byteVehicleType;
	Vector3 vecPos;
	float fRotation;
	int iColor1;
	int iColor2;
} VEHICLE_SPAWN_INFO;

//----------------------------------------------------

class CVehicleManager
{
private:
	BOOL				m_bVehicleSlotState[MAX_VEHICLES];
	CVehicle			*m_pVehicles[MAX_VEHICLES];
	VEHICLE_SPAWN_INFO	m_SpawnInfo[MAX_VEHICLES];
	BOOL				m_bIsActive[MAX_VEHICLES];
	BOOL				m_bIsWasted[MAX_VEHICLES];

public:

	CVehicleManager();
	~CVehicleManager();

	BOOL New(int vehicleID, BYTE byteVehicleType,
		Vector3 * vecPos, float fRotation,
		int iColor1, int iColor2,
		Vector3 * vecSpawnPos, float fSpawnRotation);

	BOOL Delete(int vehicleID);
	CVehicle* GetAt(int vehicleID) {
		if (vehicleID > MAX_VEHICLES) { return NULL; }
		return m_pVehicles[vehicleID];
	};
	BOOL GetSlotState(int vehicleID) {
		if (vehicleID > MAX_VEHICLES) { return FALSE; }
		return m_bVehicleSlotState[vehicleID];
	};
	BOOL Spawn(int vehicleID, int VehicleType,
		Vector3 * vecPos, float fRotation,
		int iColor1, int iColor2);

	//EntityId FindIDFromGtaPtr(VEHICLE_TYPE * pGtaVehicle);
	int FindGtaIDFromID(int vehicleID);
	EntityId FindIDFromGtaPtr(Vehicle* pGtaVehicle);

	void SendVehicleDeath(int vehicleID);

	void Process();

	bool vehInPool(Vehicle veh);
};

//----------------------------------------------------
#endif