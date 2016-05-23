//----------------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2004-2005 SA:MP team
//
// File Author(s): kyeman
//                 jenksta
// License: See LICENSE in root directory
//
//----------------------------------------------------------
// TODO: Create/Destroy/SetModelIndex functions

#include "stdafx.h"
#include "StdInc.h"
#include "CVehicle.h"

//extern CGame		*pGame;
extern CChatWindow  *pChatWindow;
char* vehModels[350] = {
	"NINEF", "NINEF2", "BLISTA", "ASEA", "ASEA2", "BOATTRAILER", "BUS", "ARMYTANKER", "ARMYTRAILER", "ARMYTRAILER2",
	"SUNTRAP", "COACH", "AIRBUS", "ASTEROPE", "AIRTUG", "AMBULANCE", "BARRACKS", "BARRACKS2", "BALLER", "BALLER2",
	"BJXL", "BANSHEE", "BENSON", "BFINJECTION", "BIFF", "BLAZER", "BLAZER2", "BLAZER3", "BISON", "BISON2",
	"BISON3", "BOXVILLE", "BOXVILLE2", "BOXVILLE3", "BOBCATXL", "BODHI2", "BUCCANEER", "BUFFALO", "BUFFALO2", "BULLDOZER",
	"BULLET", "BLIMP", "BURRITO", "BURRITO2", "BURRITO3", "BURRITO4", "BURRITO5", "CAVALCADE", "CAVALCADE2", "POLICET",
	"GBURRITO", "CABLECAR", "CADDY", "CADDY2", "CAMPER", "CARBONIZZARE", "CHEETAH", "COMET2", "COGCABRIO", "COQUETTE",
	"CUTTER", "GRESLEY", "DILETTANTE", "DILETTANTE2", "DUNE", "DUNE2", "HOTKNIFE", "DLOADER", "DUBSTA", "DUBSTA2",
	"DUMP", "RUBBLE", "DOCKTUG", "DOMINATOR", "EMPEROR", "EMPEROR2", "EMPEROR3", "ENTITYXF", "EXEMPLAR", "ELEGY2",
	"F620", "FBI", "FBI2", "FELON", "FELON2", "FELTZER2", "FIRETRUK", "FLATBED", "FORKLIFT", "FQ2",
	"FUSILADE", "FUGITIVE", "FUTO", "GRANGER", "GAUNTLET", "HABANERO", "HAULER", "HANDLER", "INFERNUS", "INGOT",
	"INTRUDER", "ISSI2", "JACKAL", "JOURNEY", "JB700", "KHAMELION", "LANDSTALKER", "LGUARD", "MANANA", "MESA",
	"MESA2", "MESA3", "CRUSADER", "MINIVAN", "MIXER", "MIXER2", "MONROE", "MOWER", "MULE", "MULE2",
	"ORACLE", "ORACLE2", "PACKER", "PATRIOT", "PBUS", "PENUMBRA", "PEYOTE", "PHANTOM", "PHOENIX", "PICADOR",
	"POUNDER", "POLICE", "POLICE4", "POLICE2", "POLICE3", "POLICEOLD1", "POLICEOLD2", "PONY", "PONY2", "PRAIRIE",
	"PRANGER", "PREMIER", "PRIMO", "PROPTRAILER", "RANCHERXL", "RANCHERXL2", "RAPIDGT", "RAPIDGT2", "RADI", "RATLOADER",
	"REBEL", "REGINA", "REBEL2", "RENTALBUS", "RUINER", "RUMPO", "RUMPO2", "RHINO", "RIOT", "RIPLEY",
	"ROCOTO", "ROMERO", "SABREGT", "SADLER", "SADLER2", "SANDKING", "SANDKING2", "SCHAFTER2", "SCHWARZER", "SCRAP",
	"SEMINOLE", "SENTINEL", "SENTINEL2", "ZION", "ZION2", "SERRANO", "SHERIFF", "SHERIFF2", "SPEEDO", "SPEEDO2",
	"STANIER", "STINGER", "STINGERGT", "STOCKADE", "STOCKADE3", "STRATUM", "SULTAN", "SUPERD", "SURANO", "SURFER",
	"SURFER2", "SURGE", "TACO", "TAILGATER", "TAXI", "TRASH", "TRACTOR", "TRACTOR2", "TRACTOR3", "GRAINTRAILER",
	"BALETRAILER", "TIPTRUCK", "TIPTRUCK2", "TORNADO", "TORNADO2", "TORNADO3", "TORNADO4", "TOURBUS", "TOWTRUCK", "TOWTRUCK2",
	"UTILLITRUCK", "UTILLITRUCK2", "UTILLITRUCK3", "VOODOO2", "WASHINGTON", "STRETCH", "YOUGA", "ZTYPE", "SANCHEZ", "SANCHEZ2",
	"SCORCHER", "TRIBIKE", "TRIBIKE2", "TRIBIKE3", "FIXTER", "CRUISER", "BMX", "POLICEB", "AKUMA", "CARBONRS",
	"BAGGER", "BATI", "BATI2", "RUFFIAN", "DAEMON", "DOUBLE", "PCJ", "VADER", "VIGERO", "FAGGIO2",
	"HEXER", "ANNIHILATOR", "BUZZARD", "BUZZARD2", "CARGOBOB", "CARGOBOB2", "CARGOBOB3", "SKYLIFT", "POLMAV", "MAVERICK",
	"NEMESIS", "FROGGER", "FROGGER2", "CUBAN800", "DUSTER", "STUNT", "MAMMATUS", "JET", "SHAMAL", "LUXOR",
	"TITAN", "LAZER", "CARGOPLANE", "SQUALO", "MARQUIS", "DINGHY", "DINGHY2", "JETMAX", "PREDATOR", "TROPIC",
	"SEASHARK", "SEASHARK2", "SUBMERSIBLE", "TRAILERS", "TRAILERS2", "TRAILERS3", "TVTRAILER", "RAKETRAILER", "TANKER", "TRAILERLOGS",
	"TR2", "TR3", "TR4", "TRFLAT", "TRAILERSMALL", "VELUM", "ADDER", "VOLTIC", "VACCA", "BIFTA",
	"SPEEDER", "PARADISE", "KALAHARI", "JESTER", "TURISMOR", "VESTRA", "ALPHA", "HUNTLEY", "THRUST", "MASSACRO",
	"MASSACRO2", "ZENTORNO", "BLADE", "GLENDALE", "PANTO", "PIGALLE", "WARRENER", "RHAPSODY", "DUBSTA3", "MONSTER",
	"SOVEREIGN", "INNOVATION", "HAKUCHOU", "FUROREGT", "MILJET", "COQUETTE2", "BTYPE", "BUFFALO3", "DOMINATOR2", "GAUNTLET2",
	"MARSHALL", "DUKES", "DUKES2", "STALION", "STALION2", "BLISTA2", "BLISTA3", "DODO", "SUBMERSIBLE2", "HYDRA",
	"INSURGENT", "INSURGENT2", "TECHNICAL", "SAVAGE", "VALKYRIE", "KURUMA", "KURUMA2", "JESTER2", "CASCO", "VELUM2",
	"GUARDIAN", "ENDURO", "LECTRO", "SLAMVAN", "SLAMVAN2", "RATLOADER2", "", "", "", ""
};
//-----------------------------------------------------------

