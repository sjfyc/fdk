#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>

/*
C/LUA主线程与LUA支线程的交互
主控者：C/LUA主线程L，通过lua_resume(T)恢复支线程运行
被控者：LUA支线程T，通过lua_yield(T)回到主线程

不同于传统意义的线程和主线程是分别独立运行；LUA主线程和支线程是排斥的，也就是说任一时刻只有一支线程在运行（主/支）。
通常的运行状态是：
主线程lua_resume(T)恢复某个支线程运行，当支线程运行至lua_yield或线程函数结束时，主线程才会恢复运行(lua_resume函数在此时方返回）
*/ 

int wait(lua_State *L)
{
  return lua_yield(L, 0); /* 由于是支线程调用该函数，因此传递进来的lua_Stage即支线程T或T2 */
}

int
main(int argc, char *argv[])
{
  lua_State *L, *T, *T2;  
    
  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }
  luaL_openlibs(L);

  lua_register(L, "wait", wait);
  
  T = lua_newthread(L);          /* <== 创建一个coroutine */
  luaL_loadfile(T, "main.lua");  /* <== coroutine对应的回调函数为main.lua对应chunk */

  T2 = lua_newthread(L);         /* <== 创建另一个coroutine */
  luaL_loadfile(T2, "main.lua"); /* <== coroutine对应的回调函数为main.lua对应chunk */

  lua_resume(T, 0);              /* <== 恢复T运行，直到遇到yield或结束 */ 
  lua_resume(T2, 0);             /* <== 恢复T2运行，直到遇到yield或结束 */ 
  lua_resume(T, 0);              /* <== 再次运行T，直到遇到yield或结束 */
  lua_resume(T2, 0);             /* <== 再次运行T2，直到遇到yield或结束 */
  
  {
      /* 这部分代码展示了coroutine在运行过程中注册的全局变量是可被主线程或其他coroutine共享访问的 */
      /* 即使coroutine的主函数已经运行结束 */
      int la, ta, t2a;
      lua_getglobal(L, "a");
      la = luaL_checkint(L, -1);
      luaL_argcheck(L, la == 100, -1, NULL);
      lua_getglobal(T, "a");
      ta = luaL_checkint(T, -1);
      luaL_argcheck(T, ta == 100, -1, NULL);
      lua_getglobal(T2, "a");
      t2a = luaL_checkint(T2, -1);
      luaL_argcheck(T2, t2a == 100, -1, NULL);
  }
  
  lua_close(L);
  return 0;
}