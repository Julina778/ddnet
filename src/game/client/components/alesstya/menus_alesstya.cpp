#include <base/log.h>
#include <base/math.h>
#include <base/system.h>

#include <engine/graphics.h>
#include <engine/shared/config.h>
#include <engine/shared/linereader.h>
#include <engine/shared/localization.h>
#include <engine/shared/protocol7.h>
#include <engine/storage.h>
#include <engine/textrender.h>
#include <engine/updater.h>

#include <game/generated/protocol.h>

#include <game/client/animstate.h>
#include <game/client/components/alesstya/bindwheel.h>
#include <game/client/components/chat.h>
#include <game/client/components/menu_background.h>
#include <game/client/components/sounds.h>
#include <game/client/gameclient.h>
#include <game/client/render.h>
#include <game/client/skin.h>
#include <game/client/ui.h>
#include <game/client/ui_listbox.h>
#include <game/client/ui_scrollregion.h>
#include <game/localization.h>

#include "../binds.h"
#include "../countryflags.h"
#include "../menus.h"
#include "../skins.h"

#include <vector>

enum
{
	ALESSTYA_TAB_SETTINGS = 0,
	ALESSTYA_TAB_BINDWHEEL,
	ALESSTYA_TAB_INFO,
	NUMBER_OF_ALESSTYA_TABS
};

typedef struct
{
	const char *m_pName;
	const char *m_pCommand;
	int m_KeyId;
	int m_ModifierCombination;
} CKeyInfo;

using namespace FontIcons;

static float s_Time = 0.0f;
static bool s_StartedTime = false;

const float FontSize = 14.0f;
const float EditBoxFontSize = 12.0f;
const float LineSize = 20.0f;
const float ColorPickerLineSize = 25.0f;
const float HeadlineFontSize = 20.0f;
[[maybe_unused]] const float StandardFontSize = 14.0f;

const float HeadlineHeight = HeadlineFontSize + 0.0f;
const float Margin = 10.0f;
const float MarginSmall = 5.0f;
const float MarginExtraSmall = 2.5f;
const float MarginBetweenSections = 30.0f;
const float MarginBetweenViews = 30.0f;

const float ColorPickerLabelSize = 13.0f;
const float ColorPickerLineSpacing = 5.0f;

bool CMenus::DoSliderWithScaledValue(const void *pId, int *pOption, const CUIRect *pRect, const char *pStr, int Min, int Max, int Scale, const IScrollbarScale *pScale, unsigned Flags, const char *pSuffix)
{
	const bool NoClampValue = Flags & CUi::SCROLLBAR_OPTION_NOCLAMPVALUE;

	int Value = *pOption;
	Min /= Scale;
	Max /= Scale;
	// Allow adjustment of slider options when ctrl is pressed (to avoid scrolling, or accidently adjusting the value)
	int Increment = std::max(1, (Max - Min) / 35);
	if(Input()->ModifierIsPressed() && Input()->KeyPress(KEY_MOUSE_WHEEL_UP) && Ui()->MouseInside(pRect))
	{
		Value += Increment;
		Value = clamp(Value, Min, Max);
	}
	if(Input()->ModifierIsPressed() && Input()->KeyPress(KEY_MOUSE_WHEEL_DOWN) && Ui()->MouseInside(pRect))
	{
		Value -= Increment;
		Value = clamp(Value, Min, Max);
	}

	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "%s: %i%s", pStr, Value * Scale, pSuffix);

	if(NoClampValue)
	{
		// Clamp the value internally for the scrollbar
		Value = clamp(Value, Min, Max);
	}

	CUIRect Label, ScrollBar;
	pRect->VSplitMid(&Label, &ScrollBar, minimum(10.0f, pRect->w * 0.05f));

	const float LabelFontSize = Label.h * CUi::ms_FontmodHeight * 0.8f;
	Ui()->DoLabel(&Label, aBuf, LabelFontSize, TEXTALIGN_ML);

	Value = pScale->ToAbsolute(Ui()->DoScrollbarH(pId, &ScrollBar, pScale->ToRelative(Value, Min, Max)), Min, Max);
	if(NoClampValue && ((Value == Min && *pOption < Min) || (Value == Max && *pOption > Max)))
	{
		Value = *pOption;
	}

	if(*pOption != Value)
	{
		*pOption = Value;
		return true;
	}
	return false;
}

bool CMenus::DoEditBoxWithLabel(CLineInput *LineInput, const CUIRect *pRect, const char *pLabel, const char *pDefault, char *pBuf, size_t BufSize)
{
	CUIRect Button, Label;
	pRect->VSplitLeft(210.0f, &Label, &Button);
	Ui()->DoLabel(&Label, pLabel, FontSize, TEXTALIGN_ML);
	LineInput->SetBuffer(pBuf, BufSize);
	LineInput->SetEmptyText(pDefault);
	return Ui()->DoEditBox(LineInput, &Button, EditBoxFontSize);
}

int CMenus::DoButtonLineSize_Menu(CButtonContainer *pButtonContainer, const char *pText, int Checked, const CUIRect *pRect, float ButtonLineSize, bool Fake, const char *pImageName, int Corners, float Rounding, float FontFactor, ColorRGBA Color)
{
	CUIRect Text = *pRect;

	if(Checked)
		Color = ColorRGBA(0.6f, 0.6f, 0.6f, 0.5f);
	Color.a *= Ui()->ButtonColorMul(pButtonContainer);

	if(Fake)
		Color.a *= 0.5f;

	pRect->Draw(Color, Corners, Rounding);

	Text.HMargin((Text.h - ButtonLineSize) / 2.0f, &Text);
	Text.HMargin(pRect->h >= 20.0f ? 2.0f : 1.0f, &Text);
	Text.HMargin((Text.h * FontFactor) / 2.0f, &Text);
	Ui()->DoLabel(&Text, pText, Text.h * CUi::ms_FontmodHeight, TEXTALIGN_MC);

	if(Fake)
		return 0;

	return Ui()->DoButtonLogic(pButtonContainer, Checked, pRect, BUTTONFLAG_LEFT);
}