CVehicle::CVehicle(int VehicleModel, float fPosX, float fPosY, float fPosZ, float fRotation)
{
	DWORD dwRetID = 0;
	m_dwGTAId = 0;
	m_bIsInvulnerable = FALSE;
	//TODO: Write vehicle spawning code here

	//Vector3 MyCoords = GET_ENTITY_COORDS(PLAYER_PED_ID(), 1);
	if (VehicleModel > (sizeof(vehModels) / sizeof(*vehModels)) || VehicleModel < 0)
		VehicleModel = 0;

	if (strlen(vehModels[VehicleModel]) < 1) VehicleModel = 0;

	int VehicleHash = GAMEPLAY::GET_HASH_KEY(vehModels[VehicleModel]);
	STREAMING::REQUEST_MODEL(VehicleHash);

	while (!STREAMING::HAS_MODEL_LOADED(VehicleHash)) WAIT(0);
	pVehiclePtr = (Vehicle*)VEHICLE::CREATE_VEHICLE(VehicleHash, fPosX, fPosY, fPosZ, fRotation, 1, 0);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(VehicleHash);
#if 0
	// Is the model not loaded?
	if (!pGame->IsModelLoaded(iType))
	{
		// Request the model
		pGame->RequestModel(iType);

		// Load all requested models
		pGame->LoadRequestedModels();

		// Wait for the model to load
		while (!pGame->IsModelLoaded(iType))
		{
			Sleep(2);
		}
	}

	ScriptCommand(&create_car, iType, fPosX, fPosY, fPosZ, &dwRetID);
	ScriptCommand(&set_car_z_angle, dwRetID, fRotation);
	SetEntity((ENTITY_TYPE*)CPools::GetVehicleFromIndex(dwRetID));
	m_dwGTAId = dwRetID;

	Vector3 vPos;
	GetPosition(vPos);
	vPos.Z = fPosZ;
	SetPosition(vPos);

	VEHICLE_TYPE * pVehicle = (VEHICLE_TYPE *)GetEntity();
	pVehicle->dwDoorsLocked = 0;
	//m_pEntity->byteLockedFlags = 1;

	SetInvulnerable(TRUE);
	ToggleRhinoInstantExplosionUponContact(FALSE);
	m_bDead = FALSE;
#endif
}

//-----------------------------------------------------------

CVehicle::~CVehicle()
{
	//TODO: Write vehicle removal code here
	if(pVehiclePtr) {
		ENTITY::SET_ENTITY_AS_MISSION_ENTITY((Entity)pVehiclePtr, 1, 1);
		//VEHICLE::DELETE_VEHICLE(&VehicleHandle);
		ENTITY::DELETE_ENTITY((Entity*)&pVehiclePtr);
		pVehiclePtr = nullptr;
		//delete pVehiclePtr;
	}
#if 0
	// Get the vehicle pointer
	VEHICLE_TYPE * pVehicle = (VEHICLE_TYPE *)GetEntity();

	// Remove ourself from the world
	CWorld::Remove((ENTITY_TYPE *)pVehicle);

	// Remove all references to ourself
	CWorld::Remove((ENTITY_TYPE *)pVehicle);

	// Call class destructor
	_asm
	{
		mov ecx, pVehicle
			mov ebx, [ecx]
			push 1
			call[ebx + 8]
	}

	// Set our pointer to null
	SetEntity(NULL);
#endif
}
void CVehicle::GetQuaternion(float * quat)
{
	//assert(quat);

	if (pVehiclePtr)
	{
		ENTITY::GET_ENTITY_QUATERNION((Entity)pVehiclePtr, &quat[0], &quat[1], &quat[2], &quat[3]);
	}
}
void CVehicle::SetQuaternion(float * quat)
{
	//assert(quat);

	if (pVehiclePtr)
	{
		ENTITY::SET_ENTITY_QUATERNION((Entity)pVehiclePtr, quat[0], quat[1], quat[2], quat[3]);
	}
}