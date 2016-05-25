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
//#include "AimSync.h"

//-----------------------------------------------------------

#define MODEL_CELLPHONE 258

enum eObjectiveType
{
	OBJECTIVE_TYPE_KILL_1 = 8,
	OBJECTIVE_TYPE_KILL_2,
	OBJECTIVE_TYPE_AVOID,
	OBJECTIVE_TYPE_FOLLOW = 12,
	OBJECTIVE_TYPE_GO_TO,
	OBJECTIVE_TYPE_EXIT_CAR = 16,
	OBJECTIVE_TYPE_ENTER_CAR_AS_PASSENGER,
	OBJECTIVE_TYPE_ENTER_CAR_AS_DRIVER,
	OBJECTIVE_TYPE_FIRE_AT_TARGET = 21,
	OBJECTIVE_TYPE_RUN_AWAY,
	OBJECTIVE_TYPE_TIE = 29,
	OBJECTIVE_TYPE_AIM = 46
};

//-----------------------------------------------------------

class CPlayerPed
{
private:
	DWORD		m_dwGTAId;
	BYTE		m_bytePlayerNumber;
public:
	// Constructor/Destructor.	
	CPlayerPed();
	CPlayerPed(int iPlayerNumber, int iModel, float fPosX, float fPosY, float fPosZ, float fRotation = 0.0f);
	~CPlayerPed();
	void			Create(int iSkin, float fX, float fY, float fZ, float fRotation);
	void			Destroy();
	void			SetPosition(Vector3 vPos);
	Ped				*curPedPtr;
	CPlayerPed		*m_pInternalPlayer;
	DWORD			nextTask;
	DWORD			nextSprintAnim;
	Hash			relGroup;
#if 0
	//PED_TYPE *     GetPed();

	//void		  SetKeys(WORD wKeys);
	WORD		  GetKeys();
	bool		  IsFiring();
	/*
	CAMERA_AIM *  GetCurrentAim();
	void		  SetCurrentAim(CAMERA_AIM * pAim);
	*/
	void		  SetDead();

	void		  ShowMarker(int iMarkerColor);
	int			  GetCurrentVehicleID();
	void		  Say(UINT uiNum);
	void		  SetInitialState();
	BOOL		  IsOnScreen();

	void		  SetWaterDeaths(int iToggle);
	void		  SetCellAction(int iToggle);
	void		  Flash(int iItem);
	void		  SetActorBleeding(int iToggle);
	void		  SetSkyColor(int iColorID, int iFade);
	void		  SetDrivebyState(int iToggle);
	void		  SetCameraShakeIntensity(int iIntensity);

	float		  GetHealth();
	void		  SetHealth(float fHealth);
	void		  SetGameSpeed(float gSpeed);
	float		  GetArmour();
	void		  SetArmour(float fArmour);
	BOOL		  IsDead();
	BOOL		  IsInVehicle();
	BYTE		  GetAction();
	void		  SetAction(BYTE byteTrigger);
	BYTE		  GetShootingFlags();
	void		  SetShootingFlags(BYTE byteShooting);

	BOOL		  EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY);

	float		  GetRotation();
	void		  SetRotation(float fRotation);

	BOOL		  GiveWeapon(int iWeaponID, int iAmmo);
	void		  ClearAllWeapons();
	BOOL		  HasAmmoForCurrentWeapon();
	/*
	WEAPON_SLOT * GetCurrentWeaponSlot();
	WEAPON_SLOT * GetWeaponInSlot(BYTE byteSlot);
	*/
	void		  SetModel(int iSkin);
	void		  CheckAndRepairInvisProblems();
	void		  ClearTargetAndVehicle();

	void		  ClearLastWeaponDamage();
	void		  SetObjective(DWORD * pObjEntity, eObjectiveType objectiveType);
	void		  ClearObjective();
	void		  SetImmunities(BOOL bBullet, BOOL bFire, BOOL bExplosion, BOOL bDamage, BOOL bUnknown);

	void		  PutDirectlyInVehicle(int iVehicleID);
	void		  EnterVehicleAsDriver(int iVehicleID);
	void		  EnterVehicleAsPassenger(int iVehicleID);
	void		  ExitCurrentVehicle();
	void		  ForceIntoPassengerSeat(UINT uiVehicleID, UINT uiSeat);

	BOOL		  IsAPassenger();

	//VEHICLE_TYPE* GetGtaVehicle();

	void		  TogglePlayerControllable(int iControllable);
	BYTE		  FindDeathReasonAndResponsiblePlayer(EntityId * nPlayer);
	void		  RestartIfWastedAt(Vector3 *vecRestart, float fRotation);
	BYTE		  GetPassengerSeat();
#endif
	void		  RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);
	void		  GetPosition(Vector3 *position);
	Vector3		  GetRotation();
	int			GetPedSeat(Ped ped);
	bool		IsAPassenger();
	void		SetRotation(float pitch, float yaw, float roll) { ENTITY::SET_ENTITY_ROTATION((Entity)curPedPtr, pitch, roll, yaw, 2, true); }
	void		SetHealth(int health) { ENTITY::SET_ENTITY_HEALTH((Entity)curPedPtr, health); }
	void		SetArmour(int amount) { PED::SET_PED_ARMOUR((Ped)curPedPtr, amount); }
	void		GetQuaternion(float * quat);
	void		SetQuaternion(float * quat);
	void		SetTickNextAction(DWORD tickCount) { nextTask = tickCount; }
	DWORD		GetTickNextAction() { return nextTask; }
	Vector3		GetForwardVector() { return ENTITY::GET_ENTITY_FORWARD_VECTOR((Entity)curPedPtr); }
	Vector3		GetCoordsFromVecFor(float dist);
	Vector3		GetVelocity() { return ENTITY::GET_ENTITY_VELOCITY((Entity)curPedPtr); }
	Vector3		GetCoordsFromVecForUsingVec(Vector3 m_VecFor, float dist);
	bool		IsPedJumping() { return PED::IS_PED_JUMPING((Ped)curPedPtr); }
	void		MakePedJump() { AI::TASK_JUMP((Ped)curPedPtr, true); }
	BOOL		SetArmedWeapon(UINT32 weaponHash);
	UINT32		GetCurrentWeapon() { return WEAPON::GET_SELECTED_PED_WEAPON((Ped)curPedPtr); };
	Vector3		GetPedSpeed() { return ENTITY::GET_ENTITY_VELOCITY((Entity)curPedPtr); }
	float		GetSpeedFloat() { return roundf(sqrtf(pow(fabs(GetPedSpeed().x), 2.0) + pow(fabs(GetPedSpeed().y), 2.0) + pow(fabs(GetPedSpeed().z), 2.0))); }
	void		SetPedSpeed(Entity ent, Vector3 Speed);
	int			GetMoveState();
	void		SetTickNextSprint(DWORD tickCount) { nextSprintAnim = tickCount; }
	DWORD		GetTickNextSprint() { return nextSprintAnim; }
	void		ChooseSprintingAnim(Ped ped);
};

extern char* pedModels[690];
extern CPlayerPed* FindPlayerPed();

//-----------------------------------------------------------