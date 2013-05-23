#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int val;
} cptr;
cptr *cptr__ = NULL;

static int getcptr(lua_State *L);
static int setcptrval(lua_State *L);
static int getcptrval(lua_State *L);

int 
main(int argc, char *argv[])
{
  lua_State *L;  
    
  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }
  luaL_openlibs(L);

  cptr__ = (cptr *)malloc(sizeof(cptr));

  lua_register(L, "getcptr",    getcptr);
  lua_register(L, "setcptrval", setcptrval);
  lua_register(L, "getcptrval", getcptrval);

  if (luaL_dofile(L, "main.lua")) {
    printf("lua error: %s!\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return -1;
  }  

  free(cptr__);
  lua_close(L);
  return 0;
}

int 
getcptr(lua_State *L)
{
  lua_pushlightuserdata(L, cptr__);  
  return 1;
};

int 
setcptrval(lua_State *L)
{  
  cptr *p = (cptr *)lua_touserdata(L, 1);             /* return full user datum / light user data pointer */
  luaL_argcheck(L, p != NULL, 1, "'cptr' expected");  /* don't use luaL_checkudata which has another meaning */
                                                      /* u can also use lua_isuserdata(L, 1) for checking, but not good; in a native function, you'd better use unprotected function */
  p->val = luaL_checkint(L, 2);
  return 0;
}

int 
getcptrval(lua_State *L)
{
  cptr *p = (cptr *)lua_touserdata(L, 1);  
  luaL_argcheck(L, p != NULL, 1, "'cptr' expected");
  lua_pushnumber(L, p->val);
  return 1;
}