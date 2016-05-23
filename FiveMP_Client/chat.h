#ifndef CHAT_H
#define CHAT_H

#include "stdafx.h"

#define MAX_MESSAGE_LENGTH			128
#define MAX_LINE_LENGTH				MAX_MESSAGE_LENGTH / 2
#define MAX_MESSAGES				50
#define DISP_MESSAGES				10
#define CHAT_WINDOW_PAGES			MAX_MESSAGES / DISP_MESSAGES
#define MAX_PLAYER_NAME				24
#define CHAT_NEXT_MSG_TIME			500 //Half a second
#define CHAT_WINDOW_MODE_OFF		0
#define CHAT_WINDOW_MODE_LIGHT		1
#define CHAT_WINDOW_MODE_FULL		2
#define CMD_CHARACTER				'/'
#define CHAT_DEF_FONT_SIZE			50
enum eChatMessageType {
	CHAT_TYPE_NONE = 0,
	CHAT_TYPE_CHAT,
	CHAT_TYPE_INFO,
	CHAT_TYPE_DEBUG
};

typedef struct _CHAT_WINDOW_ENTRY
{
	eChatMessageType eType;
	CHAR szMessage[MAX_MESSAGE_LENGTH + 1];
	CHAR szNick[MAX_PLAYER_NAME + 1];
	DWORD dwTextColor;
	DWORD dwNickColor;
} CHAT_WINDOW_ENTRY;

class CChatWindow
{
public:
	void OnTickDrawChat();
	void AddChatMessage(CHAR *szNick, DWORD dwNickColor, CHAR *szMessage);
	void AddDebugMessage(CHAR *szFormat, ...);
	void FilterInvalidChars(PCHAR szString);
	void AddToChatWindowBuffer(eChatMessageType eType, PCHAR szString, PCHAR szNick, DWORD dwTextColor, DWORD dwChatColor);
	void PageUp();
	void PageDown();
	void PushBack();
	void HandleChatOnTick();
	void showKeyBoardUI();
	void updateKeyBoardUI();
	CChatWindow::CChatWindow();
	int find_cmdParam(const char *name);
	void ProcessInput(char* inputBuffer);
	~CChatWindow();

	int					m_iEnabled;
	int					m_iCurrentPage;
	int					m_iNextMsgTime;
	bool				m_bOverlayEnabled;
	bool				m_bIsUpdating;
	DWORD				m_dwChatTextColor;
	CHAT_WINDOW_ENTRY	m_ChatWindowEntries[MAX_MESSAGES];
};
#endif