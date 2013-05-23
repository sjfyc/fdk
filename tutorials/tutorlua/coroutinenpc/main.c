/* http://hi.baidu.com/bidepan2023/blog/item/aaa2501190e70618b9127b1c.html */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

enum { WAITKEY, WAITTIME, DEAD, ERR };
typedef struct Talksession {
    lua_State *co;
    int npcid;
    int state;
    int endtime;
    struct Talksession *next;
} Talksession;
Talksession *g_talksession = NULL;

static Talksession *ts_new(lua_State *L, int npcid);
static void ts_del(Talksession *t);
static int ts_getn();
#define ts_isempty() (ts_getn() == 0)
static void ts_resume(Talksession *t, int flag);
#define ts_needcleanup(t) ((t)->state == DEAD || (t)->state == ERR)
static void ts_cleanup();
static void ts_foreach(void(*)(Talksession *));
static void ts_onkeypressed(Talksession *t);
static void ts_ontime(Talksession *t);

lua_State *g_L = NULL;
static void loadnpcchunks(lua_State *L);
static int getnpcchunk(lua_State *co, int npcid);
static void _G_add_co_ts_pair(lua_State *co, Talksession *t);
static Talksession *_G_get_ts(lua_State *co);

static int L_waitkey(lua_State *co);
static int L_waittime(lua_State *co);
static void waitkey(Talksession *t);
static void waittime(Talksession *t, int secs);

static int frame();

int main(int argc, char *argv[])
{
    g_L = lua_open();
    luaL_openlibs(g_L);
    loadnpcchunks(g_L);
    lua_register(g_L, "waitkey", L_waitkey);
    lua_register(g_L, "waittime", L_waittime);
    while (frame())
        ;
    lua_close(g_L);
    return 0;
}

Talksession *ts_new(lua_State *L, int npcid)
{
    Talksession *t;
    t = (Talksession *)malloc(sizeof(Talksession));
    t->next = g_talksession;
    g_talksession = t;
    t->co = lua_newthread(L);
    _G_add_co_ts_pair(t->co, t);
    getnpcchunk(t->co, npcid); 
    ts_resume(t, 0);
    return t;
}

void ts_del(Talksession *t)
{
    Talksession *prev, *curr;
    prev = NULL;
    curr = g_talksession;
    while (curr != NULL) {
        if (curr == t) {
            if (prev != NULL)
                prev->next = curr->next;
            else
                g_talksession = curr->next;
            free(curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }
}

int ts_getn()
{    
    int cnt;
    Talksession *t;
    cnt = 0;
    t = g_talksession;    
    while (t != NULL) {
        cnt++;
        t = t->next;        
    }
    return cnt;
}

void ts_cleanup()
{
    Talksession *prev, *curr, *temp;
    prev = NULL;
    curr = g_talksession;
    while (curr != NULL) {
        if (ts_needcleanup(curr)) {
            if (prev != NULL)
                prev->next = curr->next;
            else
                g_talksession = curr->next;
            temp = curr;
            curr = curr->next;
            free(temp);
            continue;
        }
        prev = curr;
        curr = curr->next;
    }
}

void ts_foreach(void(* action)(Talksession *))
{
    Talksession *t;
    t = g_talksession;    
    while (t != NULL) {
        action(t);
        t = t->next;        
    }
}

void ts_onkeypressed(Talksession *t)
{
    if (t->state == WAITKEY)
        ts_resume(t, 0);
}

void ts_ontime(Talksession *t)
{
    if (t->state == WAITTIME && (int)time(0) > t->endtime) {
        ts_resume(t, 0);
    }
}

void ts_resume(Talksession *t, int flag)
{
    int ret;
    lua_pushnumber(t->co, flag);
    if ((ret = lua_resume(t->co, 1)) == 0)
        t->state = DEAD;
    else if (ret != LUA_YIELD) {
        printf("%s\n", lua_tostring(t->co, -1));
        t->state = ERR;
    }
}

void loadnpcchunks(lua_State *L) 
{
    lua_newtable(L);
    luaL_loadfile(L, "101.lua"); lua_rawseti(L, -2, 101);
    luaL_loadfile(L, "102.lua"); lua_rawseti(L, -2, 102);
    lua_setglobal(L, "npcchunks");
}

int getnpcchunk(lua_State *co, int npcid)
{
    lua_getglobal(co, "npcchunks");
    lua_rawgeti(co, -1, npcid);
    if (!lua_isfunction(co, -1)) {
        lua_pop(co, 2);
        return -1;
    }
    lua_remove(co, -2);
    return 0;
}

int frame()
{
    ts_cleanup();

    /* process input */
    if (_kbhit()) {
        int c = _getch();
        /* take 0 1 2 as special key only if no talk sessions */
        if (ts_isempty()) {
            if (c == '0') /* 0 to exit */
                return 0; 
            else if (c == '1' || c == '2')
                ts_new(g_L, c-'0'+100);             
        }
        else
            ts_foreach(ts_onkeypressed);
    }

    /* process timer */
    ts_foreach(ts_ontime);

    return 1;
}

/* _G[co] = t */
void _G_add_co_ts_pair(lua_State *co, Talksession *t)
{
    lua_pushlightuserdata(co, co);
    lua_pushlightuserdata(co, t);
    lua_rawset(co, LUA_GLOBALSINDEX);
}

/* return _G[co] */
Talksession *_G_get_ts(lua_State *co)
{
    Talksession *t;
    lua_pushlightuserdata(co, co);
    lua_rawget(co, LUA_GLOBALSINDEX);
    t = (Talksession *)lua_touserdata(co, -1);
    luaL_argcheck(co, t != NULL, -1, NULL);
    lua_pop(co, 1);
    return t;
}

int L_waitkey(lua_State *co)
{
    Talksession *t;
    t = _G_get_ts(co);
    waitkey(t);
    return lua_yield(co, 0);
}

int L_waittime(lua_State *co)
{
    Talksession *t;
    t = _G_get_ts(co);
    waittime(t, luaL_checkint(co, 1));
    return lua_yield(co, 0);
}

void waitkey(Talksession *t)
{
    t->state = WAITKEY;
}

void waittime(Talksession *t, int secs)
{
    t->state = WAITTIME;
    t->endtime = (int)time(0) + secs;
}