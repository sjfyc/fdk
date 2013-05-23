/*
基本思想是: FullUserData实现面向对象的LUA接口（本质还是NativeFunction）， 这些接口转而调用实体类成员函数
比较常见的步骤是:
1. C++代码中产生实体类，实体类构造时产生LUA环境并创建FullUserData，此时配置FullUserData的实体类指针(this指针)
2. FullUserData实现面向对象的LUA接口（参考fulluserdatarw_exex）
3. 注册FullUserData到LUA Global表。
*/
#include <lua.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define PERSON_NAME_IN_LUA          "person"
#define PERSON_ARG_CHECK_FAILED_MSG "'" PERSON_NAME_IN_LUA "'" " expected"

#define SCRIPT_FUNC_DECL(func) static int func(lua_State *L)
#define SCRIPT_FUNC_IMPL(func, classtype) int classtype::func(lua_State *L)

class person
{
public:
  person(const std::string& name = "");
  ~person();
  void setname(const std::string& name) { m_name = name; }             
  const std::string getname() const { return m_name; }
  void getpos(double pos[3]) { pos[0] = pos[1] = pos[2] = 5.0; }

private:
  std::string m_name;

private:

  void _regtolua(lua_State *L);
  lua_State *L;
  
  static person *checkperson(lua_State *L, int idx);  
  SCRIPT_FUNC_DECL(setname_lua);
  SCRIPT_FUNC_DECL(getname_lua);
  SCRIPT_FUNC_DECL(getpos_lua);
  SCRIPT_FUNC_DECL(gc_lua);
};

person::person(const std::string& name)
: m_name(name)
, L(NULL)
{
  if ((L = lua_open()) == NULL) {
    printf("lua_open() failed!\n");
    return;
  }
  luaL_openlibs(L);

  _regtolua(L);

  if (luaL_dofile(L, "main.lua")) {
    printf("lua error: %s!\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    lua_close(L);
    return;
  }
}

person::~person()
{
  lua_close(L);
  printf("person::~person()");
}

void person::_regtolua(lua_State *L)
{
  void **p;  
  static const luaL_Reg lib[] = {
    { "setname",  setname_lua },
    { "getname",  getname_lua },
    { "getpos",   getpos_lua  },
    { NULL, NULL }
  };
  
  p = (void **)lua_newuserdata(L, sizeof(void **));
  *p = this;          /* stack: fud */

  luaL_newmetatable(L, PERSON_NAME_IN_LUA);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_rawset(L, -3);        /* mt.__index = mt */
                            /* stack: fud mt */
  lua_pushliteral(L, "__gc");
  lua_pushcfunction(L, gc_lua);  
  lua_rawset(L, -3);        /* mt.__index = mt */
                            /* stack: fud mt */
  /* add native function into metatable */
  luaL_register(L, NULL, lib);

                            /* stack: fud mt */
  lua_setmetatable(L, -2);  /* stack: fud */ /* fud.metatable = mt */
  lua_setglobal(L, PERSON_NAME_IN_LUA);  
}

SCRIPT_FUNC_IMPL(setname_lua, person)
{
  person     *p;
  const char *name;

  p     = checkperson(L, 1);
  name  = luaL_checkstring(L, 2);

  p->setname(name);
  return 0;
}

SCRIPT_FUNC_IMPL(getname_lua, person)
{
  person     *p;  

  p = checkperson(L, 1);
  lua_pushstring(L, p->getname().c_str());
  return 1;
}

SCRIPT_FUNC_IMPL(getpos_lua, person)
{
  person     *p;  
  double      pos[3];

  p = checkperson(L, 1);
  p->getpos(pos);
  
  lua_newtable(L);
  lua_pushnumber(L, pos[0]); lua_rawseti(L, -2, 1);
  lua_pushnumber(L, pos[1]); lua_rawseti(L, -2, 2);
  lua_pushnumber(L, pos[2]); lua_rawseti(L, -2, 3);
  return 1;
}

SCRIPT_FUNC_IMPL(gc_lua, person)
{
    person     *p;  

    p = checkperson(L, 1);
    printf("lua gc\n");
    return 0;
}

person *person::checkperson(lua_State *L, int idx)
{  
  void *p;

  p = luaL_checkudata(L, idx, PERSON_NAME_IN_LUA);
  luaL_argcheck(L, p != NULL, idx, PERSON_ARG_CHECK_FAILED_MSG);

  return (person *)(*(void **)p);
}

int 
main(int argc, char *argv[])
{
  person p("a person"); 
}
