#ifndef __GAME_H_INCLUDE__
#define __GAME_H_INCLUDE__
#include "Types.h"
#include <fdk/EventHook.h>
class GameMode;
class AStar;

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
	bool IsRunning;
	const CellCoord& getStartCoord() const;
	const CellCoord& getTargetCoord() const;
private:
	Game();
	~Game();
	virtual void onEvent(int eventType, void* params);
	void outputUsage();
	void toggleMode();
	GameMode* m_mode;
	CellCoord m_startCoord;
	CellCoord m_targetCoord;
	AStar* m_astar;
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
	virtual void update(Game& game, float delta);
	virtual void render(Game& game);
	virtual void handleEvent(Game& game, int eventType, void* params);
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
	enum EBrush
	{
		Brush_Block,
		Brush_Erase,		
		Brush_Start,
		Brush_Target,		
	};
	GameModeMapEdit();
	bool m_bMouseTracking;
	CellCoord m_lastMouseCoord;
	EBrush m_brush;
};

inline const CellCoord& Game::getStartCoord() const
{
	return m_startCoord;
}

inline const CellCoord& Game::getTargetCoord() const
{
	return m_targetCoord;
}

#define g_Game (Game::instance())
#define g_GameModeGame (GameModeGame::instance())
#define g_GameModeMapEdit (GameModeMapEdit::instance())
#endif
