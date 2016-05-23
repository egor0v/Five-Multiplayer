//----------------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2004-2005 SA:MP team
//
// File Author(s): kyeman
// License: See LICENSE in root directory
//
//----------------------------------------------------------
#ifndef CLOCALPLAYER_H
#define CLOCALPLAYER_H

#include "modbaserequired.h"
#include "CPlayerPed.h"

typedef struct _PLAYER_SPAWN_INFO
{
	BYTE byteTeam;
	BYTE byteSkin;
	Vector3 vecPos;
	float fRotation;
	int iSpawnWeapons[3];
	int iSpawnWeaponsAmmo[3];
	bool bLoaded;
} PLAYER_SPAWN_INFO;

typedef struct _PLAYER_SYNC_DATA
{
	//WORD wKeys;
	Vector3 vecPos;
	//float fRotation;
	/*
	struct
	{
	float quat[4];
	} matrix;
	*/
	BYTE byteCurrentWeapon;
	BYTE byteShootingFlags;
	BYTE byteHealth;
	BYTE byteArmour;
	Vector3 m_vecForward;
	//must reenable
	bool m_bHasAim;
	bool m_bShooting;
	Vector3 m_vecAimPos;
	UINT32 m_curWeaponHash;
	Vector3 m_speed;
	int m_moveState;
	Vector3 m_playerRot;
} PLAYER_SYNC_DATA;

typedef struct _VEHICLE_SYNC_DATA
{
	EntityId vehicleID;
	//WORD wKeys;
	//Matrix vecRoll;
	struct
	{
		float quat[4];
	} matrix;
	//float vecDirection;
	// No more compression
	//C_VECTOR1 cvecRoll;
	//C_VECTOR1 cvecDirection;
	Vector3 vecPos;
	Vector3 vecMoveSpeed;
	Vector3 vecTurnSpeed;
	BYTE byteVehicleHealth;
	BYTE bytePlayerHealth;
	BYTE bytePlayerArmour;
} VEHICLE_SYNC_DATA;

#define NO_TEAM 255

//----------------------------------------------------------
class CLocalPlayer {
private:
	CPlayerPed				*m_pPlayerPed;
	PLAYER_SPAWN_INFO		m_SpawnInfo;
	BOOL					m_bHasSpawnInfo;
	BOOL					m_bIsActive;
	BOOL					m_bIsWasted;
	CHAR					m_szPlayerName[256];
	WORD					m_wLastKeys;
	DWORD					m_dwLastSendTick;
	DWORD					m_dwLastPassengerSendTick;
	int						m_iPause;

public:

	CLocalPlayer();
	~CLocalPlayer(){};

	BOOL IsActive() { return m_bIsActive; };
	BOOL IsWasted() { return m_bIsWasted; };
	BOOL HasSpawnInfo() { return m_bHasSpawnInfo; };

	BOOL Process();
	BOOL DestroyPlayer();

	Vector3 Player_ShootLine(Vector3 m_vecPos, float dist);

	void SetIsActive(BOOL Active) { m_bIsActive = Active; };

	void SendOnFootFullSyncData();
	void SendInCarFullSyncData();
	void SendInCarPassengerData();
	int  GetOptimumInCarSendRate();
	int  GetOptimumOnFootSendRate();

	BYTE GetTeam() { 
		if(m_bHasSpawnInfo) return m_SpawnInfo.byteTeam;
		else return NO_TEAM;
	};
	
	void SendWastedNotification();
	
	void RequestClass(int iClass);

	void SetSpawnInfo(PLAYER_SPAWN_INFO * pSpawnInfo);

	BOOL SpawnPlayer(PLAYER_SPAWN_INFO * pSpawnInfo);

	BOOL SpawnPlayer();

	CPlayerPed * GetPlayerPed() { return m_pPlayerPed; };
	CPlayerPed SetPlayerPed(CPlayerPed *ptr) { m_pPlayerPed = ptr; }

	void Say(PCHAR szText);

	void SendEnterVehicleNotification(EntityId vehicleID, bool bPassenger);
	void SendExitVehicleNotification(EntityId vehicleID);	
	
	DWORD GetTeamColorAsRGBA();
	DWORD GetTeamColorAsARGB();

	void CLocalPlayer::SendInflictedDamageNotification(EntityId playerID, EntityId vehicleID, int iWeapon, float fUnk, int iPedPieces, BYTE byteUnk);

	void SendPauseNotification(int bPause);
};


//----------------------------------------------------------
#endif