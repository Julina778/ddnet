// This file can be included several times.

#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc) ;
#endif

//MACRO_CONFIG_INT(ClRunOnJoinConsole, al_run_on_join_console, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Whether to use run on join in chat or console")
//MACRO_CONFIG_INT(ClRunOnJoinDelay, al_run_on_join_delay, 2, 7, 50000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tick Delay before using run on join")

MACRO_CONFIG_INT(ClShowFrozenText, al_frozen_tees_text, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show how many tees in your team are currently frozen. (0 - off, 1 - show alive, 2 - show frozen)")
MACRO_CONFIG_INT(ClShowFrozenHud, al_frozen_tees_hud, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show frozen tee HUD")
MACRO_CONFIG_INT(ClShowFrozenHudSkins, al_frozen_tees_hud_skins, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Use ninja skin, or darkened skin for frozen tees on hud")

MACRO_CONFIG_INT(ClFrozenHudTeeSize, al_frozen_tees_size, 15, 8, 20, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of tees in frozen tee hud. (Default : 15)")
MACRO_CONFIG_INT(ClFrozenMaxRows, al_frozen_tees_max_rows, 1, 1, 6, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Maximum number of rows in frozen tee HUD display")
MACRO_CONFIG_INT(ClFrozenHudTeamOnly, al_frozen_tees_only_inteam, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Only render frozen tee HUD display while in team")

MACRO_CONFIG_INT(ClPingNameCircle, al_nameplate_ping_circle, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Shows a circle next to nameplate to indicate ping")

//MACRO_CONFIG_INT(ClSpecmenuID, al_spec_menu_ID, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Shows player IDs in spectate menu")

//MACRO_CONFIG_INT(ClLimitMouseToScreen, al_limit_mouse_to_screen, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Limit mouse to screen boundries")
//MACRO_CONFIG_INT(ClScaleMouseDistance, al_scale_mouse_distance, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Improve mouse precision by scaling max distance to 1000")

//MACRO_CONFIG_INT(ClHammerRotatesWithCursor, al_hammer_rotates_with_cursor, 0, 0, 2, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Allow your hammer to rotate like other weapons")

// Anti Latency Tools
MACRO_CONFIG_INT(ClFreezeUpdateFix, al_freeze_update_fix, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Will change your skin faster when you enter freeze. ")
MACRO_CONFIG_INT(ClRemoveAnti, al_remove_anti, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Removes some amount of antiping & player prediction in freeze")
MACRO_CONFIG_INT(ClUnfreezeLagTicks, al_remove_anti_ticks, 5, 0, 20, CFGFLAG_CLIENT | CFGFLAG_SAVE, "The biggest amount of prediction ticks that are removed")
MACRO_CONFIG_INT(ClUnfreezeLagDelayTicks, al_remove_anti_delay_ticks, 25, 5, 150, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How many ticks it takes to remove the maximum prediction after being frozen")

MACRO_CONFIG_INT(ClUnpredOthersInFreeze, al_unpred_others_in_freeze, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Dont predict other players if you are frozen")
MACRO_CONFIG_INT(ClPredMarginInFreeze, al_pred_margin_in_freeze, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "enable changing prediction margin while frozen")
MACRO_CONFIG_INT(ClPredMarginInFreezeAmount, al_pred_margin_in_freeze_amount, 15, 0, 2000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Set what your prediction margin while frozen should be")

MACRO_CONFIG_INT(ClShowOthersGhosts, al_show_others_ghosts, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show ghosts for other players in their unpredicted position")
MACRO_CONFIG_INT(ClSwapGhosts, al_swap_ghosts, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show predicted players as ghost and normal players as unpredicted")
MACRO_CONFIG_INT(ClHideFrozenGhosts, al_hide_frozen_ghosts, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Hide Ghosts of other players if they are frozen")

MACRO_CONFIG_INT(ClPredGhostsAlpha, al_pred_ghosts_alpha, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Alpha of predicted ghosts (0-100)")
MACRO_CONFIG_INT(ClUnpredGhostsAlpha, al_unpred_ghosts_alpha, 50, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Alpha of unpredicted ghosts (0-100)")
MACRO_CONFIG_INT(ClRenderGhostAsCircle, al_render_ghost_as_circle, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render Ghosts as circles instead of tee")

// MACRO_CONFIG_INT(ClHookLineSize, al_hook_line_width, 0, 0, 20, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Adjustable hookline width, set to 0 for old default rendering")

//MACRO_CONFIG_INT(ClShowCenterLines, al_show_center, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Draws lines to show the center of your screen/hitbox")
MACRO_CONFIG_INT(ClShowSkinName, al_skin_name, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Shows skin names in nameplates, good for finding missing skins")

// MACRO_CONFIG_INT(ClFreeGhost, al_freeghost, 0, 0, 1, CFGFLAG_CLIENT , "")

MACRO_CONFIG_INT(ClFastInput, al_fast_input, 0, 0, 5, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Uses input for prediction up to 20ms faster")
MACRO_CONFIG_INT(ClFastInputOthers, al_fast_input_others, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Do an extra 1 tick (20ms) for other tees with your fast inputs. (increases visual latency, makes dragging easier)")

MACRO_CONFIG_INT(ClAntiPingImproved, al_antiping_improved, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Different antiping smoothing algorithm, not compatible with cl_antiping_smooth")
MACRO_CONFIG_INT(ClAntiPingNegativeBuffer, al_antiping_negative_buffer, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Helps in Gores. Allows internal certainty value to be negative which causes more conservative prediction")
MACRO_CONFIG_INT(ClAntiPingStableDirection, al_antiping_stable_direction, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Predicts optimistically along the tees stable axis to reduce delay in gaining overall stability")
MACRO_CONFIG_INT(ClAntiPingUncertaintyScale, al_antiping_uncertainty_scale, 150, 25, 400, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Determines uncertainty duration as a factor of ping, 100 = 1.0")

//MACRO_CONFIG_INT(ClColorFreeze, al_color_freeze, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Use skin colors for frozen tees")
//MACRO_CONFIG_INT(ClColorFreezeDarken, al_color_freeze_darken, 90, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Makes color of tees darker when in freeze (0-100)")
//MACRO_CONFIG_INT(ClColorFreezeFeet, al_color_freeze_feet, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Also use color for frozen tee feet")

// Revert Variables
MACRO_CONFIG_INT(ClSmoothPredictionMargin, al_prediction_margin_smooth, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Makes prediction margin transition smooth, causes worse ping jitter adjustment (reverts a ddnet change)")
// MACRO_CONFIG_INT(ClFreezeStars, al_freeze_stars, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show old freeze stars")

// Outline Variables
MACRO_CONFIG_INT(ClOutline, al_outline, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Draws outlines")
MACRO_CONFIG_INT(ClOutlineEntities, al_outline_in_entities, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Only show outlines in entities")
MACRO_CONFIG_INT(ClOutlineFreeze, al_outline_freeze, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Draws outline around freeze and deep")
MACRO_CONFIG_INT(ClOutlineKill, al_outline_kill, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Draws outline around kill")
MACRO_CONFIG_INT(ClOutlineUnFreeze, al_outline_unfreeze, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Draws outline around unfreeze and undeep")
MACRO_CONFIG_INT(ClOutlineTele, al_outline_tele, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Draws outline around teleporters")
MACRO_CONFIG_INT(ClOutlineSolid, al_outline_solid, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Draws outline around hook and unhook")
MACRO_CONFIG_INT(ClOutlineWidth, al_outline_width, 5, 1, 16, CFGFLAG_CLIENT | CFGFLAG_SAVE, "(1-16) Width of freeze outline")
MACRO_CONFIG_INT(ClOutlineAlpha, al_outline_alpha, 50, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "(0-100) Outline alpha")
MACRO_CONFIG_INT(ClOutlineAlphaSolid, al_outline_alpha_solid, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "(0-100) Outline solids alpha")
MACRO_CONFIG_COL(ClOutlineColorSolid, al_outline_color_solid, 0, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Solid outline color") // 0 0 0
MACRO_CONFIG_COL(ClOutlineColorFreeze, al_outline_color_freeze, 0, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Freeze outline color") // 0 0 0
MACRO_CONFIG_COL(ClOutlineColorTele, al_outline_color_tele, 0, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tele outline color") // 0 0 0
MACRO_CONFIG_COL(ClOutlineColorUnfreeze, al_outline_color_unfreeze, 0, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Unfreeze outline color") // 0 0 0
MACRO_CONFIG_COL(ClOutlineColorKill, al_outline_color_kill, 0, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Unfreeze outline color") // 0 0 0

// Indicator Variables
MACRO_CONFIG_COL(ClIndicatorAlive, al_indicator_alive, 255, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Color of alive tees in player indicator")
MACRO_CONFIG_COL(ClIndicatorFreeze, al_indicator_freeze, 65407, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Color of frozen tees in player indicator")
MACRO_CONFIG_COL(ClIndicatorSaved, al_indicator_dead, 0, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Color of tees who is getting saved in player indicator")
MACRO_CONFIG_INT(ClIndicatorOffset, al_indicator_offset, 42, 16, 200, CFGFLAG_CLIENT | CFGFLAG_SAVE, "(16-128) Offset of indicator position")
MACRO_CONFIG_INT(ClIndicatorOffsetMax, al_indicator_offset_max, 100, 16, 200, CFGFLAG_CLIENT | CFGFLAG_SAVE, "(16-128) Max indicator offset for variable offset setting")
MACRO_CONFIG_INT(ClIndicatorVariableDistance, al_indicator_variable_distance, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Indicator circles will be further away the further the tee is")
MACRO_CONFIG_INT(ClIndicatorMaxDistance, al_indicator_variable_max_distance, 1000, 500, 7000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Maximum tee distance for variable offset")
MACRO_CONFIG_INT(ClIndicatorRadius, al_indicator_radius, 4, 1, 16, CFGFLAG_CLIENT | CFGFLAG_SAVE, "(1-16) indicator circle size")
MACRO_CONFIG_INT(ClIndicatorOpacity, al_indicator_opacity, 50, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Opacity of indicator circles")
MACRO_CONFIG_INT(ClPlayerIndicator, al_player_indicator, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show radial indicator of other tees")
MACRO_CONFIG_INT(ClPlayerIndicatorFreeze, al_player_indicator_freeze, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Only show frozen tees in indicator")
MACRO_CONFIG_INT(ClIndicatorTeamOnly, al_indicator_inteam, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Only show indicator while in team")
MACRO_CONFIG_INT(ClIndicatorTees, al_indicator_tees, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show tees instead of circles")
MACRO_CONFIG_INT(ClIndicatorHideVisible, al_indicator_hide_visible_tees, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Don't show tees that are on your screen")

// Bind Wheel
MACRO_CONFIG_INT(ClResetBindWheelMouse, al_reset_bindwheel_mouse, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Reset position of mouse when opening bindwheel")

// Regex chat matching
MACRO_CONFIG_STR(ClRegexChatIgnore, al_regex_chat_ignore, 512, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Filters out chat messages based on a regular expression.")

// Misc visual
//MACRO_CONFIG_INT(ClWhiteFeet, al_white_feet, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render all feet as perfectly white base color")
//MACRO_CONFIG_STR(ClWhiteFeetSkin, al_white_feet_skin, 255, "x_ninja", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Base skin for white feet")

//MACRO_CONFIG_INT(ClMiniDebug, al_mini_debug, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show position and angle")

MACRO_CONFIG_INT(ClNotifyWhenLast, al_last_notify, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Notify when you are last")
MACRO_CONFIG_STR(ClNotifyWhenLastText, al_last_notify_text, 64, "Last!", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Text for last notify")
MACRO_CONFIG_COL(ClNotifyWhenLastColor, al_last_notify_color, 256, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Color for last notify")
MACRO_CONFIG_INT(ClNotifyWhenLastXpos, al_last_notify_xpos, 100, 0, 500, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Left Right")
MACRO_CONFIG_INT(ClNotifyWhenLastYpos, al_last_notify_ypos, 16, 0, 300, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Up Down")
MACRO_CONFIG_INT(ClNotifyWhenLastSize, al_last_notify_size, 15, 0, 50, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Font Size")

//MACRO_CONFIG_INT(ClRenderCursorSpec, al_cursor_in_spec, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render your gun cursor when spectating in freeview")
//MACRO_CONFIG_INT(ClRenderCursorSpecAlpha, al_cursor_in_spec_alpha, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Alpha of cursor in freeview")

//MACRO_CONFIG_INT(ClRenderNameplateSpec, al_render_nameplate_spec, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render nameplates when spectating")

//MACRO_CONFIG_INT(ClTinyTees, al_tiny_tees, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render tees smaller")
//MACRO_CONFIG_INT(ClTinyTeesOthers, al_tiny_tees_others, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Render other tees smaller")

//MACRO_CONFIG_INT(ClCursorScale, al_cursor_scale, 100, 0, 500, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Percentage to scale the in game cursor by as a percentage (50 = half, 200 = double)")

// Profiles
MACRO_CONFIG_INT(ClApplyProfileSkin, al_profile_skin, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply skin in profiles")
MACRO_CONFIG_INT(ClApplyProfileName, al_profile_name, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply name in profiles")
MACRO_CONFIG_INT(ClApplyProfileClan, al_profile_clan, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply clan in profiles")
MACRO_CONFIG_INT(ClApplyProfileFlag, al_profile_flag, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply flag in profiles")
MACRO_CONFIG_INT(ClApplyProfileColors, al_profile_colors, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply colors in profiles")
MACRO_CONFIG_INT(ClApplyProfileEmote, al_profile_emote, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Apply emote in profiles")
/*
// Rainbow
MACRO_CONFIG_INT(ClRainbowTees, al_rainbow_tees, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Turn on rainbow client side")
MACRO_CONFIG_INT(ClRainbowHook, al_rainbow_hook, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Rainbow hook")
MACRO_CONFIG_INT(ClRainbowWeapon, al_rainbow_weapon, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Rainbow Weapons")

MACRO_CONFIG_INT(ClRainbowOthers, al_rainbow_others, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Turn on rainbow client side for others")
MACRO_CONFIG_INT(ClRainbowMode, al_rainbow_mode, 1, 1, 4, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Rainbow mode (1: rainbow, 2: pulse, 3: darkness, 4: random)")
MACRO_CONFIG_INT(ClRainbowSpeed, al_rainbow_speed, 100, 0, 10000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Rainbow speed as a percentage (50 = half speed, 200 = double speed)")

// War List
MACRO_CONFIG_INT(ClWarList, al_warlist, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Toggles war list visuals")
MACRO_CONFIG_INT(ClWarListShowClan, al_warlist_show_clan_if_war, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show clan in nameplate if there is a war")
MACRO_CONFIG_INT(ClWarListReason, al_warlist_reason, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show war reason")
MACRO_CONFIG_INT(ClWarListChat, al_warlist_chat, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show war colors in chat")
MACRO_CONFIG_INT(ClWarListScoreboard, al_warlist_scoreboard, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show war colors in scoreboard")
MACRO_CONFIG_INT(ClWarListAllowDuplicates, al_warlist_allow_duplicates, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Allow duplicate war entrys")
MACRO_CONFIG_INT(ClWarListSpectate, al_warlist_spectate, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show war colors in spectator menu")

MACRO_CONFIG_INT(ClWarListIndicator, al_warlist_indicator, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Use warlist for indicator")
MACRO_CONFIG_INT(ClWarListIndicatorColors, al_warlist_indicator_colors, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show warlist colors instead of freeze colors")
MACRO_CONFIG_INT(ClWarListIndicatorAll, al_warlist_indicator_all, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show all groups")
MACRO_CONFIG_INT(ClWarListIndicatorEnemy, al_warlist_indicator_enemy, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show players from the first group")
MACRO_CONFIG_INT(ClWarListIndicatorTeam, al_warlist_indicator_team, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show players from second group")

// Status Bar
MACRO_CONFIG_INT(ClStatusBar, al_statusbar, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable status bar")

MACRO_CONFIG_INT(ClStatusBar12HourClock, al_statusbar_12_hour_clock, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Use 12 hour clock in local time")
MACRO_CONFIG_INT(ClStatusBarLocalTimeSeocnds, al_statusbar_local_time_seconds, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show seconds in local time")
MACRO_CONFIG_INT(ClStatusBarHeight, al_statusbar_height, 8, 1, 16, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Height of the status bar")

MACRO_CONFIG_COL(ClStatusBarColor, al_statusbar_color, 3221225472, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Status bar background color")
MACRO_CONFIG_COL(ClStatusBarTextColor, al_statusbar_text_color, 4278190335, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Status bar text color")
MACRO_CONFIG_INT(ClStatusBarAlpha, al_statusbar_alpha, 75, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Status bar background alpha")
MACRO_CONFIG_INT(ClStatusBarTextAlpha, al_statusbar_text_alpha, 100, 0, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Status bar text alpha")

MACRO_CONFIG_INT(ClStatusBarLabels, al_statusbar_labels, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show labels on status bar entries")
MACRO_CONFIG_STR(ClStatusBarScheme, al_statusbar_scheme, 16, "ac pf r", CFGFLAG_CLIENT | CFGFLAG_SAVE, "The order in which to show status bar items")
*/
// Trails
MACRO_CONFIG_INT(ClTeeTrail, al_tee_trail, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Enable Tee trails")
MACRO_CONFIG_INT(ClTeeTrailOthers, al_tee_trail_others, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show tee trails for other players")
MACRO_CONFIG_INT(ClTeeTrailWidth, al_tee_trail_width, 15, 0, 20, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tee trail width")
MACRO_CONFIG_INT(ClTeeTrailLength, al_tee_trail_length, 25, 5, 200, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tee trail length")
MACRO_CONFIG_INT(ClTeeTrailAlpha, al_tee_trail_alpha, 80, 1, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tee trail alpha")
MACRO_CONFIG_COL(ClTeeTrailColor, al_tee_trail_color, 255, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tee trail color")
MACRO_CONFIG_INT(ClTeeTrailTaper, al_tee_trail_taper, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Taper tee trail over length")
MACRO_CONFIG_INT(ClTeeTrailFade, al_tee_trail_fade, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Fade trail alpha over length")
MACRO_CONFIG_INT(ClTeeTrailColorMode, al_tee_trail_color_mode, 1, 1, 5, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Tee trail color mode (1: Solid color, 2: Current Tee color, 3: Rainbow, 4: Color based on Tee speed, 5: Random)")

// Chat Reply
MACRO_CONFIG_INT(ClAutoReplyMuted, al_auto_reply_muted, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Auto reply to muted players with a message")
MACRO_CONFIG_STR(ClAutoReplyMutedMessage, al_auto_reply_muted_message, 128, "I have muted you", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Message to reply to muted players")
MACRO_CONFIG_INT(ClAutoReplyMinimized, al_auto_reply_minimized, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Auto reply when your game is minimized")
MACRO_CONFIG_STR(ClAutoReplyMinimizedMessage, al_auto_reply_minimized_message, 128, "I am not tabbed in", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Message to reply when your game is minimized")

// Voting
MACRO_CONFIG_INT(ClAutoVoteWhenFar, al_auto_vote_when_far, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Auto vote no if you far on a map")
MACRO_CONFIG_STR(ClAutoVoteWhenFarMessage, al_auto_vote_when_far_message, 128, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Message to send when auto far vote happens, leave empty to disable")
MACRO_CONFIG_INT(ClAutoVoteWhenFarTime, al_auto_vote_when_far_time, 5, 0, 20, CFGFLAG_CLIENT | CFGFLAG_SAVE, "How long until auto vote far happens")

// Integration
//MACRO_CONFIG_INT(ClDiscordRPC, al_discord_rpc, 1, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Toggle discord RPC (requires restart)")

// Font
MACRO_CONFIG_STR(ClCustomFont, al_custom_font, 255, "DejaVu Sans", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Custom font face")

// Bg Draw
MACRO_CONFIG_INT(ClBgDrawWidth, al_bg_draw_width, 5, 1, 50, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Width of background draw strokes")
MACRO_CONFIG_INT(ClBgDrawFadeTime, al_bg_draw_fade_time, 120, 0, 600, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Time until strokes dissapear (0 = never)")
MACRO_CONFIG_INT(ClBgDrawMaxItems, al_bg_draw_max_items, 128, 0, 2048, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Maximum number of strokes")
MACRO_CONFIG_COL(ClBgDrawColor, al_bg_draw_color, 14024576, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Color of background draw strokes")

// Translate
MACRO_CONFIG_STR(ClTranslateBackend, al_translate_backend, 32, "ftapi", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Translate backends (ftapi, libretranslate)")
MACRO_CONFIG_STR(ClTranslateTarget, al_translate_target, 16, "en", CFGFLAG_CLIENT | CFGFLAG_SAVE, "Translate target language (must be 2 character ISO 639 code)")
MACRO_CONFIG_STR(ClTranslateEndpoint, al_translate_endpoint, 256, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "For backends which need it, endpoint to use (must be https)")
MACRO_CONFIG_STR(ClTranslateKey, al_translate_key, 256, "", CFGFLAG_CLIENT | CFGFLAG_SAVE, "For backends which need it, api key to use")

// Animations
MACRO_CONFIG_INT(ClAnimateWheelTime, al_animate_wheel_time, 200, 0, 1000, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Duration of emote and bind wheel animations, in milliseconds (0 == no animation, 1000 = 1 second)")

// Pets
//MACRO_CONFIG_INT(ClPetShow, al_pet_show, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Show a pet")
//MACRO_CONFIG_STR(ClPetSkin, al_pet_skin, 24, "twinbop", CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Pet skin")
//MACRO_CONFIG_INT(ClPetSize, al_pet_size, 60, 10, 500, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Size of the pet as a percentage of a normal player")
//MACRO_CONFIG_INT(ClPetAlpha, al_pet_alpha, 100, 10, 100, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Alpha of pet (100 = fully opaque, 50 = half transparent)")

// Change name near finish
//MACRO_CONFIG_INT(ClChangeNameNearFinish, al_change_name_near_finish, 0, 0, 1, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Attempt to change your name when near finish")
//MACRO_CONFIG_STR(ClFinishName, al_finish_name, 16, "nameless tee", CFGFLAG_CLIENT | CFGFLAG_SAVE | CFGFLAG_INSENSITIVE, "Name to change to when near finish when al_change_name_near_finish is 1")

// Flags
//MACRO_CONFIG_INT(ClTClientSettingsTabs, al_tclient_settings_tabs, 0, 0, 65536, CFGFLAG_CLIENT | CFGFLAG_SAVE, "Bit flags to disable settings tabs")

// AAAAAAA
MACRO_CONFIG_INT(ClAmIFrozen, EEEfrz, 0, 0, 1, CFGFLAG_CLIENT, "")
