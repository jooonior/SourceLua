#include "Plugin.h"

EXPOSE_SINGLE_INTERFACE(Plugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS);

Plugin::Plugin() {}
Plugin::~Plugin() {}

bool Plugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	ConMsg(PLUGIN_NAME " loaded.\n");
	return true;
}

void Plugin::Unload(void)
{
}

const char *Plugin::GetPluginDescription(void)
{
	return PLUGIN_NAME;
}

void Plugin::Pause(void) {}
void Plugin::UnPause(void) {}
void Plugin::LevelInit(char const *pMapName) {}
void Plugin::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax) {}
void Plugin::GameFrame(bool simulating) {}
void Plugin::LevelShutdown(void) {}
void Plugin::ClientActive(edict_t *pEntity) {}
void Plugin::ClientDisconnect(edict_t *pEntity) {}
void Plugin::ClientPutInServer(edict_t *pEntity, char const *playername) {}
void Plugin::SetCommandClient(int index) {}
void Plugin::ClientSettingsChanged(edict_t *pEdict) {}
PLUGIN_RESULT Plugin::ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen) { return PLUGIN_CONTINUE; }
PLUGIN_RESULT Plugin::ClientCommand(edict_t *pEntity, const CCommand &args) { return PLUGIN_CONTINUE; }
PLUGIN_RESULT Plugin::NetworkIDValidated(const char *pszUserName, const char *pszNetworkID) { return PLUGIN_CONTINUE; }
void Plugin::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue) {}
void Plugin::OnEdictAllocated(edict_t *edict) {}
void Plugin::OnEdictFreed(const edict_t *edict) {}