void CMenus::RenderSettingsAlesstya(CUIRect MainView)
{
	s_Time += Client()->RenderFrameTime() * (1.0f / 100.0f);
	if(!s_StartedTime)
	{
		s_StartedTime = true;
		s_Time = (float)rand() / (float)RAND_MAX;
	}

	static int s_CurCustomTab = 0;

	CUIRect TabBar, LeftView, RightView, Button, Label;

	MainView.HSplitTop(LineSize * 1.2f, &TabBar, &MainView);
	const float TabWidth = TabBar.w / NUMBER_OF_ALESSTYA_TABS;
	static CButtonContainer s_aPageTabs[NUMBER_OF_ALESSTYA_TABS] = {};
	const char *apTabNames[] = {
		Localize("Settings"),
		Localize("Bindwheel"),
		Localize("Info")};

	for(int Tab = 0; Tab < NUMBER_OF_ALESSTYA_TABS; ++Tab)
	{
		TabBar.VSplitLeft(TabWidth, &Button, &TabBar);
		const int Corners = Tab == 0 ? IGraphics::CORNER_L : Tab == NUMBER_OF_ALESSTYA_TABS - 1 ? IGraphics::CORNER_R : IGraphics::CORNER_NONE;
		if(DoButton_MenuTab(&s_aPageTabs[Tab], apTabNames[Tab], s_CurCustomTab == Tab, &Button, Corners, nullptr, nullptr, nullptr, nullptr, 4.0f))
			s_CurCustomTab = Tab;
	}

	MainView.HSplitTop(MarginSmall, nullptr, &MainView);

	if(s_CurCustomTab == ALESSTYA_TAB_SETTINGS)
	{
		RenderSettingsAlesstyaSettngs(MainView);
	}
	else if(s_CurCustomTab == ALESSTYA_TAB_BINDWHEEL)
	{
		MainView.HSplitTop(MarginBetweenSections, nullptr, &MainView);
		MainView.VSplitLeft(MainView.w / 2.1f, &LeftView, &RightView);

		const float Radius = minimum(RightView.w, RightView.h) / 2.0f;
		vec2 Pos{RightView.x + RightView.w / 2.0f, RightView.y + RightView.h / 2.0f};
		// Draw Circle
		Graphics()->TextureClear();
		Graphics()->QuadsBegin();
		Graphics()->SetColor(0.0f, 0.0f, 0.0f, 0.3f);
		Graphics()->DrawCircle(Pos.x, Pos.y, Radius, 64);
		Graphics()->QuadsEnd();

		static char s_aBindName[BINDWHEEL_MAX_NAME];
		static char s_aBindCommand[BINDWHEEL_MAX_CMD];

		static int s_SelectedBindIndex = -1;
		int HoveringIndex = -1;

		float MouseDist = distance(Pos, Ui()->MousePos());
		if(MouseDist < Radius && MouseDist > Radius * 0.25f)
		{
			int SegmentCount = GameClient()->m_BindWheel.m_vBinds.size();
			float SegmentAngle = 2.0f * pi / SegmentCount;

			float HoveringAngle = angle(Ui()->MousePos() - Pos) + SegmentAngle / 2.0f;
			if(HoveringAngle < 0.0f)
				HoveringAngle += 2.0f * pi;

			HoveringIndex = (int)(HoveringAngle / (2.0f * pi) * SegmentCount);
			if(Ui()->MouseButtonClicked(0))
			{
				s_SelectedBindIndex = HoveringIndex;
				str_copy(s_aBindName, GameClient()->m_BindWheel.m_vBinds[HoveringIndex].m_aName);
				str_copy(s_aBindCommand, GameClient()->m_BindWheel.m_vBinds[HoveringIndex].m_aCommand);
			}
			else if(Ui()->MouseButtonClicked(1) && s_SelectedBindIndex >= 0 && HoveringIndex >= 0 && HoveringIndex != s_SelectedBindIndex)
			{
				CBindWheel::CBind BindA = GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex];
				CBindWheel::CBind BindB = GameClient()->m_BindWheel.m_vBinds[HoveringIndex];
				str_copy(GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex].m_aName, BindB.m_aName);
				str_copy(GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex].m_aCommand, BindB.m_aCommand);
				str_copy(GameClient()->m_BindWheel.m_vBinds[HoveringIndex].m_aName, BindA.m_aName);
				str_copy(GameClient()->m_BindWheel.m_vBinds[HoveringIndex].m_aCommand, BindA.m_aCommand);
			}
			else if(Ui()->MouseButtonClicked(2))
			{
				s_SelectedBindIndex = HoveringIndex;
			}
		}
		else if(MouseDist < Radius && Ui()->MouseButtonClicked(0))
		{
			s_SelectedBindIndex = -1;
			str_copy(s_aBindName, "");
			str_copy(s_aBindCommand, "");
		}

		const float Theta = pi * 2.0f / GameClient()->m_BindWheel.m_vBinds.size();
		for(int i = 0; i < static_cast<int>(GameClient()->m_BindWheel.m_vBinds.size()); i++)
		{
			float SegmentFontSize = FontSize * 1.1f;
			if(i == s_SelectedBindIndex)
			{
				SegmentFontSize = FontSize * 1.7f;
				TextRender()->TextColor(ColorRGBA(0.5f, 1.0f, 0.75f, 1.0f));
			}
			else if(i == HoveringIndex)
			{
				SegmentFontSize = FontSize * 1.35f;
			}

			const CBindWheel::CBind Bind = GameClient()->m_BindWheel.m_vBinds[i];
			const float Angle = Theta * i;
			vec2 TextPos = direction(Angle);
			TextPos *= Radius * 0.75f;

			float Width = TextRender()->TextWidth(SegmentFontSize, Bind.m_aName);
			TextPos += Pos;
			TextPos.x -= Width / 2.0f;
			TextRender()->Text(TextPos.x, TextPos.y, SegmentFontSize, Bind.m_aName);
			TextRender()->TextColor(TextRender()->DefaultTextColor());
		}

		LeftView.HSplitTop(LineSize, &Button, &LeftView);
		Button.VSplitLeft(100.0f, &Label, &Button);
		Ui()->DoLabel(&Label, Localize("Name:"), FontSize, TEXTALIGN_ML);
		static CLineInput s_NameInput;
		s_NameInput.SetBuffer(s_aBindName, sizeof(s_aBindName));
		s_NameInput.SetEmptyText(Localize("Name"));
		Ui()->DoEditBox(&s_NameInput, &Button, EditBoxFontSize);

		LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);
		LeftView.HSplitTop(LineSize, &Button, &LeftView);
		Button.VSplitLeft(100.0f, &Label, &Button);
		Ui()->DoLabel(&Label, Localize("Command:"), FontSize, TEXTALIGN_ML);
		static CLineInput s_BindInput;
		s_BindInput.SetBuffer(s_aBindCommand, sizeof(s_aBindCommand));
		s_BindInput.SetEmptyText(Localize("Command"));
		Ui()->DoEditBox(&s_BindInput, &Button, EditBoxFontSize);

		static CButtonContainer s_AddButton, s_RemoveButton, s_OverrideButton;

		LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);
		LeftView.HSplitTop(LineSize, &Button, &LeftView);
		if(DoButton_Menu(&s_OverrideButton, Localize("Override Selected"), 0, &Button) && s_SelectedBindIndex >= 0)
		{
			CBindWheel::CBind TempBind;
			if(str_length(s_aBindName) == 0)
				str_copy(TempBind.m_aName, "*");
			else
				str_copy(TempBind.m_aName, s_aBindName);

			str_copy(GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex].m_aName, TempBind.m_aName);
			str_copy(GameClient()->m_BindWheel.m_vBinds[s_SelectedBindIndex].m_aCommand, s_aBindCommand);
		}
		LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);
		LeftView.HSplitTop(LineSize, &Button, &LeftView);
		CUIRect ButtonAdd, ButtonRemove;
		Button.VSplitMid(&ButtonRemove, &ButtonAdd, MarginSmall);
		if(DoButton_Menu(&s_AddButton, Localize("Add Bind"), 0, &ButtonAdd))
		{
			CBindWheel::CBind TempBind;
			if(str_length(s_aBindName) == 0)
				str_copy(TempBind.m_aName, "*");
			else
				str_copy(TempBind.m_aName, s_aBindName);

			GameClient()->m_BindWheel.AddBind(TempBind.m_aName, s_aBindCommand);
			s_SelectedBindIndex = static_cast<int>(GameClient()->m_BindWheel.m_vBinds.size()) - 1;
		}
		if(DoButton_Menu(&s_RemoveButton, Localize("Remove Bind"), 0, &ButtonRemove) && s_SelectedBindIndex >= 0)
		{
			GameClient()->m_BindWheel.RemoveBind(s_SelectedBindIndex);
			s_SelectedBindIndex = -1;
		}

		LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);
		LeftView.HSplitTop(LineSize, &Label, &LeftView);
		Ui()->DoLabel(&Label, Localize("Use left mouse to select"), FontSize, TEXTALIGN_ML);
		LeftView.HSplitTop(LineSize, &Label, &LeftView);
		Ui()->DoLabel(&Label, Localize("Use right mouse to swap with selected"), FontSize, TEXTALIGN_ML);
		LeftView.HSplitTop(LineSize, &Label, &LeftView);
		Ui()->DoLabel(&Label, Localize("Use middle mouse select without copy"), FontSize, TEXTALIGN_ML);

		// Do Settings Key
		CKeyInfo Key = CKeyInfo{"Bind Wheel Key", "+bindwheel", 0, 0};
		for(int Mod = 0; Mod < CBinds::MODIFIER_COMBINATION_COUNT; Mod++)
		{
			for(int KeyId = 0; KeyId < KEY_LAST; KeyId++)
			{
				const char *pBind = m_pClient->m_Binds.Get(KeyId, Mod);
				if(!pBind[0])
					continue;

				if(str_comp(pBind, Key.m_pCommand) == 0)
				{
					Key.m_KeyId = KeyId;
					Key.m_ModifierCombination = Mod;
					break;
				}
			}
		}

		CUIRect KeyLabel;
		LeftView.HSplitBottom(LineSize, &LeftView, &Button);
		Button.VSplitLeft(120.0f, &KeyLabel, &Button);
		Button.VSplitLeft(100.0f, &Button, nullptr);
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "%s:", Localize(Key.m_pName));

		Ui()->DoLabel(&KeyLabel, aBuf, FontSize, TEXTALIGN_ML);
		int OldId = Key.m_KeyId, OldModifierCombination = Key.m_ModifierCombination, NewModifierCombination;
		int NewId = DoKeyReader((void *)&Key.m_pName, &Button, OldId, OldModifierCombination, &NewModifierCombination);
		if(NewId != OldId || NewModifierCombination != OldModifierCombination)
		{
			if(OldId != 0 || NewId == 0)
				m_pClient->m_Binds.Bind(OldId, "", false, OldModifierCombination);
			if(NewId != 0)
				m_pClient->m_Binds.Bind(NewId, Key.m_pCommand, false, NewModifierCombination);
		}
		LeftView.HSplitBottom(LineSize, &LeftView, &Button);

		DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClResetBindWheelMouse, Localize("Reset position of mouse when opening bindwheel"), &g_Config.m_ClResetBindWheelMouse, &Button, LineSize);
	}
	else if(s_CurCustomTab == ALESSTYA_TAB_INFO)
	{
		RenderSettingsInfo(MainView);
	}
}

