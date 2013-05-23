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
  { NULL, NULL }
};

static const luaL_Reg arraylib_m[] = {  
  { "set", setarray },
  { "get", getarray },
  { "size", getsize },
  { NULL, NULL }
};

#define ARRAY_METATABLE_NAME "array"               /* 注意这个名字在luaL_checkudata失败时会显示，通常为bad argument #n, array expected */
static Array *checkarray(lua_State *L, int idx);   /* 一个自定义的辅助函数，用来检查idx是否类型为array */

int 
main(int argc, char *argv[])
{
  lua_State *L;

  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }  
  luaL_openlibs(L);

  luaL_newmetatable(L, ARRAY_METATABLE_NAME); /* 在Registry表加入一个名字和空表的双向关联: Registry[name] = mt, Registry[mt] = name */
                                              /* 同时mt被压入栈顶 */
  lua_pushstring(L, "__index");               /* stack: mt __index    */
  lua_pushvalue(L, -2);                       /* stack: mt __index mt */
  lua_rawset(L, -3);                          /* stack: mt            *//* mt.__index = mt */

  luaL_register(L, NULL, arraylib_m);         /* 为栈顶的表添加成员, mt.set = setarray... */
  luaL_register(L, "array", arraylib);        /* 为"array"表添加成员, array.new = newarray */

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
  luaL_getmetatable(L, ARRAY_METATABLE_NAME); /* mt入栈顶 */
  lua_setmetatable(L, -2);                    /* 设置array的metatable为mt，mt出栈 */
                                              /* 所有array共享相同的mt */   
  return 1;
}

SCRIPT_FUNC(setarray) /* array.set(a, idx, val) */
{
  Array *a;
  int    idx;
  double val;

  a   = checkarray(L, 1);
  idx = luaL_checkint(L, 2);
  val = luaL_checknumber(L, 3);
  luaL_argcheck(L, idx >= 1 && idx <= a->size, 2, "index out of range");

  a->val[idx-1] = val;
  return 0;
}

SCRIPT_FUNC(getarray) /* val = array.get(a, idx) */
{
  Array *a;
  int    idx;  

  a   = checkarray(L, 1);
  idx = luaL_checkint(L, 2);  
  luaL_argcheck(L, idx >= 1 && idx <= a->size, 2, "index out of range");

  lua_pushnumber(L, a->val[idx-1]);
  return 1;
}

SCRIPT_FUNC(getsize) /* n = array.size(a) */
{
  Array *a;

  a = checkarray(L, 1);
  lua_pushinteger(L, a->size);
  return 1;
}

Array *
checkarray(lua_State *L, int idx)
{
  void *a;
  
  a = luaL_checkudata(L, 1, ARRAY_METATABLE_NAME); /* 如果不是带有metatable为ARRAY_METATABLE_NAME的user data,则返回NULL */
  luaL_argcheck(L, a != NULL, 1, "'array' expected");

  return (Array *)a;
}
