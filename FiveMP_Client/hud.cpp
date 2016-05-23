#include "stdafx.h"
#include "hud.h"

bool hudHidden = false;

void ShowHud(bool toggle) {
	hudHidden = toggle;
}

bool getHudHidden() {
	return hudHidden;
};

void hudOnTick() {
	if (hudHidden) UI::HIDE_HUD_AND_RADAR_THIS_FRAME();
	return;
}