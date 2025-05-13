#define PI 3.14159265358979323846

#include <algorithm>
#include <engine/console.h>
#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/shared/protocol.h>
#include <game/client/animstate.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>
#include <game/client/ui.h>
#include <game/generated/protocol.h>
#include <vector>

#include "alesstya.h"

CAlesstya::CAlesstya()
{
	OnReset();
}

void CAlesstya::OnInit()
{
	TextRender()->SetCustomFace(g_Config.m_ClCustomFont);
	m_pGraphics = Kernel()->RequestInterface<IEngineGraphics>();
}

static bool LineShouldHighlight(const char *pLine, const char *pName)
{
	const char *pHL = str_utf8_find_nocase(pLine, pName);
	if(pHL)
	{
		int Length = str_length(pName);
		if(Length > 0 && (pLine == pHL || pHL[-1] == ' ') && (pHL[Length] == 0 || pHL[Length] == ' ' || pHL[Length] == '.' || pHL[Length] == '!' || pHL[Length] == ',' || pHL[Length] == '?' || pHL[Length] == ':'))
			return true;
	}
	return false;
}

bool CAlesstya::SendNonDuplicateMessage(int Team, const char *pLine)
{
	if(str_comp(pLine, m_PreviousOwnMessage) != 0)
	{
		GameClient()->m_Chat.SendChat(Team, pLine);
		return true;
	}
	str_copy(m_PreviousOwnMessage, pLine);
	return false;
}

void CAlesstya::OnMessage(int MsgType, void *pRawMsg)
{
	if(Client()->State() == IClient::STATE_DEMOPLAYBACK)
		return;

	if(MsgType == NETMSGTYPE_SV_CHAT)
	{
		CNetMsg_Sv_Chat *pMsg = (CNetMsg_Sv_Chat *)pRawMsg;
		int ClientId = pMsg->m_ClientId;

		if(ClientId < 0 || ClientId > MAX_CLIENTS)
			return;
		int LocalId = GameClient()->m_Snap.m_LocalClientId;
		if(ClientId == LocalId)
			str_copy(m_PreviousOwnMessage, pMsg->m_pMessage);

		bool PingMessage = false;

		bool ValidIds = !(GameClient()->m_aLocalIds[0] < 0 || (GameClient()->Client()->DummyConnected() && GameClient()->m_aLocalIds[1] < 0));

		if(ValidIds && ClientId >= 0 && ClientId != GameClient()->m_aLocalIds[0] && (!GameClient()->Client()->DummyConnected() || ClientId != GameClient()->m_aLocalIds[1]))
		{
			PingMessage |= LineShouldHighlight(pMsg->m_pMessage, GameClient()->m_aClients[GameClient()->m_aLocalIds[0]].m_aName);
			PingMessage |= GameClient()->Client()->DummyConnected() && LineShouldHighlight(pMsg->m_pMessage, GameClient()->m_aClients[GameClient()->m_aLocalIds[1]].m_aName);
		}

		if(pMsg->m_Team == TEAM_WHISPER_RECV)
			PingMessage = true;

		if(!PingMessage)
			return;

		char aPlayerName[MAX_NAME_LENGTH];
		str_copy(aPlayerName, GameClient()->m_aClients[ClientId].m_aName, sizeof(aPlayerName));
		/*
		bool PlayerMuted = GameClient()->m_aClients[ClientId].m_Foe || GameClient()->m_aClients[ClientId].m_ChatIgnore;
		if(g_Config.m_ClAutoReplyMuted && PlayerMuted)
		{
			char aBuf[256];
			if(pMsg->m_Team == TEAM_WHISPER_RECV)
			{
				str_format(aBuf, sizeof(aBuf), "/w %s %s", aPlayerName, g_Config.m_ClAutoReplyMutedMessage);
				SendNonDuplicateMessage(0, aBuf);
			}
			else
			{
				str_format(aBuf, sizeof(aBuf), "%s: %s", aPlayerName, g_Config.m_ClAutoReplyMutedMessage);
				SendNonDuplicateMessage(0, aBuf);
			}
			return;
		}
		*/
		bool WindowActive = m_pGraphics && m_pGraphics->WindowActive();
		if(g_Config.m_ClAutoReplyMinimized && !WindowActive && m_pGraphics)
		{
			char aBuf[256];
			if(pMsg->m_Team == TEAM_WHISPER_RECV)
			{
				str_format(aBuf, sizeof(aBuf), "/w %s %s", aPlayerName, g_Config.m_ClAutoReplyMinimizedMessage);
				SendNonDuplicateMessage(0, aBuf);
			}
			else
			{
				str_format(aBuf, sizeof(aBuf), "%s: %s", aPlayerName, g_Config.m_ClAutoReplyMinimizedMessage);
				SendNonDuplicateMessage(0, aBuf);
			}
			return;
		}
	}
}
