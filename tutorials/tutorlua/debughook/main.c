#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

int func_c(lua_State *L);

int main(int argc, char *argv[])
{
	lua_State *L;  

	if ((L = lua_open()) == NULL) 
	{
		printf("lua_open() failed!\n");
		return -1;
	}
	luaL_openlibs(L);

	lua_register(L, "func_c", func_c);

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

int func_c(lua_State *L)
{

	return 2;
}