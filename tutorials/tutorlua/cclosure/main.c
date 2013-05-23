#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

int generatecclosure(lua_State *L);
int cclosure(lua_State *L);

int generatecclosure2(lua_State *L);
int cclosure2(lua_State *L);

int 
main(int argc, char *argv[])
{
  lua_State *L; 

  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }
  luaL_openlibs(L);  

  lua_register(L, "generatecclosure", generatecclosure);
  lua_register(L, "generatecclosure2", generatecclosure2);
  
  if (luaL_dofile(L, "main.lua")) {
    printf("lua error: %s!\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return -1;
  } 
    
  lua_close(L);
  return 0;
}

int 
generatecclosure(lua_State *L)        /* 闭包产生器 */
{
    lua_pushnumber(L, 0);             /* 压入第一个upvalue */ 
    lua_pushnumber(L, 0);             /* 压入第二个upvalue */
    lua_pushcclosure(L, cclosure, 2); /* 压入闭包的同时也把upvalue置入该闭包的upvalue表 */
    return 1;                         /* 返回闭包 */
}

int 
generatecclosure2(lua_State *L)        /* 闭包产生器 */
{
    lua_pushnumber(L, 10);             /* 压入第一个upvalue */ 
    lua_pushnumber(L, 10);             /* 压入第二个upvalue */
    lua_pushcclosure(L, cclosure2, 2); /* 压入闭包的同时也把upvalue置入该闭包的upvalue表 */
    return 1;                          /* 返回闭包 */
}

int 
cclosure(lua_State *L)
{
    double upval1, upval2;
    upval1 = lua_tonumber(L, lua_upvalueindex(1)); /* 注意upvalue索引1,2是闭包依赖的，不会和其他的闭包中的索引冲突*/
    upval2 = lua_tonumber(L, lua_upvalueindex(2));
    upval1++; upval2++;    
    lua_pushnumber(L, upval1); lua_replace(L, lua_upvalueindex(1));/* 更新upvalue1 */
    lua_pushnumber(L, upval2); lua_replace(L, lua_upvalueindex(2));/* 更新upvalue2 */
    lua_pushnumber(L, upval1 + upval2);
    return 1;
}

int 
cclosure2(lua_State *L)
{
    double upval1, upval2;
    upval1 = lua_tonumber(L, lua_upvalueindex(1));
    upval2 = lua_tonumber(L, lua_upvalueindex(2));
    upval1++; upval2++;
    lua_pushnumber(L, upval1); lua_replace(L, lua_upvalueindex(1));
    lua_pushnumber(L, upval2); lua_replace(L, lua_upvalueindex(2));
    lua_pushnumber(L, upval1 + upval2);
    return 1;
}