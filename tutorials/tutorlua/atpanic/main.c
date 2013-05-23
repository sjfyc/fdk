#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <setjmp.h>

static jmp_buf jbuf;

int
panichandler(lua_State *L)
{   
    printf("%s\n", lua_tostring(L, 1));
    lua_pop(L, 1);
    longjmp(&jbuf, 1);    
}

int 
main(int argc, char *argv[])
{
  lua_State *L;
  int        ret = 0;
  

  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return 1;
  }

  luaL_openlibs(L);
  lua_atpanic(L, panichandler);

  if (luaL_loadfile(L, "main.lua") == 0) {    
      if (setjmp(&jbuf) == 0)   /* first jmp */
          lua_call(L, 0, 0);      
      else                      /* second jmp, error */
          ret = 1;      
  } 
  else 
      ret = 1;

  lua_close(L);
  return ret;
}