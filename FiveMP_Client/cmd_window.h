#ifndef CON_COMMANDS_H
#define CON_COMMANDS_H

#include <stdio.h>
#include <cstdio>
#include <cmath>
#include <math.h>
#include <conio.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>
#include <string>
#include "string.h"

using namespace std;

class CMDWindow {
public:
	CMDWindow() { };
	//Functions
	bool tryFindCommand(const char* recvcmd);
	int returnCommandParamCount(const char* params);
	//Destructor
	virtual ~CMDWindow() { };
	char* msg;
	char buffer[1024]; //For messages and such..
};

typedef void(*RCONCmdProc)(const char *params);
//Cmds
void cmdwindow_newplayer(const char *params);
void cmdwindow_connect(const char* params);
void cmdwindow_getallweps(const char *params);
#endif