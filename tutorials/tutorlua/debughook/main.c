#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

int func(lua_State *L);

int main(int argc, char *argv[])
{
	lua_State *L;
	
	if ((L = lua_open()) == NULL) 
	{
		printf("lua_open() failed!\n");
		return -1;
	}
	luaL_openlibs(L);

	lua_register(L, "func", func);

	if (luaL_dofile(L, "main.lua")) 
	{
		printf("%s!\n", lua_tostring(L, -1));
		lua_pop(L, 1);
		lua_close(L);
		return -1;
	}

	lua_close(L);
	return 0;
}

int func(lua_State *L)
{
	lua_Debug ar;
	int i;
	for (i = 0; i < 3; ++i)
	{
		if (lua_getstack(L, i, &ar)) 
		{
			lua_getinfo(L, "nSl", &ar);
			printf("[%d]%s %s:%d\n", i, ar.name, ar.short_src, ar.currentline);
		}
	}
	return 0;
}