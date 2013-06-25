#include <common/hgeall.h>
#include "Types.h"
#include "Game.h"
#include "Option.h"

bool FrameFunc();
bool RenderFunc();

int main()
{
	g_HGE->System_SetState(HGE_LOGFILE, "astar.log");
	g_HGE->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	g_HGE->System_SetState(HGE_RENDERFUNC, RenderFunc);
	g_HGE->System_SetState(HGE_WINDOWED, true);
	g_HGE->System_SetState(HGE_SCREENWIDTH, CELL_SIZE_X*CELL_COUNT_X);
	g_HGE->System_SetState(HGE_SCREENHEIGHT, CELL_SIZE_Y*CELL_COUNT_Y);
	g_HGE->System_SetState(HGE_SCREENBPP, 32);
	g_HGE->System_SetState(HGE_TITLE, "AStar Demo");
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
	g_Game.update(g_HGE->Timer_GetDelta()*g_Option.getTimeRate());
	return false;
}

bool RenderFunc()
{
	g_HGE->Gfx_BeginScene();
	g_HGE->Gfx_Clear(0xFFFFFFFF);

	g_Game.render();
	g_HGE.DrawMouse();

	g_HGE->Gfx_EndScene();
	return false;
}
