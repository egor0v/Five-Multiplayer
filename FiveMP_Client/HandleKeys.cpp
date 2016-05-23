#include "stdafx.h"
#include "HandleKeys.h"
#include "modbaserequired.h"

extern CChatWindow   *pChatWindow;
ULONGLONG lastKeyPress = 0;

void handleKeysOnTick() {
	//Things like the chat have a 150 ms interval (This code will be improved later on)
	if (GetTickCount64() > lastKeyPress + TIME_INTERVAL_NEXT_KEYPRESS) {
		//Chat window, page up, page down
		if (GetAsyncKeyState(VK_PRIOR)) {
			if (pChatWindow) pChatWindow->PageUp();
		}
		if (GetAsyncKeyState(VK_NEXT)) {
			if (pChatWindow) pChatWindow->PageDown();
		}
		if (GetAsyncKeyState(VK_F7)) {
			ShowHud(getHudHidden() ? true : false);
		}
		//Chat window - text input
		if (pChatWindow) {
			if (!pChatWindow->m_bOverlayEnabled) {
				if (GetAsyncKeyState(TALK_KEY) && GetTickCount64() > pChatWindow->m_iNextMsgTime) {
					pChatWindow->showKeyBoardUI(); //Show the keyboard
				}
			} else {
				//Update the keyboard UI, etc
				pChatWindow->updateKeyBoardUI();
			}
		}
		lastKeyPress = GetTickCount64();
	}
}