#include "stdafx.hpp"

#include "Plugin.hpp"

#include "LuaHooks.hpp"
#include "ServerPluginCallbacks.hpp"

#include <lua.hpp>
#include <tier1/tier1.h>
#include <tier2/tier2.h>
#include <filesystem.h>

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

#define PLUGIN_VERSION STR(PLUGIN_VERSION_MAJOR) "." STR(PLUGIN_VERSION_MINOR) "." STR(PLUGIN_VERSION_PATCH)

static lua_State *L = nullptr;

class Plugin : public ServerPluginCallbacks
{
public:
	Plugin();
	~Plugin();

	virtual bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void Unload(void);
	virtual const char *GetPluginDescription(void);
};

EXPOSE_SINGLE_INTERFACE(Plugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS);

Plugin::Plugin() {}
Plugin::~Plugin() {}

bool Plugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	PluginMsg("Loading plugin version %s\n", PLUGIN_VERSION);

	L = LuaInit();
	if (L == nullptr)
	{
		PluginWarning("Failed to create Lua state.");
		return false;
	}

	PluginMsg("%s (%s)\n", LUAJIT_VERSION, LUA_VERSION);

	ConnectTier1Libraries(&interfaceFactory, 1);
	ConnectTier2Libraries(&interfaceFactory, 1);

	ConVar_Register();

	return true;
}

void Plugin::Unload(void)
{
	ConVar_Unregister();

	LuaDestroy(L);

	DisconnectTier2Libraries();
	DisconnectTier1Libraries();
}

const char *Plugin::GetPluginDescription(void)
{
	return PLUGIN_NAME " v" PLUGIN_VERSION;
}

CON_COMMAND(lua, "Execute text as Lua code")
{
	if (args.ArgC() == 1)
	{
		Warning("Usage: lua SOME LUA CODE\n");
		Msg(
			"Quote handling depends on number of arguments:\n"
			"  1  outermost quotes are stripped\n"
			"  2+ all quotes are preserved\n"
			"Examples:\n"
			"  lua print \"hello world\"     ~> print \"hello world\"\n"
			"  lua \"local i = 1; print(i)\" ~> local i = 1; print(i)\n"
		);
		return;
	}

	// This way the command with with or without quoted parameter.
	auto arg = args.ArgC() == 2 ? args.Arg(1) : args.ArgS();

	LuaRunChunk(L, arg);
}

CON_COMMAND(lua_file, "Execute Lua file")
{
	if (args.ArgC() < 2)
	{
		Warning("Usage: lua_file <file path> [args]...\n");
		Msg(
			"Resolved file path and arguments are passed to the script.\n"
		);
		return;
	}

	auto relative_path = args.Arg(1);

	char buffer[MAX_PATH];
	auto absolute_path = g_pFullFileSystem->RelativePathToFullPath_safe(relative_path, nullptr, buffer);

	if (absolute_path == nullptr)
	{
		PluginWarning("'%s' not found; not executing\n", relative_path);
		return;
	}

	PluginMsg("Executing file: %s\n", absolute_path);

	LuaRunFile(L, absolute_path, args.ArgC() - 2, args.ArgV() + 2);
}

CON_COMMAND(lua_restart, "Restart the Lua execution environment")
{
	if (L)
	{
		LuaDestroy(L);
	}

	L = LuaInit();
}

CON_COMMAND(lua_version, "Print plugin version and information about Lua binaries")
{
	PluginMsg("Plugin version %s, %s (%s)\n", PLUGIN_VERSION, LUAJIT_VERSION, LUA_VERSION);
}