void CMenus::RenderSettingsAlesstyaSettngs(CUIRect MainView)
{
	CUIRect Column, LeftView, RightView, Button, Label;

	static CScrollRegion s_ScrollRegion;
	vec2 ScrollOffset(0.0f, 0.0f);
	CScrollRegionParams ScrollParams;
	ScrollParams.m_ScrollUnit = 120.0f;
	ScrollParams.m_Flags = CScrollRegionParams::FLAG_CONTENT_STATIC_WIDTH;
	ScrollParams.m_ScrollbarMargin = 5.0f;
	s_ScrollRegion.Begin(&MainView, &ScrollOffset, &ScrollParams);

	static std::vector<CUIRect> s_SectionBoxes;
	static vec2 s_PrevScrollOffset(0.0f, 0.0f);

	MainView.y += ScrollOffset.y;

	MainView.VSplitRight(5.0f, &MainView, nullptr); // Padding for scrollbar
	MainView.VSplitLeft(5.0f, nullptr, &MainView); // Padding for scrollbar

	MainView.VSplitMid(&LeftView, &RightView, MarginBetweenViews);
	LeftView.VSplitLeft(MarginSmall, nullptr, &LeftView);
	RightView.VSplitRight(MarginSmall, &RightView, nullptr);

	// RightView.VSplitRight(10.0f, &RightView, nullptr);
	for(CUIRect &Section : s_SectionBoxes)
	{
		float Padding = MarginBetweenViews * 0.6666f;
		Section.w += Padding;
		Section.h += Padding;
		Section.x -= Padding * 0.5f;
		Section.y -= Padding * 0.5f;
		Section.y -= s_PrevScrollOffset.y - ScrollOffset.y;
		float Shade = 0.0f;
		Section.Draw(ColorRGBA(Shade, Shade, Shade, 0.25f), IGraphics::CORNER_ALL, 10.0f);
	}
	s_PrevScrollOffset = ScrollOffset;
	s_SectionBoxes.clear();

	// ***** LeftView ***** //
	Column = LeftView;

	// ***** Visual Miscellaneous ***** //
	Column.HSplitTop(Margin, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Visual"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	static std::vector<const char *> s_FontDropDownNames = {};
	static CUi::SDropDownState s_FontDropDownState;
	static CScrollRegion s_FontDropDownScrollRegion;
	s_FontDropDownState.m_SelectionPopupContext.m_pScrollRegion = &s_FontDropDownScrollRegion;
	s_FontDropDownState.m_SelectionPopupContext.m_SpecialFontRenderMode = true;
	int FontSelectedOld = -1;
	for(size_t i = 0; i < TextRender()->GetCustomFaces()->size(); ++i)
	{
		if(s_FontDropDownNames.size() != TextRender()->GetCustomFaces()->size())
			s_FontDropDownNames.push_back(TextRender()->GetCustomFaces()->at(i).c_str());

		if(str_find_nocase(g_Config.m_ClCustomFont, TextRender()->GetCustomFaces()->at(i).c_str()))
			FontSelectedOld = i;
	}
	CUIRect FontDropDownRect, FontDirectory;
	Column.HSplitTop(LineSize, &FontDropDownRect, &Column);
	FontDropDownRect.VSplitLeft(100.0f, &Label, &FontDropDownRect);
	FontDropDownRect.VSplitRight(20.0f, &FontDropDownRect, &FontDirectory);
	FontDropDownRect.VSplitRight(MarginSmall, &FontDropDownRect, nullptr);

	Ui()->DoLabel(&Label, Localize("Custom Font: "), FontSize, TEXTALIGN_ML);
	const int FontSelectedNew = Ui()->DoDropDown(&FontDropDownRect, FontSelectedOld, s_FontDropDownNames.data(), s_FontDropDownNames.size(), s_FontDropDownState);
	if(FontSelectedOld != FontSelectedNew)
	{
		str_copy(g_Config.m_ClCustomFont, s_FontDropDownNames[FontSelectedNew]);
		TextRender()->SetCustomFace(g_Config.m_ClCustomFont);

		// Attempt to reset all the containers
		TextRender()->OnPreWindowResize();
		GameClient()->OnWindowResize();
		GameClient()->Editor()->OnWindowResize();
		TextRender()->OnWindowResize();
		GameClient()->m_MapImages.SetTextureScale(101);
		GameClient()->m_MapImages.SetTextureScale(g_Config.m_ClTextEntitiesSize);
	}

	static CButtonContainer s_FontDirectoryId;
	if(DoButton_FontIcon(&s_FontDirectoryId, FONT_ICON_FOLDER, 0, &FontDirectory, IGraphics::CORNER_ALL))
	{
		Storage()->CreateFolder("data/alesstya", IStorage::TYPE_ABSOLUTE);
		Storage()->CreateFolder("data/alesstya/fonts", IStorage::TYPE_ABSOLUTE);
		Client()->ViewFile("data/alesstya/fonts");
	}

	Column.HSplitTop(MarginExtraSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClFreezeUpdateFix, Localize("Update tee skin faster after being frozen"), &g_Config.m_ClFreezeUpdateFix, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClPingNameCircle, Localize("Show ping colored circle before names"), &g_Config.m_ClPingNameCircle, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClShowSkinName, Localize("Show skin names in nameplate"), &g_Config.m_ClShowSkinName, &Column, LineSize);

	Column.HSplitTop(MarginExtraSmall, nullptr, &Column);
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Input ***** //
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Input"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClFastInput, Localize("Fast Inputs (-20ms visual delay)"), &g_Config.m_ClFastInput, &Column, LineSize);

	Column.HSplitTop(MarginSmall, nullptr, &Column);
	if(g_Config.m_ClFastInput)
		DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClFastInputOthers, Localize("Extra tick other tees (increases other tees latency, \nmakes dragging slightly easier when using fast input)"), &g_Config.m_ClFastInputOthers, &Column, LineSize);
	else
		Column.HSplitTop(LineSize, nullptr, &Column);
	// A little extra spacing because these are multi line
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Anti Latency Tools ***** //
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Anti Latency Tools"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClPredictionMargin, &g_Config.m_ClPredictionMargin, &Button, Localize("Prediction Margin"), 10, 75, &CUi::ms_LinearScrollbarScale, CUi::SCROLLBAR_OPTION_NOCLAMPVALUE, "ms");
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClRemoveAnti, Localize("Remove prediction & antiping in freeze"), &g_Config.m_ClRemoveAnti, &Column, LineSize);
	if(g_Config.m_ClRemoveAnti)
	{
		if(g_Config.m_ClUnfreezeLagDelayTicks < g_Config.m_ClUnfreezeLagTicks)
			g_Config.m_ClUnfreezeLagDelayTicks = g_Config.m_ClUnfreezeLagTicks;
		Column.HSplitTop(LineSize, &Button, &Column);
		DoSliderWithScaledValue(&g_Config.m_ClUnfreezeLagTicks, &g_Config.m_ClUnfreezeLagTicks, &Button, Localize("Amount"), 100, 300, 20, &CUi::ms_LinearScrollbarScale, CUi::SCROLLBAR_OPTION_NOCLAMPVALUE, "ms");
		Column.HSplitTop(LineSize, &Button, &Column);
		DoSliderWithScaledValue(&g_Config.m_ClUnfreezeLagDelayTicks, &g_Config.m_ClUnfreezeLagDelayTicks, &Button, Localize("Delay"), 100, 3000, 20, &CUi::ms_LinearScrollbarScale, CUi::SCROLLBAR_OPTION_NOCLAMPVALUE, "ms");
	}
	else
		Column.HSplitTop(LineSize * 2, nullptr, &Column);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClUnpredOthersInFreeze, Localize("Dont predict other players if you are frozen"), &g_Config.m_ClUnpredOthersInFreeze, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClPredMarginInFreeze, Localize("Adjust your prediction margin while frozen"), &g_Config.m_ClPredMarginInFreeze, &Column, LineSize);
	Column.HSplitTop(LineSize, &Button, &Column);
	if(g_Config.m_ClPredMarginInFreeze)
		Ui()->DoScrollbarOption(&g_Config.m_ClPredMarginInFreezeAmount, &g_Config.m_ClPredMarginInFreezeAmount, &Button, Localize("Frozen Margin"), 0, 100, &CUi::ms_LinearScrollbarScale, 0, "ms");
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Improved Anti Ping ***** //
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Anti Ping Smoothing"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClAntiPingImproved, Localize("Use new smoothing algorithm"), &g_Config.m_ClAntiPingImproved, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClAntiPingStableDirection, Localize("Optimistic prediction along stable direction"), &g_Config.m_ClAntiPingStableDirection, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClAntiPingNegativeBuffer, Localize("Negative stability buffer (for Gores)"), &g_Config.m_ClAntiPingNegativeBuffer, &Column, LineSize);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClAntiPingUncertaintyScale, &g_Config.m_ClAntiPingUncertaintyScale, &Button, Localize("Uncertainty duration"), 50, 400, &CUi::ms_LinearScrollbarScale, CUi::SCROLLBAR_OPTION_NOCLAMPVALUE, "%");
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Auto Reply ***** //
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Auto Reply"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClAutoReplyMinimized, Localize("Auto reply when tabbed out"), &g_Config.m_ClAutoReplyMinimized, &Column, LineSize);
	CUIRect MinimizedReply;
	Column.HSplitTop(LineSize + MarginExtraSmall, &MinimizedReply, &Column);
	if(g_Config.m_ClAutoReplyMinimized)
	{
		MinimizedReply.HSplitTop(MarginExtraSmall, nullptr, &MinimizedReply);
		static CLineInput s_MinimizedReply(g_Config.m_ClAutoReplyMinimizedMessage, sizeof(g_Config.m_ClAutoReplyMinimizedMessage));
		s_MinimizedReply.SetEmptyText("I am not tabbed in");
		Ui()->DoEditBox(&s_MinimizedReply, &MinimizedReply, EditBoxFontSize);
	}
	Column.HSplitTop(MarginExtraSmall, nullptr, &Column);
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Frozen Tee Display ***** //
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Frozen Tee Display"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClShowFrozenHud, Localize("Show frozen tee display"), &g_Config.m_ClShowFrozenHud, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClShowFrozenHudSkins, Localize("Use skins instead of ninja tees"), &g_Config.m_ClShowFrozenHudSkins, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClFrozenHudTeamOnly, Localize("Only show after joining a team"), &g_Config.m_ClFrozenHudTeamOnly, &Column, LineSize);

	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClFrozenMaxRows, &g_Config.m_ClFrozenMaxRows, &Button, Localize("Max Rows"), 1, 6);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClFrozenHudTeeSize, &g_Config.m_ClFrozenHudTeeSize, &Button, Localize("Tee Size"), 8, 27);

	{
		CUIRect CheckBoxRect, CheckBoxRect2;
		Column.HSplitTop(LineSize, &CheckBoxRect, &Column);
		Column.HSplitTop(LineSize, &CheckBoxRect2, &Column);
		if(DoButton_CheckBox(&g_Config.m_ClShowFrozenText, Localize("Tees left alive text"), g_Config.m_ClShowFrozenText >= 1, &CheckBoxRect))
			g_Config.m_ClShowFrozenText = g_Config.m_ClShowFrozenText >= 1 ? 0 : 1;

		if(g_Config.m_ClShowFrozenText)
		{
			static int s_CountFrozenText = 0;
			if(DoButton_CheckBox(&s_CountFrozenText, Localize("Count frozen tees"), g_Config.m_ClShowFrozenText == 2, &CheckBoxRect2))
				g_Config.m_ClShowFrozenText = g_Config.m_ClShowFrozenText != 2 ? 2 : 1;
		}
	}
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Pet ***** //
	/*
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Pet"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClPetShow, Localize("Show the pet"), &g_Config.m_ClPetShow, &Column, LineSize);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClPetSize, &g_Config.m_ClPetSize, &Button, Localize("Pet size"), 10, 500, &CUi::ms_LinearScrollbarScale, 0, "%");
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClPetAlpha, &g_Config.m_ClPetAlpha, &Button, Localize("Pet alpha"), 10, 100, &CUi::ms_LinearScrollbarScale, 0, "%");
	Column.HSplitTop(LineSize + MarginExtraSmall, &Button, &Column);
	Button.VSplitMid(&Label, &Button);
	Ui()->DoLabel(&Label, Localize("Pet Skin:"), FontSize, TEXTALIGN_ML);
	static CLineInput s_PetSkin(g_Config.m_ClPetSkin, sizeof(g_Config.m_ClPetSkin));
	Ui()->DoEditBox(&s_PetSkin, &Button, EditBoxFontSize);
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y; */
	// TODO: add preview

	// ***** RightView ***** //
	LeftView = Column;
	Column = RightView;

	// ***** HUD ***** //
	Column.HSplitTop(Margin, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("HUD"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClNotifyWhenLast, Localize("Show when you are the last alive"), &g_Config.m_ClNotifyWhenLast, &Column, LineSize);
	CUIRect NotificationConfig;
	Column.HSplitTop(LineSize + MarginSmall, &NotificationConfig, &Column);
	if(g_Config.m_ClNotifyWhenLast)
	{
		NotificationConfig.VSplitMid(&Button, &NotificationConfig);
		static CLineInput s_LastInput(g_Config.m_ClNotifyWhenLastText, sizeof(g_Config.m_ClNotifyWhenLastText));
		s_LastInput.SetEmptyText(Localize("Last!"));
		Button.HSplitTop(MarginSmall, nullptr, &Button);
		Ui()->DoEditBox(&s_LastInput, &Button, EditBoxFontSize);
		static CButtonContainer s_ClientNotifyWhenLastColor;
		DoLine_ColorPicker(&s_ClientNotifyWhenLastColor, ColorPickerLineSize, ColorPickerLabelSize, ColorPickerLineSpacing, &NotificationConfig, "", &g_Config.m_ClNotifyWhenLastColor, ColorRGBA(1.0f, 1.0f, 1.0f), false);
		Column.HSplitTop(LineSize, &Button, &Column);
		Ui()->DoScrollbarOption(&g_Config.m_ClNotifyWhenLastXpos, &g_Config.m_ClNotifyWhenLastXpos, &Button, Localize("Horizontal Position"), 1, 100);
		Column.HSplitTop(LineSize, &Button, &Column);
		Ui()->DoScrollbarOption(&g_Config.m_ClNotifyWhenLastYpos, &g_Config.m_ClNotifyWhenLastYpos, &Button, Localize("Vertical Position"), 1, 100);
		Column.HSplitTop(LineSize, &Button, &Column);
		Ui()->DoScrollbarOption(&g_Config.m_ClNotifyWhenLastSize, &g_Config.m_ClNotifyWhenLastSize, &Button, Localize("Font Size"), 1, 50);
	}
	else
	{
		Column.HSplitTop(LineSize * 3, nullptr, &Column);
	}
	Column.HSplitTop(MarginExtraSmall, nullptr, &Column);
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Tile Outlines ***** //
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Tile Outlines"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClOutline, Localize("Show any enabled outlines"), &g_Config.m_ClOutline, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClOutlineEntities, Localize("Only show outlines in entities"), &g_Config.m_ClOutlineEntities, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClOutlineFreeze, Localize("Outline freeze & deep"), &g_Config.m_ClOutlineFreeze, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClOutlineSolid, Localize("Outline walls"), &g_Config.m_ClOutlineSolid, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClOutlineTele, Localize("Outline teleporter"), &g_Config.m_ClOutlineTele, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClOutlineUnFreeze, Localize("Outline unfreeze & undeep"), &g_Config.m_ClOutlineUnFreeze, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClOutlineKill, Localize("Outline kill"), &g_Config.m_ClOutlineKill, &Column, LineSize);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClOutlineWidth, &g_Config.m_ClOutlineWidth, &Button, Localize("Outline width"), 1, 16);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClOutlineAlpha, &g_Config.m_ClOutlineAlpha, &Button, Localize("Outline alpha"), 0, 100);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClOutlineAlphaSolid, &g_Config.m_ClOutlineAlphaSolid, &Button, Localize("Outline Alpha (walls)"), 0, 100);
	static CButtonContainer s_OutlineColorFreezeId, s_OutlineColorSolidId, s_OutlineColorTeleId, s_OutlineColorUnfreezeId, s_OutlineColorKillId;
	DoLine_ColorPicker(&s_OutlineColorFreezeId, ColorPickerLineSize, ColorPickerLabelSize, ColorPickerLineSpacing, &Column, Localize("Freeze outline color"), &g_Config.m_ClOutlineColorFreeze, ColorRGBA(0.0f, 0.0f, 0.0f), false);
	DoLine_ColorPicker(&s_OutlineColorSolidId, ColorPickerLineSize, ColorPickerLabelSize, ColorPickerLineSpacing, &Column, Localize("Walls outline color"), &g_Config.m_ClOutlineColorSolid, ColorRGBA(0.0f, 0.0f, 0.0f), false);
	DoLine_ColorPicker(&s_OutlineColorTeleId, ColorPickerLineSize, ColorPickerLabelSize, ColorPickerLineSpacing, &Column, Localize("Teleporter outline color"), &g_Config.m_ClOutlineColorTele, ColorRGBA(0.0f, 0.0f, 0.0f), false);
	DoLine_ColorPicker(&s_OutlineColorUnfreezeId, ColorPickerLineSize, ColorPickerLabelSize, ColorPickerLineSpacing, &Column, Localize("Unfreeze outline color"), &g_Config.m_ClOutlineColorUnfreeze, ColorRGBA(0.0f, 0.0f, 0.0f), false);
	DoLine_ColorPicker(&s_OutlineColorKillId, ColorPickerLineSize, ColorPickerLabelSize, ColorPickerLineSpacing, &Column, Localize("Kill outline color"), &g_Config.m_ClOutlineColorKill, ColorRGBA(0.0f, 0.0f, 0.0f), false);
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Ghost Tools ***** //
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Ghost Tools"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClShowOthersGhosts, Localize("Show unpredicted ghosts for other players"), &g_Config.m_ClShowOthersGhosts, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClSwapGhosts, Localize("Swap ghosts and normal players"), &g_Config.m_ClSwapGhosts, &Column, LineSize);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClPredGhostsAlpha, &g_Config.m_ClPredGhostsAlpha, &Button, Localize("Predicted alpha"), 0, 100, &CUi::ms_LinearScrollbarScale, 0, "%");
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClUnpredGhostsAlpha, &g_Config.m_ClUnpredGhostsAlpha, &Button, Localize("Unpredicted alpha"), 0, 100, &CUi::ms_LinearScrollbarScale, 0, "%");
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClHideFrozenGhosts, Localize("Hide ghosts of frozen players"), &g_Config.m_ClHideFrozenGhosts, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClRenderGhostAsCircle, Localize("Render ghosts as circles"), &g_Config.m_ClRenderGhostAsCircle, &Column, LineSize);

	{
		static CKeyInfo s_Key = CKeyInfo{"Toggle ghosts key", "toggle tc_show_others_ghosts 0 1", 0, 0};
		s_Key.m_ModifierCombination = s_Key.m_KeyId = 0;
		for(int Mod = 0; Mod < CBinds::MODIFIER_COMBINATION_COUNT; Mod++)
		{
			for(int KeyId = 0; KeyId < KEY_LAST; KeyId++)
			{
				const char *pBind = m_pClient->m_Binds.Get(KeyId, Mod);
				if(!pBind[0])
					continue;

				if(str_comp(pBind, s_Key.m_pCommand) == 0)
				{
					s_Key.m_KeyId = KeyId;
					s_Key.m_ModifierCombination = Mod;
					break;
				}
			}
		}

		CUIRect KeyButton, KeyLabel;
		Column.HSplitTop(LineSize, &KeyButton, &Column);
		KeyButton.VSplitMid(&KeyLabel, &KeyButton);
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "%s:", Localize(s_Key.m_pName));
		Ui()->DoLabel(&KeyLabel, aBuf, 12.0f, TEXTALIGN_ML);
		int OldId = s_Key.m_KeyId, OldModifierCombination = s_Key.m_ModifierCombination, NewModifierCombination;
		int NewId = DoKeyReader(&s_Key, &KeyButton, OldId, OldModifierCombination, &NewModifierCombination);
		if(NewId != OldId || NewModifierCombination != OldModifierCombination)
		{
			if(OldId != 0 || NewId == 0)
				m_pClient->m_Binds.Bind(OldId, "", false, OldModifierCombination);
			if(NewId != 0)
				m_pClient->m_Binds.Bind(NewId, s_Key.m_pCommand, false, NewModifierCombination);
		}
		Column.HSplitTop(MarginExtraSmall, nullptr, &Column);
	}
	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;

	// ***** Tee Trails ***** //
	Column.HSplitTop(MarginBetweenSections, nullptr, &Column);
	s_SectionBoxes.push_back(Column);
	Column.HSplitTop(HeadlineHeight, &Label, &Column);
	Ui()->DoLabel(&Label, Localize("Tee Trails"), HeadlineFontSize, TEXTALIGN_ML);
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClTeeTrail, Localize("Enable tee trails"), &g_Config.m_ClTeeTrail, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClTeeTrailOthers, Localize("Show other tees' trails"), &g_Config.m_ClTeeTrailOthers, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClTeeTrailFade, Localize("Fade trail alpha"), &g_Config.m_ClTeeTrailFade, &Column, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClTeeTrailTaper, Localize("Taper trail width"), &g_Config.m_ClTeeTrailTaper, &Column, LineSize);

	Column.HSplitTop(MarginExtraSmall, nullptr, &Column);
	std::vector<const char *> s_TrailDropDownNames;
	s_TrailDropDownNames = {Localize("Solid"), Localize("Tee"), Localize("Rainbow"), Localize("Speed")};
	static CUi::SDropDownState s_TrailDropDownState;
	static CScrollRegion s_TrailDropDownScrollRegion;
	s_TrailDropDownState.m_SelectionPopupContext.m_pScrollRegion = &s_TrailDropDownScrollRegion;
	int TrailSelectedOld = g_Config.m_ClTeeTrailColorMode - 1;
	CUIRect TrailDropDownRect;
	Column.HSplitTop(LineSize, &TrailDropDownRect, &Column);
	const int TrailSelectedNew = Ui()->DoDropDown(&TrailDropDownRect, TrailSelectedOld, s_TrailDropDownNames.data(), s_TrailDropDownNames.size(), s_TrailDropDownState);
	if(TrailSelectedOld != TrailSelectedNew)
	{
		g_Config.m_ClTeeTrailColorMode = TrailSelectedNew + 1;
	}
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	static CButtonContainer s_TeeTrailColor;
	if(g_Config.m_ClTeeTrailColorMode == CTrails::COLORMODE_SOLID)
		DoLine_ColorPicker(&s_TeeTrailColor, ColorPickerLineSize, ColorPickerLabelSize, ColorPickerLineSpacing, &Column, Localize("Tee trail color"), &g_Config.m_ClTeeTrailColor, ColorRGBA(1.0f, 1.0f, 1.0f), false);
	else
		Column.HSplitTop(ColorPickerLineSize + ColorPickerLineSpacing, &Button, &Column);

	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClTeeTrailWidth, &g_Config.m_ClTeeTrailWidth, &Button, Localize("Trail width"), 0, 20);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClTeeTrailLength, &g_Config.m_ClTeeTrailLength, &Button, Localize("Trail length"), 0, 200);
	Column.HSplitTop(LineSize, &Button, &Column);
	Ui()->DoScrollbarOption(&g_Config.m_ClTeeTrailAlpha, &g_Config.m_ClTeeTrailAlpha, &Button, Localize("Trail alpha"), 0, 100);

	s_SectionBoxes.back().h = Column.y - s_SectionBoxes.back().y;
	Column.HSplitTop(MarginSmall, nullptr, &Column);

	// ***** END OF PAGE 1 SETTINGS ***** //
	RightView = Column;

	// Scroll
	CUIRect ScrollRegion;
	ScrollRegion.x = MainView.x;
	ScrollRegion.y = maximum(LeftView.y, RightView.y) + MarginSmall * 2.0f;
	ScrollRegion.w = MainView.w;
	ScrollRegion.h = 0.0f;
	s_ScrollRegion.AddRect(ScrollRegion);
	s_ScrollRegion.End();
}

