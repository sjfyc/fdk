#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>

/*
C/LUA���߳���LUA֧�̵߳Ľ���
�����ߣ�C/LUA���߳�L��ͨ��lua_resume(T)�ָ�֧�߳�����
�����ߣ�LUA֧�߳�T��ͨ��lua_yield(T)�ص����߳�

��ͬ�ڴ�ͳ������̺߳����߳��Ƿֱ�������У�LUA���̺߳�֧�߳����ų�ģ�Ҳ����˵��һʱ��ֻ��һ֧�߳������У���/֧����
ͨ��������״̬�ǣ�
���߳�lua_resume(T)�ָ�ĳ��֧�߳����У���֧�߳�������lua_yield���̺߳�������ʱ�����̲߳Ż�ָ�����(lua_resume�����ڴ�ʱ�����أ�
*/ 

int wait(lua_State *L)
{
  return lua_yield(L, 0); /* ������֧�̵߳��øú�������˴��ݽ�����lua_Stage��֧�߳�T��T2 */
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
  
  T = lua_newthread(L);          /* <== ����һ��coroutine */
  luaL_loadfile(T, "main.lua");  /* <== coroutine��Ӧ�Ļص�����Ϊmain.lua��Ӧchunk */

  T2 = lua_newthread(L);         /* <== ������һ��coroutine */
  luaL_loadfile(T2, "main.lua"); /* <== coroutine��Ӧ�Ļص�����Ϊmain.lua��Ӧchunk */

  lua_resume(T, 0);              /* <== �ָ�T���У�ֱ������yield����� */ 
  lua_resume(T2, 0);             /* <== �ָ�T2���У�ֱ������yield����� */ 
  lua_resume(T, 0);              /* <== �ٴ�����T��ֱ������yield����� */
  lua_resume(T2, 0);             /* <== �ٴ�����T2��ֱ������yield����� */
  
  {
      /* �ⲿ�ִ���չʾ��coroutine�����й�����ע���ȫ�ֱ����ǿɱ����̻߳�����coroutine������ʵ� */
      /* ��ʹcoroutine���������Ѿ����н��� */
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