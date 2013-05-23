#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>

static int settable_test();
static int gettable_test();

int 
main(int argc, char *argv[])
{
  settable_test();
  gettable_test();
  return 0;
}

int  
settable_test()
{
  struct Color {
    const char *name;
    unsigned char r, g, b;
  } colortable[] = {
    { "BLACK", 0x00, 0x00, 0x00 },
    { "WHITE", 0xFF, 0xFF, 0xFF },
    { "RED",   0xFF, 0x00, 0x00 },
    { "GREEN", 0x00, 0xFF, 0x00 },
    { "BLUE",  0x00, 0x00, 0xFF },    
  };

  lua_State *L;
  int       i;

  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }
  luaL_openlibs(L);

  /* register color table into lua environment */
  lua_newtable(L);                      /* color = {} */
  for (i = 0; i < sizeof(colortable)/sizeof(*colortable); i++) {
    lua_pushstring(L, colortable[i].name);

    lua_newtable(L);                    /* c = {} */
    lua_pushnumber(L, (double)colortable[i].r/255.0);      
    lua_rawseti(L, -2, 1);              /* c[1] = colortable[i].r */
    lua_pushnumber(L, (double)colortable[i].g/255.0);
    lua_rawseti(L, -2, 2);              /* c[2] = colortable[i].g */
    lua_pushnumber(L, (double)colortable[i].b/255.0); 
    lua_rawseti(L, -2, 3);              /* c[3] = colortable[i].b */

    lua_rawset(L, -3);                  /* color["name"] = c */
  }  
  lua_setglobal(L, "color");            /* pop color, add it into lua environment */

  /* luaL_loadfile will push 'function() lua_file_content end'*/
  /* luaL_pcall will pop the temp function and run it; push error string if error occurs */  
  if (luaL_dofile(L, "settable.lua")) { /* SAMEAS: if (luaL_loadfile(L, "main.lua") != 0 || lua_pcall(L, 0, 0, 0) != 0) { */
    printf("lua error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return -1;
  }  

  lua_close(L);
  return 0;
}

int 
gettable_test()
{
  lua_State *L;
  int        ok, oktmp;  
  int        i, j, n, nbadelems;

  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return -1;
  }
  luaL_openlibs(L);

  /* luaL_loadfile will push 'function() lua_file_content end'*/
  /* luaL_pcall will pop the temp function and run it; push error string if error occurs */  
  if (luaL_dofile(L, "gettable.lua")) { /* SAMEAS: if (luaL_loadfile(L, "main.lua") != 0 || lua_pcall(L, 0, 0, 0) != 0) { */
    printf("lua error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return -1;
  }

  /*--------background*/
  ok = 1;
  if (ok) {
    lua_getglobal(L, "background"); /* push background */
    if (!lua_istable(L, -1)) {
      printf("lua error: background must be table\n");
      ok = 0;
    }
  }

  if (ok) {
    /* lua_getfield(L, -1, "r") can be used to replace the following 2 functions, but slower */    
    lua_pushstring(L, "r");         /* push "r" */
    lua_rawget(L, -2);              /* pop "r", push background["r"] */

    if (!lua_isnumber(L, -1)) {
      ok = 0;
      printf("lua error: background[\"%s\"] must be number\n", "r");
    }
    else
      printf("background[\"r\"]=%g\n", lua_tonumber(L, -1));
    lua_pop(L, 1);                  /* pop background["r"] */
  }
  if (ok) {
    lua_pushstring(L, "g");         /* push "g" */
    lua_rawget(L, -2);              /* pop "g", push background["g"] */
    if (!lua_isnumber(L, -1)) {
      ok = 0;
      printf("lua error: background[\"%s\"] must be number\n", "g");
    }
    else
      printf("background[\"g\"]=%g\n", lua_tonumber(L, -1));
    lua_pop(L, 1);                  /* pop background["g"] */
  }
  if (ok) {
    lua_pushstring(L, "b");         /* push "b" */
    lua_rawget(L, -2);              /* pop "b", push background["b"] */
    if (!lua_isnumber(L, -1)) {
      ok = 0;
      printf("lua error: background[\"%s\"] must be number\n", "b");
    }
    else
      printf("background[\"b\"]=%g\n", lua_tonumber(L, -1));
    lua_pop(L, 1);                  /* pop background["b"] */
  }

  lua_pop(L, 1);                    /* pop background */ 

  /*--------positions*/
  ok = 1;
  if (ok) {
    lua_getglobal(L, "positions");  /* push positions */
    if (!lua_istable(L, -1)) {
      printf("lua error: positions must be table\n");
      ok = 0;
    }
  }

  if (ok) {
    n = luaL_getn(L, -1);
    nbadelems = 0;
    for (i = 1; i <= n; i++) {
      lua_rawgeti(L, -1, i);        /* push positions[i] */
      if (!lua_istable(L, -1)) {
        printf("lua error: positions[%d] must be table\n", i);
        nbadelems++;
      }
      else {
        for (j = 1; j <= 3; j++) {
          lua_rawgeti(L, -1, j);    /* push positions[i][j] */
          oktmp = lua_isnumber(L, -1);
          if (oktmp)
            printf("%g ", lua_tonumber(L, -1));
          lua_pop(L, 1);            /* pop positions[i][j] */
          if (!oktmp) {            
            printf("lua error: positions[%d][%d] must be number\n", i, j);
            nbadelems++;
            break;
          }
        }
      }
      lua_pop(L, 1);                /* pop positions[i] */
    }
  }
  
  if (ok)
    printf("positions: %d total, %d bad\n", n, nbadelems);  

  lua_pop(L, 1);                    /* pop positions */

  lua_close(L);
  return 0;
}
