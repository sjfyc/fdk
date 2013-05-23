#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <lua.hpp>

// If the value at the given acceptable index is a full user data and have a metatable with given name, returns its block address. 
// Otherwise, returns NULL.  
static void *touserdata(lua_State *L, int ud, const char *tname);

class A {
public:
    A(int val) : m_val(val)             { printf("c++: A::A(%d)\n", val); }
    ~A()                                { printf("c++: A::~A(%d)\n", m_val); }
    A(const A& rhs) : m_val(rhs.m_val)  { printf("c++: A::A(const A& rhs), rhs.m_val = %d\n", rhs.m_val); }
    void dump() const                   { printf("c++: A::dump(), m_val = %d\n", m_val); }
    void setval(int val)                { m_val = val; }
    int  getval() const                 { return m_val; }

    static void reg(lua_State *L);
    static void reg(lua_State *L, A *obj, const char *name, bool autogc = false);

private:
    struct Fud { A *ptr; bool autogc; };
    static A   *check(lua_State *L);

    static int L_constructor(lua_State *L);
    static int L_destructor(lua_State *L);
    static int L_dump(lua_State *L);
    static int L_setval(lua_State *L);
    static int L_getval(lua_State *L);
    static int L_tostring(lua_State *L);
   
private:
    int m_val;
};

int
main(int argc, char *argv[])
{
    lua_State *L;

    if ((L = lua_open()) == NULL) {
        printf("failed to create lua state!\n");
        return 1;
    }
    luaL_openlibs(L);

    A::reg(L);

    A obj(12345);
    A::reg(L, &obj, "cobj");

    if (luaL_dofile(L, "main.lua")) {
        printf("%s!\n", lua_tostring(L, -1));        
        lua_close(L);
        return 1;
    }
    lua_close(L);
    return 0;
}   

void *
touserdata(lua_State *L, int ud, const char *tname)
{
    // the same code as luaL_checkudata except no error thrown
    void *p = lua_touserdata(L, ud);
    if (p != NULL) {  /* value is a userdata? */
        if (lua_getmetatable(L, ud)) {  /* does it have a metatable? */
            lua_getfield(L, LUA_REGISTRYINDEX, tname);  /* get correct metatable */
            if (lua_rawequal(L, -1, -2)) {  /* does it have the correct mt? */
                lua_pop(L, 2);  /* remove both metatables */
                return p;
            }
        }
    }    
    return NULL;  /* to avoid warnings */    
}

void 
A::reg(lua_State *L)
{
    int A, methods, mt;

    // construct table methods
    lua_newtable(L);
    methods = lua_gettop(L);

    lua_pushliteral(L, "__call");
    lua_pushcfunction(L, &A::L_constructor);
    lua_rawset(L, methods);

    lua_pushliteral(L, "dump");
    lua_pushcfunction(L, &A::L_dump);
    lua_rawset(L, methods);

    lua_pushliteral(L, "setval");
    lua_pushcfunction(L, &A::L_setval);
    lua_rawset(L, methods);

    lua_pushliteral(L, "getval");
    lua_pushcfunction(L, &A::L_getval);
    lua_rawset(L, methods);

    // construct table mt
    luaL_newmetatable(L, "A");
    mt = lua_gettop(L);

    lua_pushliteral(L, "__index");
    lua_pushvalue(L, methods);
    lua_rawset(L, mt);

    lua_pushliteral(L, "__gc");
    lua_pushcfunction(L, &A::L_destructor);
    lua_rawset(L, mt);

    lua_pushliteral(L, "__tostring");
    lua_pushcfunction(L, &A::L_tostring);
    lua_rawset(L, mt);

    // construct table A
    lua_newtable(L);
    A = lua_gettop(L);
    lua_pushvalue(L, methods);
    lua_setmetatable(L, A);
    lua_setglobal(L, "A");
}

void 
A::reg(lua_State *L, A *obj, const char *name, bool autogc)
{
    int a, mt;
    Fud *fud;

    // make sure class A already registered
    lua_getfield(L, LUA_REGISTRYINDEX, "A");
    assert(!lua_isnil(L, -1));
    mt = lua_gettop(L);

    fud = (Fud *)lua_newuserdata(L, sizeof(Fud));
    fud->ptr = obj;
    fud->autogc = autogc;
    a = lua_gettop(L);
    lua_pushvalue(L, mt);
    lua_setmetatable(L, a);
    lua_setglobal(L, name);
}

A *
A::check(lua_State *L)
{
    Fud *fud;
    
    // make sure type A
    fud = (Fud *)luaL_checkudata(L, 1, "A");
    // remove self so that the argument start from 1
    lua_remove(L, 1);
    return fud->ptr;
}

int 
A::L_constructor(lua_State *L)
{
    int val, a, mt;
    Fud *fud, *rhs;    
    
    // remove unused self A
    lua_remove(L, 1);

    // make sure mt exist
    lua_getfield(L, LUA_REGISTRYINDEX, "A");
    assert(!lua_isnil(L, -1));
    mt = lua_gettop(L);

    // constructor an object a
    // object a is a full user data, a proxy of the actual c++ pointer
    if ((rhs = (Fud *)touserdata(L, 1, "A")) == NULL) {  // is lua script like: a = A(1)?
        val = luaL_checkint(L, 1);
        fud = (Fud *)lua_newuserdata(L, sizeof(Fud));
        fud->ptr = new A(val);
        fud->autogc = true;
    }
    else {                                              // is lua script like: b = A(a)?
        fud = (Fud *)lua_newuserdata(L, sizeof(Fud));
        fud->ptr = new A(*rhs->ptr);
        fud->autogc = true;
    }
    
    a = lua_gettop(L);
    lua_pushvalue(L, mt);    
    lua_setmetatable(L, a);    
    return 1;
}

int 
A::L_destructor(lua_State *L)
{
    Fud *fud;

    // make sure type A
    fud = (Fud *)luaL_checkudata(L, 1, "A");
    // remove self so that the argument start from 1
    lua_remove(L, 1);
    
    // free c++ resource when lua doing garbage collection
    if (fud->autogc)
        delete fud->ptr;
    return 0;
}

int 
A::L_dump(lua_State *L) 
{ 
    A *self;

    self = A::check(L);
    self->dump();
    return 0; 
}

int 
A::L_setval(lua_State *L) 
{ 
    A *self;
    int val;

    self = A::check(L);
    val = luaL_checkint(L, 1);
    self->setval(val);
    return 0; 
}

int 
A::L_getval(lua_State *L) 
{
    A *self;

    self = A::check(L);
    lua_pushnumber(L, self->getval());
    return 1; 
}

int 
A::L_tostring(lua_State *L) 
{
    A *self;

    self = A::check(L);
    lua_pushfstring(L, "m_val = %d", self->getval());
    return 1; 
}
