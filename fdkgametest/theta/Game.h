#ifndef __GAME_H_INCLUDE__
#define __GAME_H_INCLUDE__
#include "Types.h"
#include <fdk/EventHook.h>
class GameMode;

class Game 
	: public fdk::Singleton<Game>
	, public fdk::EventHook
{
	friend class fdk::Singleton<Game>;
	friend class GameModeGame;
	friend class GameModeMapEdit;
public:
	bool start();
	void stop();
	void update(float delta);
	void render();
	bool isInGameMode() const;
	bool IsRunning;	
	void pauseGame();
private:
	Game();
	~Game();
	virtual void onEvent(int eventType, void* params);
	void outputUsage();
	void toggleMode();
	GameMode* m_mode;
	float m_savedDeltaTime;
};

class GameMode
{
public:
	virtual void enter(Game& game) {}
	virtual void leave(Game& game) {}
	virtual void update(Game& game, float delta) {}
	virtual void render(Game& game) {}
	virtual void handleEvent(Game& game, int eventType, void* params) {}
};

class GameModeGame
	: public GameMode
	, public fdk::Singleton<GameModeGame>
{
	friend class fdk::Singleton<GameModeGame>;
public:
	virtual void enter(Game& game);
	virtual void leave(Game& game);
	virtual void update(Game& game, float delta);
	virtual void render(Game& game);
	virtual void handleEvent(Game& game, int eventType, void* params);	
private:
	GameModeGame();
	bool m_canDynamicChangeTile;
};

class GameModeMapEdit
	: public GameMode
	, public fdk::Singleton<GameModeMapEdit>
{
	friend class fdk::Singleton<GameModeMapEdit>;
public:
	virtual void enter(Game& game);
	virtual void leave(Game& game);
	virtual void update(Game& game, float delta);
	virtual void handleEvent(Game& game, int eventType, void* params);
private:
	GameModeMapEdit();
	bool m_bMouseTracking;
	CellCoord m_lastMouseCoord;
};

class GameModePause
	: public GameMode
	, public fdk::Singleton<GameModePause>
{
	friend class fdk::Singleton<GameModePause>;
public:
	virtual void enter(Game& game);
	virtual void leave(Game& game);
	GameMode* m_lastMode;
};

#define g_Game (Game::instance())
#define g_GameModeGame (GameModeGame::instance())
#define g_GameModeMapEdit (GameModeMapEdit::instance())
#define g_GameModePause (GameModePause::instance())
#endif
