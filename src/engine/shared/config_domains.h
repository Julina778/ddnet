// This file can be included several times.

#ifndef CONFIG_DOMAIN
#error "CONFIG_DOMAIN macro not defined"
#define CONFIG_DOMAIN(Name, ConfigPath, HasVars) ;
#endif

CONFIG_DOMAIN(DDNET, "settings_ddnet.cfg", true)
CONFIG_DOMAIN(TCLIENT, "settings_alesstya.cfg", true)
CONFIG_DOMAIN(TCLIENTPROFILES, "settings_profiles_alesstya.cfg", false)
//CONFIG_DOMAIN(TCLIENTCHATBINDS, "tclient_chatbinds.cfg", false)
//CONFIG_DOMAIN(TCLIENTWARLIST, "tclient_warlist.cfg", false)
