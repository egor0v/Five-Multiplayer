//----------------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2004-2005 SA:MP team
//
// File Author(s): kyeman
// License: See LICENSE in root directory
//
//----------------------------------------------------------

#pragma once

#include <stdint.h>

//-----------------------------------------------------------

class CVehicle
{
private:
	DWORD m_dwGTAId;
	BOOL  m_bIsInvulnerable;
	BOOL  m_bDead;

public:
	Vehicle *pVehiclePtr;
	CVehicle(int vehicleModel, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f);
	~CVehicle();
	void	SetQuaternion(float * quat);
	void	GetQuaternion(float * quat);
	void	SetHealth(float amount) { VEHICLE::SET_VEHICLE_BODY_HEALTH((Vehicle)pVehiclePtr, amount); }
	void	SetMoveSpeed(Vector3 speed) { ENTITY::SET_ENTITY_VELOCITY((Entity)pVehiclePtr, speed.x, speed.y, speed.z); }
	void	SetTurnSpeed(Vector3 turnspeed) { ENTITY::APPLY_FORCE_TO_ENTITY((Entity)pVehiclePtr, 3, turnspeed.x, turnspeed.y, turnspeed.z, 0.0f, 0.0f, 0.0f, true, false, true, true, true, true); }
	void	SetPos(Vector3 pos) { ENTITY::SET_ENTITY_COORDS((Entity)pVehiclePtr, pos.x, pos.y, pos.z, true, true, true, false); }
#if 0
	VEHICLE_TYPE * GetVehicle();

	float          GetDistanceFromLocalPlayerPed();
	void           SetLockedState(int iLocked);

	BYTE           GetSubtype();

	float          GetHealth();
	void           SetHealth(float fHealth);
	void           SetColor(int iColor1, int iColor2);
	void           VerifyControlState();

	BOOL           HasSunk();
	BOOL           IsDriverLocalPlayer();
	void           EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY);
	BOOL           HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY);

	BOOL           GetHandbrakeState();
	void           SetHandbrakeState(BOOL bState);
	BYTE           GetMaxPassengers();

	BYTE           GetEntityFlags();
	void           SetEnitityFlags(BYTE byteEntityFlags);

	void           SetInvulnerable(BOOL bInv);
	BOOL           IsInvulnerable() { return m_bIsInvulnerable; };

	PED_TYPE *     GetDriver();

	void           SetImmunities(int iIm1, int iIm2, int iIm3, int iIm4, int iIm5);

	void           SetDead(BOOL bDead);
	BOOL           IsDead();

	void           PopTrunk();
	void           ToggleRhinoInstantExplosionUponContact(int iToggle);
#endif
};

extern char* vehModels[350];

//-----------------------------------------------------------
