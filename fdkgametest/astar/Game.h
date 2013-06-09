#ifndef __GAME_H_INCLUDE__
#define __GAME_H_INCLUDE__
#include "Types.h"
#include <fdk/EventHook.h>

class Game 
	: public fdk::Singleton<Game>
	, public fdk::EventHook
{
public:
	bool start();
	void stop();
	void update(float delta);
	void render();
	bool IsRunning;
private:
	virtual void onEvent(int eventType, void* params);
	CellCoord m_coordFrom;
	CellCoord m_coordTo;
};

#define g_Game (Game::instance())
#endif
