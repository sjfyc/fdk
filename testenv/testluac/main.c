#include <stdio.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

int main(int argc, char *argv[])
{  
	lua_State *L;

	L = lua_open();
	luaL_openlibs(L);  

	if (luaL_loadfile(L, "main.lua") != 0 || lua_pcall(L, 0, 0, 0) != 0) 
	{
		printf("%s", lua_tostring(L, -1));
		lua_close(L);
		return -1;
	}

	lua_close(L);
	return 0;
}
