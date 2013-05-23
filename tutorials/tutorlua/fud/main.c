#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define SCRIPT_FUNC(func) static int func(lua_State *L)

typedef struct {
  int     size;
  double  val[1];
} Array;

SCRIPT_FUNC(newarray);
SCRIPT_FUNC(setarray);
SCRIPT_FUNC(getarray);
SCRIPT_FUNC(getsize);

static const luaL_Reg arraylib[] = {
  { "new", newarray },
  { "set", setarray },
  { "get", getarray },
  { "size", getsize },
  { NULL, NULL }
};

int 
main(int argc, char *argv[])
{
  lua_State *L;

  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }  
  luaL_openlibs(L);

  luaL_register(L, "array", arraylib);

  if (luaL_dofile(L, "main.lua") != 0) {
    printf("[LUA Error] %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return -1;
  }

  lua_close(L);
  return 0;
}

SCRIPT_FUNC(newarray) /* a = array.new(n) */
{
  Array *a;
  int    n;

  n = luaL_checkint(L, 1); 
  a = (Array *)lua_newuserdata(L, sizeof(Array)+sizeof(double)*(n-1));  /* array已经在栈顶 */
  a->size = n;
  return 1;
}

SCRIPT_FUNC(setarray) /* array.set(a, idx, val) */
{
  Array *a;
  int    idx;
  double val;

  a   = (Array *)lua_touserdata(L, 1);
  idx = luaL_checkint(L, 2);
  val = luaL_checknumber(L, 3);

  luaL_argcheck(L, a != NULL, 1, "'array' expected");                   /* 存在潜在的风险, 只能检测出userdata，但检测不出具体的类型 */
                                                                        /* 比如array.set(io.stdin, 1, 0), io.stdin也是个userdatum */
  luaL_argcheck(L, idx >= 1 && idx <= a->size, 2, "index out of range");

  a->val[idx-1] = val;
  return 0;
}

SCRIPT_FUNC(getarray) /* val = array.get(a, idx) */
{
  Array *a;
  int    idx;  

  a   = (Array *)lua_touserdata(L, 1);
  idx = luaL_checkint(L, 2);  

  luaL_argcheck(L, a != NULL, 1, "'array' expected");
  luaL_argcheck(L, idx >= 1 && idx <= a->size, 2, "index out of range");

  lua_pushnumber(L, a->val[idx-1]);
  return 1;
}

SCRIPT_FUNC(getsize) /* n = array.size(a) */
{
  Array *a;

  a   = (Array *)lua_touserdata(L, 1);  
  luaL_argcheck(L, a != NULL, 1, "'array' expected");

  lua_pushinteger(L, a->size);
  return 1;
}