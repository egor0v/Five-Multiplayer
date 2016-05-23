#include "stdafx.h"

void AddClanLogoToVehicle(Vehicle vehicle, Ped ped)
{
	vector3_t x, y, z;
	float scale;
	Hash modelHash = ENTITY::GET_ENTITY_MODEL(vehicle);
	if (GetVehicleInfoForClanLogo(modelHash, x, y, z, scale))
	{
		int alpha = 200;
		if (modelHash == VEHICLE_WINDSOR)
			alpha = 255;
		GRAPHICS::_ADD_CLAN_DECAL_TO_VEHICLE(vehicle, ped, ENTITY::GET_ENTITY_BONE_INDEX_BY_NAME(vehicle, "chassis_dummy"), x.x, x.y, x.z, y.x, y.y, y.z, z.x, z.y, z.z, scale, 0, alpha);
	}
}

Vehicle ClonePedVehicle(Ped ped)
{
	Vehicle pedVeh = NULL;
	Ped playerPed = PLAYER::PLAYER_PED_ID();
	if (PED::IS_PED_IN_ANY_VEHICLE(ped, FALSE))
		pedVeh = PED::GET_VEHICLE_PED_IS_IN(ped, FALSE);
	else //If they're not in a vehicle, try to get their last vehicle.
		pedVeh = PED::GET_VEHICLE_PED_IS_IN(ped, TRUE);
	if (ENTITY::DOES_ENTITY_EXIST(pedVeh))
	{
		Hash vehicleModelHash = ENTITY::GET_ENTITY_MODEL(pedVeh);
		STREAMING::REQUEST_MODEL(vehicleModelHash); //This should already be loaded since we're stealing it from someone in memory.
		Vector3 playerPosition = ENTITY::GET_ENTITY_COORDS(playerPed, FALSE);
		Vehicle playerVeh = VEHICLE::CREATE_VEHICLE(vehicleModelHash, playerPosition.x, playerPosition.y, playerPosition.z, ENTITY::GET_ENTITY_HEADING(playerPed), TRUE, TRUE);
		PED::SET_PED_INTO_VEHICLE(playerPed, playerVeh, SEAT_DRIVER);
		int primaryColor, secondaryColor;
		VEHICLE::GET_VEHICLE_COLOURS(pedVeh, &primaryColor, &secondaryColor);
		VEHICLE::SET_VEHICLE_COLOURS(playerVeh, primaryColor, secondaryColor);
		if (VEHICLE::GET_IS_VEHICLE_PRIMARY_COLOUR_CUSTOM(pedVeh))
		{
			int r, g, b;
			VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(pedVeh, &r, &g, &b);
			VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(playerVeh, r, g, b);
		}
		if (VEHICLE::GET_IS_VEHICLE_SECONDARY_COLOUR_CUSTOM(pedVeh))
		{
			int r, g, b;
			VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(pedVeh, &r, &g, &b);
			VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(playerVeh, r, g, b);
		}
		if (VEHICLE::IS_THIS_MODEL_A_CAR(vehicleModelHash) || VEHICLE::IS_THIS_MODEL_A_BIKE(vehicleModelHash))
		{
			VEHICLE::SET_VEHICLE_MOD_KIT(playerVeh, 0);
			VEHICLE::SET_VEHICLE_WHEEL_TYPE(playerVeh, VEHICLE::GET_VEHICLE_WHEEL_TYPE(pedVeh));
			for (int i = 0; i <= MOD_LIVERY; i++)
			{
				if (i > MOD_ARMOR && i < MOD_FRONTWHEELS)
					VEHICLE::TOGGLE_VEHICLE_MOD(playerVeh, i, VEHICLE::IS_TOGGLE_MOD_ON(pedVeh, i));
				else
					VEHICLE::SET_VEHICLE_MOD(playerVeh, i, VEHICLE::GET_VEHICLE_MOD(pedVeh, i), VEHICLE::GET_VEHICLE_MOD_VARIATION(pedVeh, i));
			}
			int tireSmokeColor[3], pearlescentColor, wheelColor;
			VEHICLE::GET_VEHICLE_TYRE_SMOKE_COLOR(pedVeh, &tireSmokeColor[0], &tireSmokeColor[1], &tireSmokeColor[2]);
			VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(playerVeh, tireSmokeColor[0], tireSmokeColor[1], tireSmokeColor[2]);
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(playerVeh, VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(pedVeh));
			VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(playerVeh, VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(pedVeh));
			VEHICLE::GET_VEHICLE_EXTRA_COLOURS(pedVeh, &pearlescentColor, &wheelColor);
			VEHICLE::SET_VEHICLE_EXTRA_COLOURS(playerVeh, pearlescentColor, wheelColor);
			if (VEHICLE::IS_VEHICLE_A_CONVERTIBLE(pedVeh, 0))
			{
				int convertableState = VEHICLE::GET_CONVERTIBLE_ROOF_STATE(pedVeh);
				if (convertableState == 0 || convertableState == 3 || convertableState == 5)
					VEHICLE::RAISE_CONVERTIBLE_ROOF(playerVeh, 1);
				else
					VEHICLE::LOWER_CONVERTIBLE_ROOF(playerVeh, 1);
			}
			for (int i = 0; i <= NEON_BACK; i++)
			{
				VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(playerVeh, i, VEHICLE::_IS_VEHICLE_NEON_LIGHT_ENABLED(pedVeh, i));
			}
			for (int i = 0; i <= 11; i++)
			{
				if (VEHICLE::DOES_EXTRA_EXIST(pedVeh, i))
					VEHICLE::SET_VEHICLE_EXTRA(playerVeh, i, !VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(pedVeh, i));
			}
			if ((VEHICLE::GET_VEHICLE_LIVERY_COUNT(pedVeh) > 1) && VEHICLE::GET_VEHICLE_LIVERY(pedVeh) >= 0)
			{
				VEHICLE::SET_VEHICLE_LIVERY(playerVeh, VEHICLE::GET_VEHICLE_LIVERY(pedVeh));
			}
			int neonColor[3];
			VEHICLE::_GET_VEHICLE_NEON_LIGHTS_COLOUR(pedVeh, &neonColor[0], &neonColor[1], &neonColor[2]);
			VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(playerVeh, neonColor[0], neonColor[1], neonColor[2]);
			VEHICLE::SET_VEHICLE_WINDOW_TINT(playerVeh, VEHICLE::GET_VEHICLE_WINDOW_TINT(pedVeh));
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(playerVeh, VEHICLE::GET_VEHICLE_DIRT_LEVEL(pedVeh));
			VEHICLE::SET_VEHICLE_ENGINE_ON(playerVeh, TRUE, TRUE, TRUE);
			if (GRAPHICS::_HAS_VEHICLE_GOT_DECAL(pedVeh, 0) == TRUE)
			{
				AddClanLogoToVehicle(playerVeh, ped);
			}
		}
		STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(vehicleModelHash);
	}
	return pedVeh;
}

