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
#include "modbaserequired.h"

extern CChatWindow   *pChatWindow;
//extern CCmdWindow	 *pCmdWindow;
//extern CPlayerPed	 *pLocalPlayer;
//extern CScripts		 *pScripts;

using namespace RakNet;
extern CNetworkManager* pNetowkManager;

#define REJECT_REASON_BAD_VERSION   1
#define REJECT_REASON_BAD_NICKNAME  2

//----------------------------------------------------
// Sent when a client joins the server we're
// currently connected to.

void ServerJoin(RakNet::BitStream *bitStream, Packet *packet)
{
	CPlayerManager *pPlayerManager = pNetowkManager->GetPlayerManager();
	CHAR szPlayerName[MAX_PLAYER_NAME];
	EntityId playerID;
	UINT uiNameLength;

	memset(szPlayerName,0,MAX_PLAYER_NAME);

	bitStream->Read(playerID);
	bitStream->Read(uiNameLength);
	bitStream->Read(szPlayerName,uiNameLength);
	szPlayerName[uiNameLength] = '\0';

	// Add this client to the player pool.
	pPlayerManager->New(playerID, szPlayerName);
}

//----------------------------------------------------
// Sent when a client joins the server we're
// currently connected to.

void ServerQuit(RakNet::BitStream *bitStream, Packet *packet)
{
	CPlayerManager *pPlayerManager = pNetowkManager->GetPlayerManager();
	EntityId playerID;
	BYTE byteReason;

	bitStream->Read(playerID);
	bitStream->Read(byteReason);

	// Delete this client from the player pool.
	pPlayerManager->Delete(playerID,byteReason);
}


//----------------------------------------------------
// Server is giving us basic init information.

void InitGame(RakNet::BitStream *bitStream, Packet *packet)
{
	CPlayerManager *pPlayerManager = pNetowkManager->GetPlayerManager();
	EntityId myPlayerID;

	bitStream->Read((char *)&pNetowkManager->m_vecInitPlayerPos, sizeof(Vector3));
	bitStream->Read((char *)&pNetowkManager->m_vecInitCameraPos, sizeof(Vector3));
	bitStream->Read((char *)&pNetowkManager->m_vecInitCameraLook, sizeof(Vector3));
	bitStream->Read(pNetowkManager->m_WorldBounds[0]);
	bitStream->Read(pNetowkManager->m_WorldBounds[1]);
	bitStream->Read(pNetowkManager->m_WorldBounds[2]);
	bitStream->Read(pNetowkManager->m_WorldBounds[3]);
	bitStream->Read(pNetowkManager->m_byteFriendlyFire);
	bitStream->Read(pNetowkManager->m_byteShowOnRadar);
	bitStream->Read(myPlayerID);

	pPlayerManager->SetLocalPlayerID(myPlayerID);

	//pGame->FadeScreen(1, 0);
	pNetowkManager->InitGameLogic();
	pNetowkManager->SetGameState(GAMESTATE_CONNECTED);
}

// send message to client
void Script_ClientMessage(RakNet::BitStream *bitStream, Packet *packet)
{
	DWORD dwColor;
	CHAR szMessage[256];
	UINT uiLength;

	bitStream->Read(dwColor);
	bitStream->Read(uiLength);
	bitStream->Read(szMessage, uiLength);
	szMessage[uiLength] = '\0';

	pChatWindow->AddChatMessage("", dwColor, szMessage);
}

//----------------------------------------------------
// Remote player has sent a chat message.

void Chat(RakNet::BitStream *bitStream, Packet *packet)
{
	EntityId playerID;
	BYTE byteTextLen;
	CHAR szText[256];

	bitStream->Read(playerID);
	bitStream->Read(byteTextLen);
	bitStream->Read(szText,byteTextLen);

	szText[byteTextLen] = '\0';

	CPlayerManager * pPlayerManager = pNetowkManager->GetPlayerManager();
	if(playerID == pPlayerManager->GetLocalPlayerID()) {
		pChatWindow->AddChatMessage(pNetowkManager->GetPlayerManager()->GetLocalPlayerName(),
			pPlayerManager->GetLocalPlayer()->GetTeamColorAsARGB(),szText);
	}
	else
	{
		CRemotePlayer *pRemotePlayer = pNetowkManager->GetPlayerManager()->GetAt(playerID);
		if(pRemotePlayer) {
			pRemotePlayer->Say(szText);	
		}
	}
}

