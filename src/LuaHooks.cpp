#include "stdafx.hpp"

#include "LuaHooks.hpp"

static void stringify_stack(lua_State *L, int count)
{
	int top = lua_gettop(L);

	lua_getglobal(L, "tostring");

	for (int i = 0; i < count; i++)
	{
		if (lua_isstring(L, top - i))
			continue;

		// Call 'tostring' to convert the value.
		lua_pushvalue(L, -1);  // 'tostring' function
		lua_pushvalue(L, top - i);
		lua_call(L, 1, 1);

		if (!lua_isstring(L, -1))
		{
			lua_pop(L, 1);
			luaL_error(L, LUA_QL("tostring") " must return a string");
			return;
		}

		// Replace original with converted value.
		lua_replace(L, top - i);
	}

	lua_pop(L, 1);
}

static int l_print(lua_State *L)
{
	int arg_count = lua_gettop(L);

	stringify_stack(L, arg_count);

	for (int i = 1; i <= arg_count; i++)
	{
		if (i > 1)
			Msg("\t");

		Msg("%s", lua_tostring(L, i));
	}

	Msg("\n");

	lua_pop(L, arg_count);
	return 0;
}

static inline void lua_setglobalfunction(lua_State *L, const char *name, lua_CFunction f)
{
	lua_pushcfunction(L, f);
	lua_setglobal(L, name);
}

lua_State *LuaInit()
{
	lua_State *L = luaL_newstate();

	if (L == nullptr)
		return nullptr;

	luaL_openlibs(L);

	lua_setglobalfunction(L, "print", l_print);

	return L;
}

void LuaDestroy(lua_State *L)
{
	lua_close(L);
}

static int l_error_handler(lua_State *L)
{
	stringify_stack(L, 1);
	luaL_traceback(L, L, lua_tostring(L, -1), 1);
	return 1;
}

bool LuaRunChunk(lua_State *L, const char *chunk)
{
	// Reset stack.
	lua_settop(L, 0);

	// Load chunk.
	if (luaL_loadstring(L, chunk) != LUA_OK)
	{
		Warning("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);  // pop error message
		return false;
	}

	// Insert error handler.
	lua_pushcfunction(L, l_error_handler);
	lua_insert(L, 1);

	// Run chunk.
	if (lua_pcall(L, 0, 0, 1) != LUA_OK)
	{
		Warning("%s\n", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	// Pop error handler.
	lua_pop(L, 1);

	return true;
}
