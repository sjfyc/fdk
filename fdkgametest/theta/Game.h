#ifndef __GAME_H_INCLUDE__
#define __GAME_H_INCLUDE__
#include "Types.h"
#include <fdk/EventHook.h>

class Game 
	: public fdk::Singleton<Game>
	, public fdk::EventHook
{
	friend class fdk::Singleton<Game>;
public:
	bool start();
	void stop();
	void update(float delta);
	void render();
	bool IsRunning;
private:
	Game();
	~Game();
	virtual void onEvent(int eventType, void* params);
	void outputUsage();
};

#define g_Game (Game::instance())
#endif
