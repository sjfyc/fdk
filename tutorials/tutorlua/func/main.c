#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

int func_c(lua_State *L);

int 
main(int argc, char *argv[])
{
  lua_State *L;  
  luaL_Reg   c[] = {
    { "func_c", func_c },
    { NULL, NULL }
  };

  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }
  luaL_openlibs(L);
  
  lua_register(L, "func_c", func_c);
  luaL_register(L, "c", c);
  
  if (luaL_dofile(L, "main.lua")) {
    printf("%s!\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return -1;
  }

  lua_getglobal(L, "func_lua");     /* push function func_lua */
                                    /* lua_isfunction check not needed, lua_pcall will do this */
  lua_pushnumber(L, 2);             /* push 1st argument */
  lua_pushnumber(L, 3);             /* push 2nd argument */
  if (lua_pcall(L, 2, 2, 0) != 0) { /* pop arguments and function, push all result or error string */
    printf("%s!\n", lua_tostring(L, -1));
    lua_pop(L, 1);
  }
  else {
    /* stack -1: 2nd result */
    /* stack -2: 1st result */
    if (!lua_isnumber(L, -2) || !lua_isnumber(L, -1))
      printf("func_lua must return 2 numbers!\n");
    else
      printf("func_lua(2, 3)=%d,%d\n", lua_tointeger(L, -2), lua_tointeger(L, -1));
    lua_pop(L, 2);
  }
    
  lua_close(L);
  return 0;
}

int 
func_c(lua_State *L)
{
  double a, b;
  a = luaL_checknumber(L, 1); /* 1st argument */
  b = luaL_checknumber(L, 2); /* 2nd argument */
  lua_pushnumber(L, a+b);     /* 1st result   */
  lua_pushnumber(L, a-b);     /* 2nd result   */
  return 2;
}