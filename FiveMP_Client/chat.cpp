#include <stdafx.h>
#include "chat.h"
#include "StdInc.h"

extern CNetworkManager *pNetowkManager;
extern CMDWindow *pCmdWindow;

int m_lFontSizeY = CHAT_DEF_FONT_SIZE;

void CChatWindow::HandleChatOnTick() {
	this->OnTickDrawChat();
}
void CChatWindow::updateKeyBoardUI() {
	// Wait for the user to edit
	while (GAMEPLAY::UPDATE_ONSCREEN_KEYBOARD() == 0) {
		m_bIsUpdating = true;
		WAIT(0);
	}
	// Make sure they didn't exit without confirming their change
	if (!GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT()) {
		m_bIsUpdating = false;
		m_bOverlayEnabled = false;
		m_iNextMsgTime = GetTickCount64() + CHAT_NEXT_MSG_TIME;
		return;
	}
	// Send message to myself
	//this->AddChatMessage("", 0x00000000, GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT());
	ProcessInput(GAMEPLAY::GET_ONSCREEN_KEYBOARD_RESULT());
	m_bOverlayEnabled = false;
	m_bIsUpdating = false;
	m_iNextMsgTime = GetTickCount64() + CHAT_NEXT_MSG_TIME;
}
typedef struct {
	char name;
} HostCMDInfo;
//HostCMDFunc
HostCMDInfo host_cmds[] = {
	{ '!' },
	{ '#' },
	{ '/' }
};
int CChatWindow::find_cmdParam(const char *name) {
	for (int i = 0; i<sizeof(host_cmds) / sizeof(HostCMDInfo); i++) {
		if (name[0] == host_cmds[i].name) {
			return 1;
		}
	}
	return NULL;
}
void CChatWindow::ProcessInput(char *inputBuffer)
{
	PCHAR szCmdEndPos;

	if (inputBuffer[0] != '\0')
	{
		if (find_cmdParam(inputBuffer)) { // possible valid command
			if (pCmdWindow->tryFindCommand(inputBuffer)) //It's a client command that they entered, so process it
				return;

			bool bSendToServer = true;
			if (bSendToServer) {
				if (pNetowkManager->IsConnected()) {
					RakNet::BitStream bsSend;
					BYTE byteTextLen = strlen(inputBuffer);
					bsSend.Write(byteTextLen);
					bsSend.Write(inputBuffer, byteTextLen);
					pNetowkManager->GetRPC4()->Call("ChatCommand", &bsSend, HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, TRUE);
				}
				else {
					//pChatWindow->AddInfoMessage("Not connected");
				}
			}
		}
		else
		{
			CLocalPlayer *pLocalPlayer;
			pLocalPlayer = pNetowkManager->GetPlayerManager()->GetLocalPlayer();
			pLocalPlayer->Say(inputBuffer);
			// chat type message
		}
	}
}
void CChatWindow::showKeyBoardUI() {
	// Invoke keyboard
	m_bOverlayEnabled = true;
	WAIT(200);
	GAMEPLAY::DISPLAY_ONSCREEN_KEYBOARD(true, "", "", "", "", "", "", 128);
}
void CChatWindow::OnTickDrawChat() {
	DWORD dwColorChat = 0;
	RECT rect;
	RECT rectSize;
	int x = 0;
	int i = 0;
	int iMessageAt;

	rect.top = 10;
	rect.left = 30;
	rect.bottom = 110;
	rect.right = 550;
	iMessageAt = (m_iCurrentPage * DISP_MESSAGES) - 1; //m_iCurrentPage starts from 1
	int screen_w, screen_h;
	GRAPHICS::_GET_SCREEN_ACTIVE_RESOLUTION(&screen_w, &screen_h);

	if (this != NULL) {
		while (x != DISP_MESSAGES) {
			switch (m_ChatWindowEntries[iMessageAt].eType) {
				case CHAT_TYPE_CHAT:
					i = strlen(m_ChatWindowEntries[iMessageAt].szNick);
					if (i) {
						draw_menu_line(m_ChatWindowEntries[iMessageAt].szMessage, 15.0f, m_lFontSizeY, rect.top, rect.left, 5.0f, false, false, false);
						rect.left = 35;
					}
					break;

				case CHAT_TYPE_INFO:
				case CHAT_TYPE_DEBUG:
					break;
			}
			rect.top += ((screen_h / m_lFontSizeY) / 2) + 5; //Still hardcoded 5 pix but at least it now works based on resolution
			//rect.top += m_lFontSizeY + 11; //The hardcode is real (This is the gap between messages)
			rect.bottom = rect.top + m_lFontSizeY + 1;
			rect.left = 30;

			iMessageAt--;
			x++;
		}
	}
}
CChatWindow::CChatWindow() {
	m_bOverlayEnabled = false;
	m_iNextMsgTime = GetTickCount64() + CHAT_NEXT_MSG_TIME;
	m_iEnabled = CHAT_WINDOW_MODE_FULL;
	m_iCurrentPage = 1;
	m_lFontSizeY = CHAT_DEF_FONT_SIZE; //50 by default
}
CChatWindow::~CChatWindow() {

}
void CChatWindow::AddChatMessage(CHAR *szNick, DWORD dwNickColor, CHAR *szMessage)
{
	FilterInvalidChars(szMessage);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, szMessage, szNick, m_dwChatTextColor, dwNickColor);
}
void CChatWindow::AddDebugMessage(CHAR * szFormat, ...)
{
	char tmp_buf[512];
	memset(tmp_buf, 0, 512);

	va_list args;
	va_start(args, szFormat);
	vsprintf(tmp_buf, szFormat, args);
	va_end(args);

	FilterInvalidChars(tmp_buf);
	AddToChatWindowBuffer(CHAT_TYPE_CHAT, tmp_buf, "", m_dwChatTextColor, 0);
}
void CChatWindow::FilterInvalidChars(PCHAR szString)
{
	while (*szString) {
		if (*szString > 0 && *szString < ' ') {
			*szString = ' ';
		}
		szString++;
	}
}
void CChatWindow::AddToChatWindowBuffer(eChatMessageType eType,
	PCHAR szString,
	PCHAR szNick,
	DWORD dwTextColor,
	DWORD dwChatColor)
{
	int iBestLineLength = 0;

	PushBack();

	m_ChatWindowEntries[0].eType = eType;
	m_ChatWindowEntries[0].dwTextColor = dwTextColor;
	m_ChatWindowEntries[0].dwNickColor = dwChatColor;

	if (szNick) {
		strcpy_s(m_ChatWindowEntries[0].szNick, szNick);
		strcat_s(m_ChatWindowEntries[0].szNick, ":");
	}
	else {
		m_ChatWindowEntries[0].szNick[0] = '\0';
	}

	if (m_ChatWindowEntries[0].eType == CHAT_TYPE_CHAT && strlen(szString) > MAX_LINE_LENGTH)
	{
		iBestLineLength = MAX_LINE_LENGTH;
		// see if we can locate a space.
		while (szString[iBestLineLength] != ' ' && iBestLineLength)
			iBestLineLength--;

		if ((MAX_LINE_LENGTH - iBestLineLength) > 12) {
			// we should just take the whole line
			strncpy_s(m_ChatWindowEntries[0].szMessage, szString, MAX_LINE_LENGTH);
			m_ChatWindowEntries[0].szMessage[MAX_LINE_LENGTH] = '\0';

			PushBack();

			m_ChatWindowEntries[0].eType = eType;
			m_ChatWindowEntries[0].dwTextColor = dwTextColor;
			m_ChatWindowEntries[0].dwNickColor = dwChatColor;
			m_ChatWindowEntries[0].szNick[0] = '\0';

			strcpy_s(m_ChatWindowEntries[0].szMessage, szString + MAX_LINE_LENGTH);
		}
		else {
			// grab upto the found space.
			strncpy_s(m_ChatWindowEntries[0].szMessage, szString, iBestLineLength);
			m_ChatWindowEntries[0].szMessage[iBestLineLength] = '\0';

			PushBack();

			m_ChatWindowEntries[0].eType = eType;
			m_ChatWindowEntries[0].dwTextColor = dwTextColor;
			m_ChatWindowEntries[0].dwNickColor = dwChatColor;
			m_ChatWindowEntries[0].szNick[0] = '\0';

			strcpy_s(m_ChatWindowEntries[0].szMessage, szString + (iBestLineLength + 1));
		}
	}
	else {
		strncpy_s(m_ChatWindowEntries[0].szMessage, szString, MAX_MESSAGE_LENGTH);
		m_ChatWindowEntries[0].szMessage[MAX_MESSAGE_LENGTH] = '\0';
	}

}

void CChatWindow::PageUp()
{
	if (!m_iEnabled) return;

	m_iCurrentPage++;
	if (m_iCurrentPage > CHAT_WINDOW_PAGES) {
		m_iCurrentPage = CHAT_WINDOW_PAGES;
	}

}

//----------------------------------------------------

void CChatWindow::PageDown()
{
	if (!m_iEnabled) return;

	m_iCurrentPage--;
	if (m_iCurrentPage < 1) {
		m_iCurrentPage = 1;
	}
}

//----------------------------------------------------

void CChatWindow::PushBack()
{
	int x = MAX_MESSAGES - 1;
	while (x) {
		memcpy(&m_ChatWindowEntries[x], &m_ChatWindowEntries[x - 1], sizeof(CHAT_WINDOW_ENTRY));
		x--;
	}
}