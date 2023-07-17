#include "stdafx.hpp"

#include "LuaHooks.hpp"

static int l_print(lua_State *L)
{
	int arg_count = lua_gettop(L);

	lua_getglobal(L, "tostring");

	for (int i = 1; i <= arg_count; i++)
	{
		// Call 'tostring' to convert the value.
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);

		// Get the result.
		const char *s = lua_tostring(L, -1);
		lua_pop(L, 1);

		if (s == nullptr)
			return luaL_error(L, LUA_QL("tostring") " must return a string");

		if (i > 1)
			Msg("\t");

		Msg("%s", s);
	}

	Msg("\n");

	return 0;  // number of results
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