void DumpVehicleStats(Vehicle vehicle)
{
	if (ENTITY::DOES_ENTITY_EXIST(vehicle) == FALSE || ENTITY::IS_ENTITY_A_VEHICLE(vehicle) == FALSE)
		Log::Error("DumpVehicleStats was passed an incorrect entity");
	static char* modNames[] = { "MOD_SPOILER", "MOD_FRONTBUMPER", "MOD_REARBUMPER", "MOD_SIDESKIRT", "MOD_EXHAUST", "MOD_CHASSIS", "MOD_GRILLE", "MOD_HOOD", "MOD_FENDER", "MOD_RIGHTFENDER", "MOD_ROOF", "MOD_ENGINE", "MOD_BRAKES", "MOD_TRANSMISSION", "MOD_HORNS", "MOD_SUSPENSION", "MOD_ARMOR", "", "MOD_TURBO", "", "MOD_TIRESMOKE", "", "MOD_XENONLIGHTS", "MOD_FRONTWHEELS", "MOD_BACKWHEELS", "", "MOD_VANITYPLATE", "MOD_TRIMDESIGN", "MOD_BOBBLEHEAD", 
		"MOD_SPEEDODIAL", "MOD_SPEAKER", "MOD_SEATS", "MOD_STEERINGWHEEL", "MOD_SHIFTERLEVER", "MOD_PLAQUE", "MOD_SPEAKER2", "MOD_TRUNKSUB", "MOD_HYDRAULICS", "MOD_ENGINECOVER", "MOD_ENGINEFILTER", "", "", "", "MOD_ARCHCOVER",
		"MOD_AERIAL", "MOD_TRIM", "MOD_TANK", "", "", "MOD_LIVERY" };
	int primaryColor, secondaryColor;
	VEHICLE::GET_VEHICLE_COLOURS(vehicle, &primaryColor, &secondaryColor);
	Log::Msg("vehicleModelHash = VEHICLE_%s; //0x%X", VEHICLE::GET_DISPLAY_NAME_FROM_VEHICLE_MODEL(ENTITY::GET_ENTITY_MODEL(vehicle)), ENTITY::GET_ENTITY_MODEL(vehicle));
	Log::Msg("VEHICLE::SET_VEHICLE_COLOURS(playerVeh, %u, %u);", primaryColor, secondaryColor);
	if (VEHICLE::GET_IS_VEHICLE_PRIMARY_COLOUR_CUSTOM(vehicle))
	{
		int r, g, b;
		VEHICLE::GET_VEHICLE_CUSTOM_PRIMARY_COLOUR(vehicle, &r, &g, &b);
		Log::Msg("VEHICLE::SET_VEHICLE_CUSTOM_PRIMARY_COLOUR(playerVeh, %i, %i, %i);", r, g, b);
	}
	if (VEHICLE::GET_IS_VEHICLE_SECONDARY_COLOUR_CUSTOM(vehicle))
	{
		int r, g, b;
		VEHICLE::GET_VEHICLE_CUSTOM_SECONDARY_COLOUR(vehicle, &r, &g, &b);
		Log::Msg("VEHICLE::SET_VEHICLE_CUSTOM_SECONDARY_COLOUR(playerVeh, %i, %i, %i);", r, g, b);
	}
	Log::Msg("VEHICLE::SET_VEHICLE_WINDOW_TINT(playerVeh, %i);", VEHICLE::GET_VEHICLE_WINDOW_TINT(vehicle));
	if (VEHICLE::IS_THIS_MODEL_A_CAR(ENTITY::GET_ENTITY_MODEL(vehicle)) == TRUE || VEHICLE::IS_THIS_MODEL_A_BIKE(ENTITY::GET_ENTITY_MODEL(vehicle)))
	{
		Log::Msg("VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(playerVeh, %s);", VEHICLE::GET_VEHICLE_TYRES_CAN_BURST(vehicle) ? "TRUE" : "FALSE");
		Log::Msg("VEHICLE::SET_VEHICLE_WHEEL_TYPE(playerVeh, %i);", VEHICLE::GET_VEHICLE_WHEEL_TYPE(vehicle));
		for (int i = 0; i <= MOD_LIVERY; i++)
		{
			if (i > MOD_ARMOR && i < MOD_FRONTWHEELS)
			{
				BOOL bModOn = VEHICLE::IS_TOGGLE_MOD_ON(vehicle, i);
				if (i == MOD_TIRESMOKE && bModOn == TRUE)
				{
					int tireSmokeColor[3];
					VEHICLE::GET_VEHICLE_TYRE_SMOKE_COLOR(vehicle, &tireSmokeColor[0], &tireSmokeColor[1], &tireSmokeColor[2]);
					Log::Msg("VEHICLE::TOGGLE_VEHICLE_MOD(playerVeh, MOD_TIRESMOKE, TRUE);", VEHICLE::IS_TOGGLE_MOD_ON(vehicle, i));
					Log::Msg("VEHICLE::SET_VEHICLE_TYRE_SMOKE_COLOR(playerVeh, %u, %u, %u);", tireSmokeColor[0], tireSmokeColor[1], tireSmokeColor[2]);
				}
				else if (bModOn == TRUE)
				{
					Log::Msg("VEHICLE::TOGGLE_VEHICLE_MOD(playerVeh, %s, TRUE);", modNames[i]);
				}
			}
			else if (VEHICLE::GET_VEHICLE_MOD(vehicle, i) != -1)
				Log::Msg("VEHICLE::SET_VEHICLE_MOD(playerVeh, %s, %i, %s);", modNames[i], VEHICLE::GET_VEHICLE_MOD(vehicle, i), VEHICLE::GET_VEHICLE_MOD_VARIATION(vehicle, i) ? "TRUE" : "FALSE");
		}
		int pearlescentColor, wheelColor;
		Log::Msg("VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(playerVeh, \"%s\");", VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT(vehicle));
		Log::Msg("VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(playerVeh, %i);", VEHICLE::GET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vehicle));
		VEHICLE::GET_VEHICLE_EXTRA_COLOURS(vehicle, &pearlescentColor, &wheelColor);
		Log::Msg("VEHICLE::SET_VEHICLE_EXTRA_COLOURS(playerVeh, %u, %u);", pearlescentColor, wheelColor);
		bool bDumpLightColor = false;
		for (int i = 0; i <= NEON_BACK; i++)
		{
			BOOL bIsLightOn = VEHICLE::_IS_VEHICLE_NEON_LIGHT_ENABLED(vehicle, i);
			if (bIsLightOn == TRUE)
			{
				if (bDumpLightColor == false)
				{
					int neonColor[3];
					VEHICLE::_GET_VEHICLE_NEON_LIGHTS_COLOUR(vehicle, &neonColor[0], &neonColor[1], &neonColor[2]);
					Log::Msg("VEHICLE::_SET_VEHICLE_NEON_LIGHTS_COLOUR(playerVeh, %u, %u, %u);", neonColor[0], neonColor[1], neonColor[2]);
					bDumpLightColor = true;
				}
				Log::Msg("VEHICLE::_SET_VEHICLE_NEON_LIGHT_ENABLED(playerVeh, %i, TRUE);", i);
			}
		}
		if (VEHICLE::IS_VEHICLE_A_CONVERTIBLE(vehicle, 0))
		{
			int convertableState = VEHICLE::GET_CONVERTIBLE_ROOF_STATE(vehicle);
			if (convertableState == 0 || convertableState == 3 || convertableState == 5)
				Log::Msg("VEHICLE::RAISE_CONVERTIBLE_ROOF(playerVeh, TRUE);");
			else
				Log::Msg("VEHICLE::LOWER_CONVERTIBLE_ROOF(playerVeh, TRUE);");
		}
		for (int i = 0; i <= 11; i++)
		{
			if (VEHICLE::DOES_EXTRA_EXIST(vehicle, i) == TRUE)
				Log::Msg("VEHICLE::SET_VEHICLE_EXTRA(playerVeh, %i, %s);", i, VEHICLE::IS_VEHICLE_EXTRA_TURNED_ON(vehicle, i) ? "FALSE" : "TRUE");
		}
		if ((VEHICLE::GET_VEHICLE_LIVERY_COUNT(vehicle) > 1) && VEHICLE::GET_VEHICLE_LIVERY(vehicle) >= 0)
		{
			Log::Msg("VEHICLE::SET_VEHICLE_LIVERY(playerVeh, %i);", VEHICLE::GET_VEHICLE_LIVERY(vehicle));
		}
		if (GRAPHICS::_HAS_VEHICLE_GOT_DECAL(vehicle, 0) == TRUE)
		{
			Log::Msg("AddClanLogoToVehicle(playerVeh, playerPed);");
		}
	}
}

