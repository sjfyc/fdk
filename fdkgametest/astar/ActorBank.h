#ifndef __ACTORBANK_H_INCLUDE__
#define __ACTORBANK_H_INCLUDE__
#include <list>
#include <fdk/EventHook.h>
#include "Types.h"
class Actor;

class ActorBank
	: public fdk::Singleton<ActorBank>
	, public fdk::EventHook
{
	friend class fdk::Singleton<ActorBank>;
public:
	typedef std::list<Actor*> Actors;
	void update(float delta);
	void draw();
	Actor* createActor(const Location& location, float radius);
	void destroyActor(Actor& actor);
	Actor* findFirstActorConverLocation(const Location& location) const;
private:
	ActorBank();
	~ActorBank();
	void insertActor(Actor& actor);
	void removeActor(Actor& actor);
	virtual void onEvent(int eventType, void* params);
	Actors m_actors;
	Actor* m_currentActor;
};

#define g_ActorBank (ActorBank::instance())
#endif
