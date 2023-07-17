#include "stdafx.hpp"

#include "Plugin.hpp"

#include "LuaHooks.hpp"
#include "ServerPluginCallbacks.hpp"

#include <lua.hpp>
#include <tier1/tier1.h>

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

#define PLUGIN_VERSION STR(PLUGIN_VERSION_MAJOR) "." STR(PLUGIN_VERSION_MINOR) "." STR(PLUGIN_VERSION_PATCH)

static lua_State *L;

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

	ConVar_Register();

	return true;
}

void Plugin::Unload(void)
{
	ConVar_Unregister();

	LuaDestroy(L);

	DisconnectTier1Libraries();
}

const char *Plugin::GetPluginDescription(void)
{
	return PLUGIN_NAME " v" PLUGIN_VERSION;
}

CON_COMMAND(lua, "Run Lua string")
{
	if (luaL_dostring(L, args.ArgS()) != LUA_OK)
	{
		Warning("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);  // pop error message
	}
}