void CMenus::RenderSettingsInfo(CUIRect MainView)
{
	CUIRect LeftView, RightView, Button, Label, LowerLeftView;
	MainView.HSplitTop(MarginSmall, nullptr, &MainView);

	MainView.VSplitMid(&LeftView, &RightView, MarginBetweenViews);
	LeftView.VSplitLeft(MarginSmall, nullptr, &LeftView);
	RightView.VSplitRight(MarginSmall, &RightView, nullptr);
	LeftView.HSplitMid(&LeftView, &LowerLeftView, 0.0f);

	LeftView.HSplitTop(HeadlineHeight, &Label, &LeftView);
	Ui()->DoLabel(&Label, Localize("Nothing here"), HeadlineFontSize, TEXTALIGN_ML);

	LeftView = LowerLeftView;
	LeftView.HSplitBottom(LineSize * 2.0f + MarginSmall * 2.0f + HeadlineFontSize, nullptr, &LeftView);
	LeftView.HSplitTop(HeadlineHeight, &Label, &LeftView);
	Ui()->DoLabel(&Label, Localize("Config Files"), HeadlineFontSize, TEXTALIGN_ML);
	LeftView.HSplitTop(MarginSmall, nullptr, &LeftView);

	char aBuf[128 + IO_MAX_PATH_LENGTH];
	CUIRect TClientConfig, ProfilesFile;

	LeftView.HSplitTop(LineSize * 2.0f, &Button, &LeftView);
	Button.VSplitMid(&TClientConfig, &ProfilesFile, MarginSmall);

	static CButtonContainer s_Config, s_Profiles;
	if(DoButtonLineSize_Menu(&s_Config, Localize("Alesstya Settings"), 0, &TClientConfig, LineSize, false, 0, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)))
	{
		Storage()->GetCompletePath(IStorage::TYPE_SAVE, s_aConfigDomains[ConfigDomain::TCLIENT].m_aConfigPath, aBuf, sizeof(aBuf));
		Client()->ViewFile(aBuf);
	}
	if(DoButtonLineSize_Menu(&s_Profiles, Localize("Profiles"), 0, &ProfilesFile, LineSize, false, 0, IGraphics::CORNER_ALL, 5.0f, 0.0f, ColorRGBA(0.0f, 0.0f, 0.0f, 0.25f)))
	{
		Storage()->GetCompletePath(IStorage::TYPE_SAVE, s_aConfigDomains[ConfigDomain::TCLIENTPROFILES].m_aConfigPath, aBuf, sizeof(aBuf));
		Client()->ViewFile(aBuf);
	}
}

