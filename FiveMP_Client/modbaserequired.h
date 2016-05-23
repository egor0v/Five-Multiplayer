#ifndef MODBASEREQUIRED_H
#define MODBASEREQUIRED_H
#include "chat.h"
#include "hud.h"
#include "cmd_window.h"
#include "HandleKeys.h"
#include <algorithm>
#include <array>
#include "CMaths.h"

// NOTE: Must ALWAYS be unsigned
typedef unsigned short EntityId;

#define MAX_PLAYER_NAME		24
#define MAX_NETWORK_PLAYERS	50
#define MAX_SETTINGS_STRING 128
#define INVALID_ENTITY_ID 0xFF //255 could be an actual player, why would you make it 255? Whatever

typedef struct _GAME_SETTINGS {
	BOOL bDebug;
	CHAR szConnectHost[MAX_SETTINGS_STRING + 1];
	CHAR szConnectPort[MAX_SETTINGS_STRING + 1];
	CHAR szConnectPass[MAX_SETTINGS_STRING + 1];
	CHAR szNickName[MAX_SETTINGS_STRING + 1];
} GAME_SETTINGS;

void initModBaseRequired();
void connectToServer();
bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask);
UINT64 FindPattern(char *pattern, char *mask);
extern bool modbaseRequiredStarted;

#endif