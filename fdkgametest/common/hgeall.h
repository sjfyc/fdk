#ifndef __HGEALL_H_INCLUDE__
#define __HGEALL_H_INCLUDE__
#include <hge/hge.h>
#include <hge/hgeanim.h>
#include <hge/hgecolor.h>
#include <hge/hgedistort.h>
#include <hge/hgefont.h>
#include <hge/hgegui.h>
#include <hge/hgeguictrls.h>
#include <hge/hgeparticle.h>
#include <hge/hgerect.h>
#include <hge/hgeresource.h>
#include <hge/hgesprite.h>
#include <hge/hgestrings.h>
#include <hge/hgevector.h>
#include <fdk/Base.h>

enum
{
	MyColor_Black = 0xFF000000,
	MyColor_White = 0xFFFFFFFF,
	MyColor_Red = 0xFFFF0000,
	MyColor_Green = 0xFF00FF00,
	MyColor_Blue = 0xFF0000FF,
	MyColor_Yellow = 0xFFFFFF00,
};

enum 
{
	GAME_SYSTEM_EVENT_KEYDOWN = 0,
	GAME_SYSTEM_EVENT_KEYUP,
	GAME_USER_EVENT = 100
};

class MyHGE 
	: public fdk::Singleton<MyHGE>
{
	friend class fdk::Singleton<MyHGE>;
public:
	bool IsKeyDown(int key);
	void Rectangle(float x1, float y1, float x2, float y2, DWORD col = MyColor_White, DWORD blendMode = BLEND_DEFAULT);
	void FrameRect(float x1, float y1, float x2, float y2, DWORD col = MyColor_White, int width=1);
	void DrawMouse(DWORD col=MyColor_Black);
	void Update();
	void OnKeyDown(int key);
	void OnKeyUp(int key);
	HGE* operator->();
	HGE& operator*();
private:
	MyHGE();
	~MyHGE();
	HGE* m_hge;
	bool m_bLastKeyDown[256];
};
#define g_HGE (MyHGE::instance())

inline bool MyHGE::IsKeyDown(int key)
{ 
	return m_hge->Input_GetKeyState(key); 
}

inline HGE* MyHGE::operator->() 
{
	return m_hge; 
}

inline HGE& MyHGE::operator*() 
{
	return *m_hge; 
}

#endif