void CMenus::RenderSettingsProfiles(CUIRect MainView)
{
	CUIRect Label, LabelMid, Section, LabelRight;
	static int s_SelectedProfile = -1;

	char *pSkinName = g_Config.m_ClPlayerSkin;
	int *pUseCustomColor = &g_Config.m_ClPlayerUseCustomColor;
	unsigned *pColorBody = &g_Config.m_ClPlayerColorBody;
	unsigned *pColorFeet = &g_Config.m_ClPlayerColorFeet;
	int CurrentFlag = m_Dummy ? g_Config.m_ClDummyCountry : g_Config.m_PlayerCountry;

	if(m_Dummy)
	{
		pSkinName = g_Config.m_ClDummySkin;
		pUseCustomColor = &g_Config.m_ClDummyUseCustomColor;
		pColorBody = &g_Config.m_ClDummyColorBody;
		pColorFeet = &g_Config.m_ClDummyColorFeet;
	}

	// skin info
	CTeeRenderInfo OwnSkinInfo;
	const CSkin *pSkin = m_pClient->m_Skins.Find(pSkinName);
	OwnSkinInfo.m_OriginalRenderSkin = pSkin->m_OriginalSkin;
	OwnSkinInfo.m_ColorableRenderSkin = pSkin->m_ColorableSkin;
	OwnSkinInfo.m_SkinMetrics = pSkin->m_Metrics;
	OwnSkinInfo.m_CustomColoredSkin = *pUseCustomColor;
	if(*pUseCustomColor)
	{
		OwnSkinInfo.m_ColorBody = color_cast<ColorRGBA>(ColorHSLA(*pColorBody).UnclampLighting(ColorHSLA::DARKEST_LGT));
		OwnSkinInfo.m_ColorFeet = color_cast<ColorRGBA>(ColorHSLA(*pColorFeet).UnclampLighting(ColorHSLA::DARKEST_LGT));
	}
	else
	{
		OwnSkinInfo.m_ColorBody = ColorRGBA(1.0f, 1.0f, 1.0f);
		OwnSkinInfo.m_ColorFeet = ColorRGBA(1.0f, 1.0f, 1.0f);
	}
	OwnSkinInfo.m_Size = 50.0f;

	//======YOUR PROFILE======
	char aTempBuf[256];
	str_format(aTempBuf, sizeof(aTempBuf), "%s:", Localize("Your profile"));
	MainView.HSplitTop(LineSize, &Label, &MainView);
	Ui()->DoLabel(&Label, aTempBuf, FontSize, TEXTALIGN_ML);
	MainView.HSplitTop(MarginSmall, nullptr, &MainView);

	MainView.HSplitTop(50.0f, &Label, &MainView);
	Label.VSplitLeft(250.0f, &Label, &LabelMid);
	const CAnimState *pIdleState = CAnimState::GetIdle();
	vec2 OffsetToMid;
	CRenderTools::GetRenderTeeOffsetToRenderedTee(pIdleState, &OwnSkinInfo, OffsetToMid);
	vec2 TeeRenderPos(Label.x + LineSize, Label.y + Label.h / 2.0f + OffsetToMid.y);
	int Emote = m_Dummy ? g_Config.m_ClDummyDefaultEyes : g_Config.m_ClPlayerDefaultEyes;
	RenderTools()->RenderTee(pIdleState, &OwnSkinInfo, Emote, vec2(1.0f, 0.0f), TeeRenderPos);

	char aName[64];
	char aClan[64];
	str_format(aName, sizeof(aName), "%s", m_Dummy ? g_Config.m_ClDummyName : g_Config.m_PlayerName);
	str_format(aClan, sizeof(aClan), "%s", m_Dummy ? g_Config.m_ClDummyClan : g_Config.m_PlayerClan);

	CUIRect FlagRect;
	Label.VSplitLeft(90.0f, &FlagRect, &Label);

	Label.HSplitTop(LineSize, &Section, &Label);
	str_format(aTempBuf, sizeof(aTempBuf), Localize("Name: %s"), aName);
	Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

	Label.HSplitTop(LineSize, &Section, &Label);
	str_format(aTempBuf, sizeof(aTempBuf), Localize("Clan: %s"), aClan);
	Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

	Label.HSplitTop(LineSize, &Section, &Label);
	str_format(aTempBuf, sizeof(aTempBuf), Localize("Skin: %s"), pSkinName);
	Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

	FlagRect.VSplitRight(50.0f, nullptr, &FlagRect);
	FlagRect.HSplitBottom(25.0f, nullptr, &FlagRect);
	FlagRect.y -= 10.0f;
	ColorRGBA Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_pClient->m_CountryFlags.Render(m_Dummy ? g_Config.m_ClDummyCountry : g_Config.m_PlayerCountry, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

	bool DoSkin = g_Config.m_ClApplyProfileSkin;
	bool DoColors = g_Config.m_ClApplyProfileColors;
	bool DoEmote = g_Config.m_ClApplyProfileEmote;
	bool DoName = g_Config.m_ClApplyProfileName;
	bool DoClan = g_Config.m_ClApplyProfileClan;
	bool DoFlag = g_Config.m_ClApplyProfileFlag;

	// After load
	if(s_SelectedProfile != -1 && s_SelectedProfile < (int)GameClient()->m_SkinProfiles.m_Profiles.size())
	{
		CProfile LoadProfile = GameClient()->m_SkinProfiles.m_Profiles[s_SelectedProfile];
		MainView.HSplitTop(LineSize, nullptr, &MainView);
		MainView.HSplitTop(10.0f, &Label, &MainView);
		str_format(aTempBuf, sizeof(aTempBuf), "%s:", Localize("After Load"));
		Ui()->DoLabel(&Label, aTempBuf, FontSize, TEXTALIGN_ML);

		MainView.HSplitTop(50.0f, &Label, &MainView);
		Label.VSplitLeft(250.0f, &Label, nullptr);

		if(DoSkin && strlen(LoadProfile.m_SkinName) != 0)
		{
			const CSkin *pLoadSkin = m_pClient->m_Skins.Find(LoadProfile.m_SkinName);
			OwnSkinInfo.m_OriginalRenderSkin = pLoadSkin->m_OriginalSkin;
			OwnSkinInfo.m_ColorableRenderSkin = pLoadSkin->m_ColorableSkin;
			OwnSkinInfo.m_SkinMetrics = pLoadSkin->m_Metrics;
		}
		if(*pUseCustomColor && DoColors && LoadProfile.m_BodyColor != -1 && LoadProfile.m_FeetColor != -1)
		{
			OwnSkinInfo.m_ColorBody = color_cast<ColorRGBA>(ColorHSLA(LoadProfile.m_BodyColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
			OwnSkinInfo.m_ColorFeet = color_cast<ColorRGBA>(ColorHSLA(LoadProfile.m_FeetColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
		}

		CRenderTools::GetRenderTeeOffsetToRenderedTee(pIdleState, &OwnSkinInfo, OffsetToMid);
		TeeRenderPos = vec2(Label.x + LineSize, Label.y + Label.h / 2.0f + OffsetToMid.y);
		int LoadEmote = Emote;
		if(DoEmote && LoadProfile.m_Emote != -1)
			LoadEmote = LoadProfile.m_Emote;
		RenderTools()->RenderTee(pIdleState, &OwnSkinInfo, LoadEmote, vec2(1.0f, 0.0f), TeeRenderPos);

		if(DoName && strlen(LoadProfile.m_Name) != 0)
			str_format(aName, sizeof(aName), "%s", LoadProfile.m_Name);
		if(DoClan && strlen(LoadProfile.m_Clan) != 0)
			str_format(aClan, sizeof(aClan), "%s", LoadProfile.m_Clan);

		Label.VSplitLeft(90.0f, &FlagRect, &Label);

		Label.HSplitTop(LineSize, &Section, &Label);
		str_format(aTempBuf, sizeof(aTempBuf), Localize("Name: %s"), aName);
		Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

		Label.HSplitTop(LineSize, &Section, &Label);
		str_format(aTempBuf, sizeof(aTempBuf), Localize("Clan: %s"), aClan);
		Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

		Label.HSplitTop(LineSize, &Section, &Label);
		str_format(aTempBuf, sizeof(aTempBuf), Localize("Skin: %s"), (DoSkin && strlen(LoadProfile.m_SkinName) != 0) ? LoadProfile.m_SkinName : pSkinName);
		Ui()->DoLabel(&Section, aTempBuf, FontSize, TEXTALIGN_ML);

		FlagRect.VSplitRight(50.0f, nullptr, &FlagRect);
		FlagRect.HSplitBottom(25.0f, nullptr, &FlagRect);
		FlagRect.y -= 10.0f;
		int RenderFlag = m_Dummy ? g_Config.m_ClDummyCountry : g_Config.m_PlayerCountry;
		if(DoFlag && LoadProfile.m_CountryFlag != -2)
			RenderFlag = LoadProfile.m_CountryFlag;
		m_pClient->m_CountryFlags.Render(RenderFlag, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

		str_format(aName, sizeof(aName), "%s", m_Dummy ? g_Config.m_ClDummyName : g_Config.m_PlayerName);
		str_format(aClan, sizeof(aClan), "%s", m_Dummy ? g_Config.m_ClDummyClan : g_Config.m_PlayerClan);
	}
	else
	{
		MainView.HSplitTop(80.0f, nullptr, &MainView);
	}

	//===BUTTONS AND CHECK BOX===
	CUIRect DummyCheck, CustomCheck;
	MainView.HSplitTop(30.0f, &DummyCheck, nullptr);
	DummyCheck.HSplitTop(13.0f, nullptr, &DummyCheck);

	DummyCheck.VSplitLeft(100.0f, &DummyCheck, &CustomCheck);
	CustomCheck.VSplitLeft(150.0f, &CustomCheck, nullptr);

	DoButton_CheckBoxAutoVMarginAndSet(&m_Dummy, Localize("Dummy"), (int *)&m_Dummy, &DummyCheck, LineSize);

	static int s_CustomColorId = 0;
	CustomCheck.HSplitTop(LineSize, &CustomCheck, nullptr);

	if(DoButton_CheckBox(&s_CustomColorId, Localize("Custom colors"), *pUseCustomColor, &CustomCheck))
	{
		*pUseCustomColor = *pUseCustomColor ? 0 : 1;
		SetNeedSendInfo();
	}

	LabelMid.VSplitLeft(20.0f, nullptr, &LabelMid);
	LabelMid.VSplitLeft(160.0f, &LabelMid, &LabelRight);

	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileSkin, Localize("Save/Load Skin"), &g_Config.m_ClApplyProfileSkin, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileColors, Localize("Save/Load Colors"), &g_Config.m_ClApplyProfileColors, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileEmote, Localize("Save/Load Emote"), &g_Config.m_ClApplyProfileEmote, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileName, Localize("Save/Load Name"), &g_Config.m_ClApplyProfileName, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileClan, Localize("Save/Load Clan"), &g_Config.m_ClApplyProfileClan, &LabelMid, LineSize);
	DoButton_CheckBoxAutoVMarginAndSet(&g_Config.m_ClApplyProfileFlag, Localize("Save/Load Flag"), &g_Config.m_ClApplyProfileFlag, &LabelMid, LineSize);

	CUIRect Button;
	LabelRight.VSplitLeft(150.0f, &LabelRight, nullptr);

	LabelRight.HSplitTop(30.0f, &Button, &LabelRight);
	static CButtonContainer s_LoadButton;

	if(DoButton_Menu(&s_LoadButton, Localize("Load"), 0, &Button))
	{
		if(s_SelectedProfile != -1 && s_SelectedProfile < (int)GameClient()->m_SkinProfiles.m_Profiles.size())
		{
			CProfile LoadProfile = GameClient()->m_SkinProfiles.m_Profiles[s_SelectedProfile];
			if(!m_Dummy)
			{
				if(DoSkin && strlen(LoadProfile.m_SkinName) != 0)
					str_copy(g_Config.m_ClPlayerSkin, LoadProfile.m_SkinName, sizeof(g_Config.m_ClPlayerSkin));
				if(DoColors && LoadProfile.m_BodyColor != -1 && LoadProfile.m_FeetColor != -1)
				{
					g_Config.m_ClPlayerColorBody = LoadProfile.m_BodyColor;
					g_Config.m_ClPlayerColorFeet = LoadProfile.m_FeetColor;
				}
				if(DoEmote && LoadProfile.m_Emote != -1)
					g_Config.m_ClPlayerDefaultEyes = LoadProfile.m_Emote;
				if(DoName && strlen(LoadProfile.m_Name) != 0)
					str_copy(g_Config.m_PlayerName, LoadProfile.m_Name, sizeof(g_Config.m_PlayerName));
				if(DoClan && strlen(LoadProfile.m_Clan) != 0)
					str_copy(g_Config.m_PlayerClan, LoadProfile.m_Clan, sizeof(g_Config.m_PlayerClan));
				if(DoFlag && LoadProfile.m_CountryFlag != -2)
					g_Config.m_PlayerCountry = LoadProfile.m_CountryFlag;
			}
			else
			{
				if(DoSkin && strlen(LoadProfile.m_SkinName) != 0)
					str_copy(g_Config.m_ClDummySkin, LoadProfile.m_SkinName, sizeof(g_Config.m_ClDummySkin));
				if(DoColors && LoadProfile.m_BodyColor != -1 && LoadProfile.m_FeetColor != -1)
				{
					g_Config.m_ClDummyColorBody = LoadProfile.m_BodyColor;
					g_Config.m_ClDummyColorFeet = LoadProfile.m_FeetColor;
				}
				if(DoEmote && LoadProfile.m_Emote != -1)
					g_Config.m_ClDummyDefaultEyes = LoadProfile.m_Emote;
				if(DoName && strlen(LoadProfile.m_Name) != 0)
					str_copy(g_Config.m_ClDummyName, LoadProfile.m_Name, sizeof(g_Config.m_ClDummyName));
				if(DoClan && strlen(LoadProfile.m_Clan) != 0)
					str_copy(g_Config.m_ClDummyClan, LoadProfile.m_Clan, sizeof(g_Config.m_ClDummyClan));
				if(DoFlag && LoadProfile.m_CountryFlag != -2)
					g_Config.m_ClDummyCountry = LoadProfile.m_CountryFlag;
			}
		}
		SetNeedSendInfo();
	}
	LabelRight.HSplitTop(5.0f, nullptr, &LabelRight);

	LabelRight.HSplitTop(30.0f, &Button, &LabelRight);
	static CButtonContainer s_SaveButton;
	if(DoButton_Menu(&s_SaveButton, Localize("Save"), 0, &Button))
	{
		GameClient()->m_SkinProfiles.AddProfile(
			DoColors ? *pColorBody : -1,
			DoColors ? *pColorFeet : -1,
			DoFlag ? CurrentFlag : -2,
			DoEmote ? Emote : -1,
			DoSkin ? pSkinName : "",
			DoName ? aName : "",
			DoClan ? aClan : "");
	}
	LabelRight.HSplitTop(5.0f, nullptr, &LabelRight);

	static int s_AllowDelete;
	DoButton_CheckBoxAutoVMarginAndSet(&s_AllowDelete, Localizable("Enable Deleting"), &s_AllowDelete, &LabelRight, LineSize);
	LabelRight.HSplitTop(5.0f, nullptr, &LabelRight);

	if(s_AllowDelete)
	{
		LabelRight.HSplitTop(28.0f, &Button, &LabelRight);
		static CButtonContainer s_DeleteButton;
		if(DoButton_Menu(&s_DeleteButton, Localize("Delete"), 0, &Button))
			if(s_SelectedProfile != -1 && s_SelectedProfile < (int)GameClient()->m_SkinProfiles.m_Profiles.size())
				GameClient()->m_SkinProfiles.m_Profiles.erase(GameClient()->m_SkinProfiles.m_Profiles.begin() + s_SelectedProfile);
		LabelRight.HSplitTop(5.0f, nullptr, &LabelRight);

		LabelRight.HSplitTop(28.0f, &Button, &LabelRight);
		static CButtonContainer s_OverrideButton;
		if(DoButton_Menu(&s_OverrideButton, Localize("Override"), 0, &Button))
		{
			if(s_SelectedProfile != -1 && s_SelectedProfile < (int)GameClient()->m_SkinProfiles.m_Profiles.size())
			{
				GameClient()->m_SkinProfiles.m_Profiles[s_SelectedProfile] = CProfile(
					DoColors ? *pColorBody : -1,
					DoColors ? *pColorFeet : -1,
					DoFlag ? CurrentFlag : -2,
					DoEmote ? Emote : -1,
					DoSkin ? pSkinName : "",
					DoName ? aName : "",
					DoClan ? aClan : "");
			}
		}
	}

	//---RENDER THE SELECTOR---
	CUIRect FileButton;
	CUIRect SelectorRect;
	MainView.HSplitTop(50.0f, nullptr, &SelectorRect);
	SelectorRect.HSplitBottom(LineSize, &SelectorRect, &FileButton);
	SelectorRect.HSplitBottom(MarginSmall, &SelectorRect, nullptr);
	std::vector<CProfile> *pProfileList = &GameClient()->m_SkinProfiles.m_Profiles;

	static CListBox s_ListBox;
	s_ListBox.DoStart(50.0f, pProfileList->size(), 4, 3, s_SelectedProfile, &SelectorRect, true);

	static bool s_Indexs[1024];

	for(size_t i = 0; i < pProfileList->size(); ++i)
	{
		CProfile CurrentProfile = GameClient()->m_SkinProfiles.m_Profiles[i];

		char RenderSkin[24];
		if(strlen(CurrentProfile.m_SkinName) == 0)
			str_copy(RenderSkin, pSkinName, sizeof(RenderSkin));
		else
			str_copy(RenderSkin, CurrentProfile.m_SkinName, sizeof(RenderSkin));

		const CSkin *pSkinToBeDraw = m_pClient->m_Skins.Find(RenderSkin);

		CListboxItem Item = s_ListBox.DoNextItem(&s_Indexs[i], s_SelectedProfile >= 0 && (size_t)s_SelectedProfile == i);

		if(!Item.m_Visible)
			continue;

		if(Item.m_Visible)
		{
			CTeeRenderInfo Info;
			Info.m_ColorBody = color_cast<ColorRGBA>(ColorHSLA(CurrentProfile.m_BodyColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
			Info.m_ColorFeet = color_cast<ColorRGBA>(ColorHSLA(CurrentProfile.m_FeetColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
			Info.m_CustomColoredSkin = true;
			Info.m_OriginalRenderSkin = pSkinToBeDraw->m_OriginalSkin;
			Info.m_ColorableRenderSkin = pSkinToBeDraw->m_ColorableSkin;
			Info.m_SkinMetrics = pSkinToBeDraw->m_Metrics;
			Info.m_Size = 50.0f;
			if(CurrentProfile.m_BodyColor == -1 && CurrentProfile.m_FeetColor == -1)
			{
				Info.m_CustomColoredSkin = m_Dummy ? g_Config.m_ClDummyUseCustomColor : g_Config.m_ClPlayerUseCustomColor;
				Info.m_ColorBody = ColorRGBA(1.0f, 1.0f, 1.0f);
				Info.m_ColorFeet = ColorRGBA(1.0f, 1.0f, 1.0f);
			}

			CRenderTools::GetRenderTeeOffsetToRenderedTee(pIdleState, &Info, OffsetToMid);

			int RenderEmote = CurrentProfile.m_Emote == -1 ? Emote : CurrentProfile.m_Emote;
			TeeRenderPos = vec2(Item.m_Rect.x + 30.0f, Item.m_Rect.y + Item.m_Rect.h / 2.0f + OffsetToMid.y);

			Item.m_Rect.VSplitLeft(60.0f, nullptr, &Item.m_Rect);
			CUIRect PlayerRect, ClanRect, FeetColorSquare, BodyColorSquare;

			Item.m_Rect.VSplitLeft(60.0f, nullptr, &BodyColorSquare); // Delete this maybe

			Item.m_Rect.VSplitRight(60.0f, &BodyColorSquare, &FlagRect);
			BodyColorSquare.x -= 11.0f;
			BodyColorSquare.VSplitLeft(10.0f, &BodyColorSquare, nullptr);
			BodyColorSquare.HSplitMid(&BodyColorSquare, &FeetColorSquare);
			BodyColorSquare.HSplitMid(nullptr, &BodyColorSquare);
			FeetColorSquare.HSplitMid(&FeetColorSquare, nullptr);
			FlagRect.HSplitBottom(10.0f, &FlagRect, nullptr);
			FlagRect.HSplitTop(10.0f, nullptr, &FlagRect);

			Item.m_Rect.HSplitMid(&PlayerRect, &ClanRect);

			SLabelProperties Props;
			Props.m_MaxWidth = Item.m_Rect.w;
			if(CurrentProfile.m_CountryFlag != -2)
				m_pClient->m_CountryFlags.Render(CurrentProfile.m_CountryFlag, Color, FlagRect.x, FlagRect.y, FlagRect.w, FlagRect.h);

			if(CurrentProfile.m_BodyColor != -1 && CurrentProfile.m_FeetColor != -1)
			{
				ColorRGBA BodyColor = color_cast<ColorRGBA>(ColorHSLA(CurrentProfile.m_BodyColor).UnclampLighting(ColorHSLA::DARKEST_LGT));
				ColorRGBA FeetColor = color_cast<ColorRGBA>(ColorHSLA(CurrentProfile.m_FeetColor).UnclampLighting(ColorHSLA::DARKEST_LGT));

				Graphics()->TextureClear();
				Graphics()->QuadsBegin();
				Graphics()->SetColor(BodyColor.r, BodyColor.g, BodyColor.b, 1.0f);
				IGraphics::CQuadItem Quads[2];
				Quads[0] = IGraphics::CQuadItem(BodyColorSquare.x, BodyColorSquare.y, BodyColorSquare.w, BodyColorSquare.h);
				Graphics()->QuadsDrawTL(&Quads[0], 1);
				Graphics()->SetColor(FeetColor.r, FeetColor.g, FeetColor.b, 1.0f);
				Quads[1] = IGraphics::CQuadItem(FeetColorSquare.x, FeetColorSquare.y, FeetColorSquare.w, FeetColorSquare.h);
				Graphics()->QuadsDrawTL(&Quads[1], 1);
				Graphics()->QuadsEnd();
			}
			RenderTools()->RenderTee(pIdleState, &Info, RenderEmote, vec2(1.0f, 0.0f), TeeRenderPos);

			if(strlen(CurrentProfile.m_Name) == 0 && strlen(CurrentProfile.m_Clan) == 0)
			{
				PlayerRect = Item.m_Rect;
				PlayerRect.y += MarginSmall;
				Ui()->DoLabel(&PlayerRect, CurrentProfile.m_SkinName, FontSize, TEXTALIGN_ML, Props);
			}
			else
			{
				Ui()->DoLabel(&PlayerRect, CurrentProfile.m_Name, FontSize, TEXTALIGN_ML, Props);
				Item.m_Rect.HSplitTop(LineSize, nullptr, &Item.m_Rect);
				Props.m_MaxWidth = Item.m_Rect.w;
				Ui()->DoLabel(&ClanRect, CurrentProfile.m_Clan, FontSize, TEXTALIGN_ML, Props);
			}
		}
	}

	const int NewSelected = s_ListBox.DoEnd();
	if(s_SelectedProfile != NewSelected)
	{
		s_SelectedProfile = NewSelected;
	}
	static CButtonContainer s_ProfilesFile;
	FileButton.VSplitLeft(130.0f, &FileButton, nullptr);
	if(DoButton_Menu(&s_ProfilesFile, Localize("Profiles file"), 0, &FileButton))
	{
		Storage()->GetCompletePath(IStorage::TYPE_SAVE, s_aConfigDomains[ConfigDomain::TCLIENTPROFILES].m_aConfigPath, aTempBuf, sizeof(aTempBuf));
		Client()->ViewFile(aTempBuf);
	}
}

void CMenus::RenderFontIcon(const CUIRect Rect, const char *pText, float Size, int Align)
{
	TextRender()->SetFontPreset(EFontPreset::ICON_FONT);
	TextRender()->SetRenderFlags(ETextRenderFlags::TEXT_RENDER_FLAG_ONLY_ADVANCE_WIDTH | ETextRenderFlags::TEXT_RENDER_FLAG_NO_X_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_Y_BEARING);
	Ui()->DoLabel(&Rect, pText, Size, Align);
	TextRender()->SetRenderFlags(0);
	TextRender()->SetFontPreset(EFontPreset::DEFAULT_FONT);
}

int CMenus::DoButtonNoRect_FontIcon(CButtonContainer *pButtonContainer, const char *pText, int Checked, const CUIRect *pRect, int Corners)
{
	TextRender()->SetFontPreset(EFontPreset::ICON_FONT);
	TextRender()->SetRenderFlags(ETextRenderFlags::TEXT_RENDER_FLAG_ONLY_ADVANCE_WIDTH | ETextRenderFlags::TEXT_RENDER_FLAG_NO_X_BEARING | ETextRenderFlags::TEXT_RENDER_FLAG_NO_Y_BEARING);
	TextRender()->TextOutlineColor(TextRender()->DefaultTextOutlineColor());
	TextRender()->TextColor(TextRender()->DefaultTextSelectionColor());
	if(Ui()->HotItem() == pButtonContainer)
	{
		TextRender()->TextColor(TextRender()->DefaultTextColor());
	}
	CUIRect Temp;
	pRect->HMargin(0.0f, &Temp);
	Ui()->DoLabel(&Temp, pText, Temp.h * CUi::ms_FontmodHeight, TEXTALIGN_MC);
	TextRender()->SetRenderFlags(0);
	TextRender()->SetFontPreset(EFontPreset::DEFAULT_FONT);

	return Ui()->DoButtonLogic(pButtonContainer, Checked, pRect, BUTTONFLAG_LEFT);
}
