#ifndef CREMOTEPLAYER_H
#define CREMOTEPLAYER_H

#pragma once
#include "CPlayerPed.h"
#include "system.h"

#define UPDATE_TYPE_NONE		0
#define UPDATE_TYPE_FULL_ONFOOT	1
#define UPDATE_TYPE_MINI_ONFOOT 2
#define UPDATE_TYPE_FULL_INCAR	3
#define UPDATE_TYPE_MINI_INCAR  4
#define UPDATE_TYPE_PASSENGER	5

#define VALID_KILL		  1
#define TEAM_KILL		  2
#define SELF_KILL		  3

#define NO_TEAM			  255

//----------------------------------------------------

class CRemotePlayer
{
private:
	CPlayerPed			*m_pPlayerPed;

	EntityId			m_playerID;
	BYTE				m_byteUpdateFromNetwork;
	BOOL				m_bIsActive;
	BOOL				m_bIsWasted;
	BYTE				m_byteTeam;

	// Information that is synced.
	//WORD				m_wKeys;
	MATRIX2X4			m_matWorld;

	Vector3				m_vecMoveSpeed;
	Vector3				m_vecTurnSpeed;
	//float				m_fRotation;
	struct
	{
		float quat[4];
	} matrix;

	float				m_fVehicleHealth;
	BYTE				m_byteHealth; // health float casted to a byte.
	BYTE				m_byteArmour; // armour float casted to a byte.
	BYTE				m_byteCurrentWeapon;

	BOOL				m_bIsInVehicle;
	BOOL				m_bIsAPassenger;
	BYTE				m_vehicleID;
	BYTE				m_bytePassengerSeat;
	BYTE				m_byteShootingFlags;

	CHAR				m_szPlayerName[MAX_PLAYER_NAME];

	BYTE				m_iJustSpawned; // Indicates that we shouldn't process
										// this player straight away.

	bool				m_bHasAim;
	Vector3				m_vecForward;
	Vector3				m_vecAimPos;
	bool				m_bShooting;
	UINT32				m_curWeaponHash;
	Vector3				m_speed;
	int					m_moveState;
	Vector3				m_playerRot;
	//CAMERA_AIM			m_Aim;

public:
	CRemotePlayer();
	~CRemotePlayer();

	BOOL IsActive() { return m_bIsActive; };
	BOOL IsWasted() { return m_bIsWasted; };

	void Process();
	void HandleVehicleEntryExit();
	void Say(PCHAR szText);

	void SetID(EntityId playerID) { m_playerID = playerID; };
	EntityId GetID() { return m_playerID; };
	BYTE GetTeam() { return m_byteTeam; };	
	UINT32 GetCurrentWeapon() { return m_curWeaponHash; };

	CPlayerPed * GetPlayerPed() { return m_pPlayerPed; };

	void SetReportedHealth(BYTE byteReportHealth) { 
		m_byteHealth = byteReportHealth;
	}
	void SetReportedArmour(BYTE byteReportArmour) { 
		m_byteArmour = byteReportArmour;
	}

	float GetReportedHealth() { return (float)m_byteHealth; };

	float GetReportedArmour() { return (float)m_byteArmour; };

	void UpdateOnFootPosition(Vector3 vPos);

	void UpdateOnAimPosition(Vector3 vPos);

	void UpdateInCarMatrixAndSpeed(MATRIX2X4 * matWorld, Vector3 * vecMoveSpeed, Vector3 * vecTurnSpeed);

	void StoreOnFootFullSyncData(PLAYER_SYNC_DATA * pPlayerSyncData);

#if 0
	void StoreAimSyncData(CAMERA_AIM * pAim);
#endif

	void StoreInCarFullSyncData(VEHICLE_SYNC_DATA * pVehicleSyncData);

	void StorePassengerData(EntityId vehicleID, BYTE byteSeat);

	BOOL DestroyPlayer();

	BOOL SpawnPlayer( BYTE byteTeam, BYTE byteSkin, 
					  Vector3 * vecPos, float fRotation, int iSpawnWeapon1,
					  int iSpawnWeapon1Ammo, int iSpawnWeapon2, 
					  int iSpawnWeapon2Ammo, int iSpawnWeapon3,
					  int iSpawnWeapon3Ammo );

	void HandleDeath(BYTE byteReason, BYTE byteWhoKilled, BYTE byteScoringModifier);

	float GetDistanceFromRemotePlayer(CRemotePlayer *pFromPlayer);
	float GetDistanceFromLocalPlayer();

	DWORD GetTeamColorAsRGBA();
	DWORD GetTeamColorAsARGB();

	void InflictDamage(bool bPlayerVehicleDamager, EntityId damagerID, int iWeapon, float fUnk, int iPedPieces, BYTE byteUnk);
	void RemotePlayerDecideShoot(Vector3 vPos, Vector3 Position, float dist, DWORD timeTakes);
	float GetSpeedFloat() { return roundf(sqrtf(pow(fabs(m_speed.x), 2.0) + pow(fabs(m_speed.y), 2.0) + pow(fabs(m_speed.z), 2.0))); }
};

//----------------------------------------------------
#endif