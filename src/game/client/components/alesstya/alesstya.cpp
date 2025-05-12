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
}
