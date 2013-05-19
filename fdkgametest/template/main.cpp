#include <common/hgeall.h>
#include "Game.h"

bool FrameFunc();
bool RenderFunc();

int main()
{
	g_HGE->System_SetState(HGE_LOGFILE, "template.log");
	g_HGE->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	g_HGE->System_SetState(HGE_RENDERFUNC, RenderFunc);
	g_HGE->System_SetState(HGE_WINDOWED, true);
	g_HGE->System_SetState(HGE_SCREENWIDTH, 800);
	g_HGE->System_SetState(HGE_SCREENHEIGHT, 600);
	g_HGE->System_SetState(HGE_SCREENBPP, 32);
	g_HGE->System_SetState(HGE_TITLE, "template");
	g_HGE->System_SetState(HGE_FPS, 60);

	if (g_HGE->System_Initiate() && g_Game.start()) 
	{
		g_Game.IsRunning = true;
		g_HGE->System_Start();
	}

	g_Game.stop();
	g_HGE->System_Shutdown();
	g_HGE->Release();

	return 0;
}

bool FrameFunc()
{
	if (!g_Game.IsRunning)
	{
		return true;
	}
	g_HGE.Update();
	g_Game.update(g_HGE->Timer_GetDelta());
	return false;
}

bool RenderFunc()
{
	g_HGE->Gfx_BeginScene();
	g_HGE->Gfx_Clear(0);

	g_Game.render();

	g_HGE->Gfx_EndScene();
	return false;
}
