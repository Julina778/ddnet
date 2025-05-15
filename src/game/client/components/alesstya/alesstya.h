#ifndef GAME_CLIENT_COMPONENTS_ALESSTYA_ALESSTYA_H
#define GAME_CLIENT_COMPONENTS_ALESSTYA_ALESSTYA_H

#include <engine/console.h>
#include <game/client/component.h>

class CAlesstya : public CComponent
{
	class IEngineGraphics *m_pGraphics = nullptr;

	char m_PreviousOwnMessage[2048] = {};

	bool SendNonDuplicateMessage(int Team, const char *pLine);

public:
	CAlesstya();
	int Sizeof() const override { return sizeof(*this); }
	void OnInit() override;
	void OnMessage(int MsgType, void *pRawMsg) override;
	void RenderMiniVoteHud();
};

#endif
