#pragma once

#include <lua.hpp>

lua_State *LuaInit();

void LuaDestroy(lua_State *L);

bool LuaRunChunk(lua_State *L, const char *chunk);

bool LuaRunFile(lua_State *L, const char *path, int argc, const char *argv[]);
