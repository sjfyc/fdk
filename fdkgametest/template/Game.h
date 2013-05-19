#ifndef __GAME_H_INCLUDE__
#define __GAME_H_INCLUDE__
#include <fdk/Base.h>

class Game 
	: public fdk::Singleton<Game>
{
public:
	bool start();
	void stop();
	void update(float delta);
	void render();
	bool IsRunning;
};

#define g_Game (Game::instance())
#endif
