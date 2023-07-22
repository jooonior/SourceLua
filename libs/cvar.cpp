#include "lib.hpp"

#define CLIENT_DLL
#include <tier1/convar.h>

EXPORT_INTERFACE_VERSION(CVAR_INTERFACE_VERSION)

EXPORT_METHOD(ICvar, AllocateDLLIdentifier);
EXPORT_METHOD(ICvar, RegisterConCommand);
EXPORT_METHOD(ICvar, UnregisterConCommand);
EXPORT_METHOD(ICvar, UnregisterConCommands);
EXPORT_METHOD(ICvar, GetCommandLineValue);
EXPORT_METHOD(ICvar, FindCommandBase, ConCommandBase *(ICvar:: *)(const char *));
EXPORT_METHOD(ICvar, FindVar, ConVar *(ICvar:: *)(const char *));
EXPORT_METHOD(ICvar, FindCommand, ConCommand *(ICvar:: *)(const char *));
EXPORT_METHOD(ICvar, GetCommands, ConCommandBase *(ICvar:: *)());
EXPORT_METHOD(ICvar, InstallGlobalChangeCallback);
EXPORT_METHOD(ICvar, RemoveGlobalChangeCallback);
EXPORT_METHOD(ICvar, CallGlobalChangeCallbacks);
EXPORT_METHOD(ICvar, InstallConsoleDisplayFunc);
EXPORT_METHOD(ICvar, RemoveConsoleDisplayFunc);
// EXPORT_METHOD(ICvar, ConsoleColorPrintf);
// EXPORT_METHOD(ICvar, ConsolePrintf);
// EXPORT_METHOD(ICvar, ConsoleDPrintf);
EXPORT_METHOD(ICvar, RevertFlaggedConVars);
EXPORT_METHOD(ICvar, InstallCVarQuery);
EXPORT_METHOD(ICvar, IsMaterialThreadSetAllowed);
EXPORT_METHOD(ICvar, QueueMaterialThreadSetValue, void(ICvar:: *)(ConVar *, const char *));
EXPORT_METHOD_AS(ICvar, QueueMaterialThreadSetIntValue, QueueMaterialThreadSetValue, void(ICvar:: *)(ConVar *, int));
EXPORT_METHOD_AS(ICvar, QueueMaterialThreadSetFloatValue, QueueMaterialThreadSetValue, void(ICvar:: *)(ConVar *, float));
EXPORT_METHOD(ICvar, HasQueuedMaterialThreadConVarSets);
EXPORT_METHOD(ICvar, ProcessQueuedMaterialThreadConVarSets);

EXPORT_CLASS(CCommand, int nArgC, const char **ppArgV);
EXPORT_METHOD(CCommand, Tokenize);
EXPORT_METHOD(CCommand, Reset);
EXPORT_METHOD(CCommand, ArgC);
EXPORT_METHOD(CCommand, ArgV);
EXPORT_METHOD(CCommand, ArgS);
EXPORT_METHOD(CCommand, GetCommandString);
EXPORT_METHOD(CCommand, Arg);
EXPORT_METHOD(CCommand, FindArg);
EXPORT_METHOD(CCommand, FindArgInt);
EXPORT_METHOD(CCommand, MaxCommandLength);
EXPORT_METHOD(CCommand, DefaultBreakSet);

EXPORT_CLASS(ConCommandBase, const char *pName, const char *pHelpString, int flags);
EXPORT_METHOD(ConCommandBase, IsCommand);
EXPORT_METHOD(ConCommandBase, IsFlagSet);
EXPORT_METHOD(ConCommandBase, AddFlags);
EXPORT_METHOD(ConCommandBase, GetName);
EXPORT_METHOD(ConCommandBase, GetHelpText);
EXPORT_METHOD(ConCommandBase, GetNext, ConCommandBase *(ConCommandBase:: *)());
EXPORT_METHOD(ConCommandBase, IsRegistered);
EXPORT_METHOD(ConCommandBase, GetDLLIdentifier);

EXPORT_CLASS(ConCommand, const char *pName, FnCommandCallback_t callback, const char *pHelpString, int flags, FnCommandCompletionCallback completionFunc);
EXPORT_METHOD(ConCommand, AutoCompleteSuggest);
EXPORT_METHOD(ConCommand, CanAutoComplete);
EXPORT_METHOD(ConCommand, Dispatch);

EXPORT_CLASS(ConVar, const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback);
EXPORT_METHOD(ConVar, InstallChangeCallback);
EXPORT_METHOD(ConVar, GetFloat);
EXPORT_METHOD(ConVar, GetInt);
EXPORT_METHOD(ConVar, GetBool);
EXPORT_METHOD(ConVar, GetString);
EXPORT_METHOD(ConVar, SetValue, void (ConVar:: *)(const char *));
EXPORT_METHOD_AS(ConVar, SetIntValue, SetValue, void (ConVar:: *)(int));
EXPORT_METHOD_AS(ConVar, SetFloatValue, SetValue, void (ConVar:: *)(float));
EXPORT_METHOD(ConVar, Revert);
EXPORT_METHOD(ConVar, GetMin);
EXPORT_METHOD(ConVar, GetMax);
EXPORT_METHOD(ConVar, GetDefault);
EXPORT_METHOD(ConVar, SetDefault);
