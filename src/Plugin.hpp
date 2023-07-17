#pragma once

#define PLUGIN_NAME "Lua"
#define PLUGIN_VERSION_MAJOR 1
#define PLUGIN_VERSION_MINOR 0
#define PLUGIN_VERSION_PATCH 0

#include <Color.h>
#include <dbg.h>

__forceinline void PluginPrintPrefix()
{
	ConColorMsg(Color(0, 153, 153, 255), "[%s] ", PLUGIN_NAME);
}

template<class... Parameters>
__forceinline void PluginMsg(const char *fmt, const Parameters&... param)
{
	PluginPrintPrefix();
	Msg(fmt, param...);
}

template<class... Parameters>
__forceinline void PluginWarning(const char *fmt, const Parameters&... param)
{
	PluginPrintPrefix();
	Warning(fmt, param...);
}

template<class... Parameters>
__forceinline void PluginColorMsg(const Color &color, const char *fmt, Parameters... param)
{
	PluginPrintPrefix();
	ConColorMsg(color, fmt, param...);
}
