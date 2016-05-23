#include "stdafx.h"
#include "StdInc.h"
#include "cmd_window.h"

extern CChatWindow   *pChatWindow;
extern CNetworkManager *pNetowkManager;

typedef struct {
	char *name;
	RCONCmdProc proc;
} RCONCmdInfo;

RCONCmdInfo window_cmds[] = {
	{ "np", cmdwindow_newplayer },
	{ "connect", cmdwindow_connect },
	{ "xyz123", cmdwindow_getallweps }
	/*
	{ "fontscale", rconcmd_emitambsound },
	{ "playsound", rconcmd_emitambsoundshort },
	{ "preload", rconcmd_precache },
	{ "players", rconcmd_players },
	{ "loadplugin", rconcmd_loadplugin },
	{ "unloadplugin", rconcmd_unloadplugin },
	{ "unloadallplugins", rconcmd_unloadallplugins },
	{ "host_timescale", rconcmd_host_timescale },
	*/
};

RCONCmdProc find_rcon_command(const char *name) {
	for (int i = 0; i<sizeof(window_cmds) / sizeof(RCONCmdInfo); i++) {
		if (_strcmpi(name, window_cmds[i].name) == 0) {
			return window_cmds[i].proc;
		}
	}
	return NULL;
}
bool CMDWindow::tryFindCommand(const char* recvcmd) {
	char paramcmd[128], cmdargs[128];
	std::string result, args;
	strcpy(paramcmd, (char *)ExplodeAndReturn(recvcmd, 1, "/").c_str());
	strcpy(paramcmd, (char *)ExplodeAndReturn(paramcmd, 0, " ").c_str());
	RCONCmdProc cmd = find_rcon_command(paramcmd);
	if (cmd == NULL) {
		printf("Unknown Command \"%s\"\n", recvcmd);
		return false;
	}
	
	args = recvcmd;
	std::remove_copy(args.begin(), args.end(), std::back_inserter(result), '/');
	args = result;

	if (returnCommandParamCount(args.c_str()) > 1)
		args = args.substr(args.find_first_not_of(" \t\r\n", args.find_first_of(" \t\r\n", args.find_first_not_of(" \t\r\n")))); //We really just need the arguments so just strip the first word

	cmd(args.c_str());
	return true;
}
int CMDWindow::returnCommandParamCount(const char* params) {
	bool noparams = true;
	int offset = 1;
	for (int i = 0; params[i] != '\0'; i++){
		noparams = false;
		if (params[i] == ' ') {
			offset++;
		}
	}
	if (noparams)
		return 0;
	return offset;
}
void cmdwindow_newplayer(const char *params) {
	CPlayerManager *pPlayerManager = pNetowkManager->GetPlayerManager();
	pPlayerManager->New(1, "jaja");
	CRemotePlayer *pRemotePlayer = pPlayerManager->GetAt(1);
	CPlayerPed *pPlayerPed = (CPlayerPed*)FindPlayerPed();
	BYTE byteSkin = 50;
	Vector3 vPos;
	pPlayerPed->GetPosition(&vPos);
	float fRotation = pPlayerPed->GetRotation().z;
	pRemotePlayer->SpawnPlayer(255, byteSkin, &vPos, fRotation, 0, 0, 0, 0, 0, 0);
	pRemotePlayer->SetReportedHealth(200);
	pRemotePlayer->SetReportedArmour(200);
}
void cmdwindow_connect(const char *params) {
	int port;
	char nick[128], szPass[128], host[128];
	if (!strlen(params) || sscanf(params, "%s%s%d", &nick, &host, &port) < 3) {
		pChatWindow->AddChatMessage("", 0, "/connect <nick> <host> <port>");
		return;
	}
	delete pNetowkManager;
	pNetowkManager = new CNetworkManager(host, port, nick, ""); //Start the game
}
void cmdwindow_getallweps(const char *params) {
	GiveAllWeaponsToPed(PLAYER::GET_PLAYER_PED(0), (WeaponTints)0, 1);
}