//----------------------------------------------------
// This should be rewritten as a packet instead of
// an RPC.

void Passenger(RakNet::BitStream *bitStream, Packet *packet)
{
	EntityId playerID;
	EntityId vehicleID;
	BYTE byteSeat;

	bitStream->Read(playerID);
	bitStream->Read(vehicleID);
	bitStream->Read(byteSeat);
	
	CRemotePlayer * pRemotePlayer = pNetowkManager->GetPlayerManager()->GetAt(playerID);

	if(pRemotePlayer) {
		pRemotePlayer->StorePassengerData(vehicleID, byteSeat);
	}	
}

//----------------------------------------------------
// Reply to our class request from the server.

void RequestClass(RakNet::BitStream *bitStream, Packet *packet)
{
	BYTE byteOutcome;
	int iRequestedClass;
	PLAYER_SPAWN_INFO SpawnInfo;
	CLocalPlayer * pPlayer = pNetowkManager->GetPlayerManager()->GetLocalPlayer();
	CSpawnSelection * pGameLogic = pNetowkManager->GetGameLogic();

	bitStream->Read(byteOutcome);
	if(byteOutcome) {
		bitStream->Read(iRequestedClass);
		bitStream->Read((char *)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

		pGameLogic->SetSelectedClass(iRequestedClass);
		pPlayer->SetSpawnInfo(&SpawnInfo);
		pGameLogic->HandleClassSelectionOutcome(&SpawnInfo, byteOutcome);
	}
}

//----------------------------------------------------
// Remote client is spawning.

void Spawn(RakNet::BitStream *bitStream, Packet *packet)
{
	CRemotePlayer *pRemotePlayer;

	EntityId playerID=0;
	BYTE byteTeam=0;
	BYTE byteSkin=0;
	Vector3 vecPos;
	float fRotation=0;
	BYTE byteOnGround=0;
	int iSpawnWeapons1,iSpawnWeapons2,iSpawnWeapons3;
	int iSpawnWeaponsAmmo1,iSpawnWeaponsAmmo2,iSpawnWeaponsAmmo3;

	bitStream->Read(playerID);
	bitStream->Read(byteTeam);
	bitStream->Read(byteSkin);
	bitStream->Read(vecPos.x);
	bitStream->Read(vecPos.y);
	bitStream->Read(vecPos.z);
	bitStream->Read(fRotation);
	bitStream->Read(iSpawnWeapons1);
	bitStream->Read(iSpawnWeaponsAmmo1);
	bitStream->Read(iSpawnWeapons2);
	bitStream->Read(iSpawnWeaponsAmmo2);
	bitStream->Read(iSpawnWeapons3);
	bitStream->Read(iSpawnWeaponsAmmo3);

	pRemotePlayer = pNetowkManager->GetPlayerManager()->GetAt(playerID);

	if(pRemotePlayer) {
		pRemotePlayer->SpawnPlayer(byteTeam,byteSkin,&vecPos,fRotation,
			iSpawnWeapons1,iSpawnWeaponsAmmo1,
			iSpawnWeapons2,iSpawnWeaponsAmmo2,
			iSpawnWeapons3,iSpawnWeaponsAmmo3);
	}		
}

//----------------------------------------------------
// Remote client is dead.

void Death(RakNet::BitStream *bitStream, Packet *packet)
{
	EntityId playerID=0;
	BYTE byteReason;
	BYTE byteWhoKilled;
	BYTE byteScoringModifier;

	bitStream->Read(playerID);
	bitStream->Read(byteReason);
	bitStream->Read(byteWhoKilled);
	bitStream->Read(byteScoringModifier);

	CRemotePlayer *pRemotePlayer = pNetowkManager->GetPlayerManager()->GetAt(playerID);
	if(pRemotePlayer) {
		pRemotePlayer->HandleDeath(byteReason,byteWhoKilled,byteScoringModifier);
	}
}

//----------------------------------------------------
// Remote client is trying to enter vehicle gracefully.

void EnterVehicle(RakNet::BitStream *bitStream, Packet *packet)
{
	EntityId playerID=0;
	EntityId vehicleID=0;
	BYTE bytePassenger=0;

	bitStream->Read(playerID);
	bitStream->Read(vehicleID);
	bitStream->Read(bytePassenger);

	CRemotePlayer *pRemotePlayer = pNetowkManager->GetPlayerManager()->GetAt(playerID);
	CVehicleManager *pVehicleManager = pNetowkManager->GetVehicleManager();
#if 0 //TODO: ADD FUNCTIONALITY
	if(pRemotePlayer) {
		if(!bytePassenger) {
			pRemotePlayer->GetPlayerPed()->EnterVehicleAsDriver(pVehicleManager->FindGtaIDFromID(vehicleID));
		} else {
			pRemotePlayer->GetPlayerPed()->EnterVehicleAsPassenger(pVehicleManager->FindGtaIDFromID(vehicleID));
		}
	}
#endif
}

//----------------------------------------------------
// Remote client is trying to enter vehicle gracefully.

void ExitVehicle(RakNet::BitStream *bitStream, Packet *packet)
{
	EntityId playerID=0;
	EntityId vehicleID=0;

	bitStream->Read(playerID);
	bitStream->Read(vehicleID);

	CRemotePlayer *pRemotePlayer = pNetowkManager->GetPlayerManager()->GetAt(playerID);
	CVehicleManager *pVehicleManager = pNetowkManager->GetVehicleManager();

	if(pRemotePlayer) {
#if 0 //TODO: ADD FUNCTIONALITY
		pRemotePlayer->GetPlayerPed()->ExitCurrentVehicle();
#endif
	}
}

//----------------------------------------------------

void VehicleSpawn(RakNet::BitStream *bitStream, Packet *packet)
{
	CVehicleManager *pVehicleManager = pNetowkManager->GetVehicleManager();
	EntityId vehicleID=0;
	BYTE byteVehicleType;
	Vector3 vecPos;
	Vector3 vecSpawnPos;
	float fRotation;
	float fSpawnRotation;
	float fHealth;
	int iColor1, iColor2;

	bitStream->Read(vehicleID);
	bitStream->Read(byteVehicleType);
	bitStream->Read(vecPos.x);
	bitStream->Read(vecPos.y);
	bitStream->Read(vecPos.z);
	bitStream->Read(fRotation);
	bitStream->Read(iColor1);
	bitStream->Read(iColor2);
	bitStream->Read(fHealth);
	bitStream->Read(vecSpawnPos.x);
	bitStream->Read(vecSpawnPos.y);
	bitStream->Read(vecSpawnPos.z);
	bitStream->Read(fSpawnRotation);

	pVehicleManager->New(vehicleID,byteVehicleType,
		&vecPos,fRotation,iColor1,iColor2,&vecSpawnPos,fSpawnRotation);
	//pVehicleManager->GetAt(vehicleID)->SetHealth(fHealth);
}

//----------------------------------------------------

void VehicleDestroy(RakNet::BitStream *bitStream, Packet *packet)
{
	CVehicleManager *pVehicleManager = pNetowkManager->GetVehicleManager();
	EntityId vehicleID=0;

	bitStream->Read(vehicleID);

	pVehicleManager->Delete(vehicleID);
}

//----------------------------------------------------

void UpdateScoreAndPing(RakNet::BitStream *bitStream, Packet *packet)
{	
	CPlayerManager * pPlayerManager = pNetowkManager->GetPlayerManager();
	EntityId playerID;
	int iPlayerScore;
	int iPlayerPing;
	unsigned long ulIp;

	while(bitStream->Read(playerID))
	{
		bitStream->Read(iPlayerScore);
		bitStream->Read(iPlayerPing);
		bitStream->Read(ulIp);

		if(pPlayerManager->GetSlotState(playerID) || playerID == pPlayerManager->GetLocalPlayerID())
		{
			pPlayerManager->UpdateScore(playerID, iPlayerScore);
			pPlayerManager->UpdatePing(playerID, iPlayerPing);
			pPlayerManager->UpdateIPAddress(playerID, ulIp);
		}
	}
}

//----------------------------------------------------

void ConnectionRejected(RakNet::BitStream *bitStream, Packet *packet)
{
	BYTE byteRejectReason;

	bitStream->Read(byteRejectReason);

	if(byteRejectReason==REJECT_REASON_BAD_VERSION) {
		pChatWindow->AddChatMessage("", 0, "CONNECTION REJECTED");
		pChatWindow->AddChatMessage("", 0, "YOU'RE USING AN INCORRECT VERSION!");
	}
	else if(byteRejectReason==REJECT_REASON_BAD_NICKNAME)
	{
		pChatWindow->AddChatMessage("", 0, "CONNECTION REJECTED");
		pChatWindow->AddChatMessage("", 0, "YOUR NICKNAME IS INVALID");
	}

	if(pNetowkManager) {
		pNetowkManager->Shutdown();
	}
}

void RegisterRPCs()
{
	pNetowkManager->GetRPC4()->RegisterFunction("ServerJoin",ServerJoin);
	pNetowkManager->GetRPC4()->RegisterFunction("ServerQuit",ServerQuit);	
	pNetowkManager->GetRPC4()->RegisterFunction("InitGame",InitGame);
	pNetowkManager->GetRPC4()->RegisterFunction("Chat",Chat);
	pNetowkManager->GetRPC4()->RegisterFunction("RequestClass",RequestClass);
	pNetowkManager->GetRPC4()->RegisterFunction("Spawn",Spawn);
	pNetowkManager->GetRPC4()->RegisterFunction("Death",Death);
	pNetowkManager->GetRPC4()->RegisterFunction("EnterVehicle",EnterVehicle);
	pNetowkManager->GetRPC4()->RegisterFunction("ExitVehicle",ExitVehicle);
	pNetowkManager->GetRPC4()->RegisterFunction("VehicleSpawn",VehicleSpawn);
	pNetowkManager->GetRPC4()->RegisterFunction("VehicleDestroy",VehicleDestroy);
	pNetowkManager->GetRPC4()->RegisterFunction("UpdateScoreAndPing",UpdateScoreAndPing);
	pNetowkManager->GetRPC4()->RegisterFunction("ConnectionRejected",ConnectionRejected);
	pNetowkManager->GetRPC4()->RegisterFunction("Script_ClientMessage", Script_ClientMessage);
}

//----------------------------------------------------

void UnRegisterRPCs()
{
	pNetowkManager->GetRPC4()->UnregisterFunction("ServerJoin");
	pNetowkManager->GetRPC4()->UnregisterFunction("ServerQuit");
	pNetowkManager->GetRPC4()->UnregisterFunction("InitGame");
	pNetowkManager->GetRPC4()->UnregisterFunction("Chat");
	pNetowkManager->GetRPC4()->UnregisterFunction("RequestClass");
	pNetowkManager->GetRPC4()->UnregisterFunction("Spawn");
	pNetowkManager->GetRPC4()->UnregisterFunction("Death");
	pNetowkManager->GetRPC4()->UnregisterFunction("EnterVehicle");
	pNetowkManager->GetRPC4()->UnregisterFunction("ExitVehicle");
	pNetowkManager->GetRPC4()->UnregisterFunction("VehicleSpawn");
	pNetowkManager->GetRPC4()->UnregisterFunction("VehicleDestroy");
	pNetowkManager->GetRPC4()->UnregisterFunction("UpdateScoreAndPing");
	pNetowkManager->GetRPC4()->UnregisterFunction("ConnectionRejected");
	pNetowkManager->GetRPC4()->UnregisterFunction("Script_ClientMessage");
}

//----------------------------------------------------
