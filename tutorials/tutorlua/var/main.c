#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

int 
main(int argc, char *argv[])
{
  lua_State *L;
  int        ok;

  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }
  luaL_openlibs(L);

  lua_pushnumber(L, 10);        /* push 10 */
  lua_setglobal(L, "width_c");  /* pop 10; width_c = 10 in lua environment */  

  lua_pushnumber(L, 20);        /* push 20 */
  lua_setglobal(L, "height_c"); /* pop 20; height_c = 20 in lua environment */

  /* luaL_loadfile will push 'function() lua_file_content end'*/
  /* luaL_pcall will pop the temp function and run it; push error string if error occurs */  
  if (luaL_dofile(L, "main.lua")) { /* SAMEAS: if (luaL_loadfile(L, "main.lua") != 0 || lua_pcall(L, 0, 0, 0) != 0) { */
    printf("lua error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return -1;
  }
  
  ok = 1;

  lua_getglobal(L, "width_lua");  /* push value of width_lua */
  lua_getglobal(L, "height_lua"); /* push value of height_lua */
  
  if (ok && !lua_isnumber(L, -2)) {
    printf("lua error: width must be number!\n");
    ok = 0;
  }
  if (ok && !lua_isnumber(L, -1)) {
    printf("lua error: height must be number!\n");
    ok = 0;
  }

  if (ok)
    printf("width_lua=%d\nheight_lua=%d\n", (int)lua_tonumber(L, -2), (int)lua_tonumber(L, -1));

  lua_pop(L, 1); /* push the value of height */
  lua_pop(L, 1); /* push the value of width */

  lua_close(L);
  return 0;
}