void BoostBaseVehicleStats(Vehicle vehicle)
{
	VEHICLE::SET_VEHICLE_DIRT_LEVEL(vehicle, 0.0f);
	ENTITY::SET_ENTITY_INVINCIBLE(vehicle, TRUE);
	ENTITY::SET_ENTITY_PROOFS(vehicle, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE, TRUE);
	VEHICLE::SET_VEHICLE_STRONG(vehicle, TRUE); //2stronk
	VEHICLE::SET_VEHICLE_CAN_BE_VISIBLY_DAMAGED(vehicle, FALSE); //I don't think this really works, but fuck it. Call it anyway.
	VEHICLE::SET_VEHICLE_CAN_BREAK(vehicle, FALSE); //Hopefully this fixes the car blowing up after getting hit by a train...
	VEHICLE::SET_VEHICLE_ENGINE_CAN_DEGRADE(vehicle, FALSE);
	VEHICLE::SET_VEHICLE_IS_STOLEN(vehicle, FALSE); //What seems to be the officer, problem? *le9gagmemeface*
	VEHICLE::SET_VEHICLE_TYRES_CAN_BURST(vehicle, FALSE); //Bulletproof Tires.
	Player player = PLAYER::PLAYER_ID();
	DECORATOR::DECOR_REGISTER("Player_Vehicle", 3);
	DECORATOR::DECOR_REGISTER("Veh_Modded_By_Player", 3);
	DECORATOR::DECOR_SET_INT(vehicle, "Player_Vehicle", NETWORK::_0xBC1D768F2F5D6C05(player));
	DECORATOR::DECOR_SET_INT(vehicle, "Veh_Modded_By_Player", GAMEPLAY::GET_HASH_KEY(PLAYER::GET_PLAYER_NAME(player)));
	Hash vehicleModel = ENTITY::GET_ENTITY_MODEL(vehicle);
	if (VEHICLE::IS_THIS_MODEL_A_CAR(vehicleModel) || VEHICLE::IS_THIS_MODEL_A_BIKE(vehicleModel))
	{
		VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0); //IDK what this does, but I guess it allows individual mods to be added? It's what the game does before calling SET_VEHICLE_MOD.
		VEHICLE::SET_VEHICLE_HAS_STRONG_AXLES(vehicle, TRUE); //6stronk9meme
		VEHICLE::SET_VEHICLE_MOD(vehicle, MOD_ENGINE, MOD_INDEX_FOUR, FALSE); //6fast9furious
		VEHICLE::SET_VEHICLE_MOD(vehicle, MOD_BRAKES, MOD_INDEX_THREE, FALSE); //GOTTA STOP FAST
		VEHICLE::SET_VEHICLE_MOD(vehicle, MOD_TRANSMISSION, MOD_INDEX_THREE, FALSE); //Not when I shift in to MAXIMUM OVERMEME!
		VEHICLE::SET_VEHICLE_MOD(vehicle, MOD_SUSPENSION, MOD_INDEX_FOUR, FALSE); //How low can you go?
		VEHICLE::SET_VEHICLE_MOD(vehicle, MOD_ARMOR, MOD_INDEX_FIVE, FALSE); //100% armor.
		VEHICLE::TOGGLE_VEHICLE_MOD(vehicle, MOD_TURBO, TRUE); //Forced induction huehuehue
	}
	VEHICLE::SET_VEHICLE_BODY_HEALTH(vehicle, 1000.0f); //This is what the game does
}