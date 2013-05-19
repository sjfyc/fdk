#include "hgeall.h"
#include <fdk/EventHook.h>

MyHGE::MyHGE()
{
	m_hge = hgeCreate(HGE_VERSION);
	fdk::zeroMemory(m_bLastKeyDown);
}

MyHGE::~MyHGE()
{
}

void MyHGE::Update()
{
	for (int key = 0; key < FDK_DIM(m_bLastKeyDown); ++key)
	{
		bool isDown = IsKeyDown(key);
		if (!m_bLastKeyDown[key] && isDown)
		{
			OnKeyDown(key);
		}
		else if (m_bLastKeyDown[key] && !isDown)
		{
			OnKeyUp(key);
		}
		m_bLastKeyDown[key] = isDown;
	}
}

void MyHGE::OnKeyDown(int key)
{
	fdk::EventCenter::instance().send(GAME_SYSTEM_EVENT_KEYDOWN,(void*)key);
}

void MyHGE::OnKeyUp(int key)
{
	fdk::EventCenter::instance().send(GAME_SYSTEM_EVENT_KEYUP,(void*)key);
}

void MyHGE::Rectangle(float x1, float y1, float x2, float y2, DWORD col, DWORD blendMode)
{
	hgeQuad quad;	
	quad.tex = 0;
	quad.blend = blendMode;

	quad.v[0].x = x1;
	quad.v[0].y = y1;
	quad.v[0].z = 0.5;
	quad.v[0].col = col;	

	quad.v[1].x = x2;
	quad.v[1].y = y1;
	quad.v[1].z = 0.5;
	quad.v[1].col = col;

	quad.v[2].x = x2;
	quad.v[2].y = y2;
	quad.v[2].z = 0.5;
	quad.v[2].col = col;

	quad.v[3].x = x1;
	quad.v[3].y = y2;
	quad.v[3].z = 0.5;
	quad.v[3].col = col;

	m_hge->Gfx_RenderQuad(&quad);
}

void MyHGE::FrameRect(float x1, float y1, float x2, float y2, DWORD col)
{
	m_hge->Gfx_RenderLine(x1, y1, x2, y1, col);
	m_hge->Gfx_RenderLine(x1, y1, x1, y2, col);
	m_hge->Gfx_RenderLine(x1, y2, x2, y2, col);
	m_hge->Gfx_RenderLine(x2, y1, x2, y2, col);
}

void MyHGE::DrawMouse(DWORD col)
{	
	float x0, y0;
	float x1, y1;
	float x2, y2;
	m_hge->Input_GetMousePos(&x0, &y0);
	x1 = x0; y1 = y0+16;
	x2 = x0+12; y2 = y0+12;
	g_HGE->Gfx_RenderLine(x0, y0, x1, y1, col);
	g_HGE->Gfx_RenderLine(x1, y1, x2, y2, col);
	g_HGE->Gfx_RenderLine(x2, y2, x0, y0, col);
}
