#ifndef __ACTORBANK_H_INCLUDE__
#define __ACTORBANK_H_INCLUDE__
#include <list>
#include <fdk/EventHook.h>
#include <fdk/Module.h>
#include "Types.h"
class Actor;

class ActorBank
	: public fdk::Module<ActorBank>
	, public fdk::EventHook
{
	friend class fdk::Module<ActorBank>;
public:
	typedef std::list<Actor*> Actors;
	static const char* NAME() { return "ActorBank"; }
	bool tick(float delta);
	void draw();
	Actor* createActor(const Location& location, float radius);
	void destroyActor(Actor& actor);
	Actor* findFirstActorConverLocation(const Location& location) const;
	const Actors& getActors() const;
private:
	ActorBank();
	~ActorBank();
	void insertActor(Actor& actor);
	void removeActor(Actor& actor);
	virtual void onEvent(int eventType, void* params);
	Actors m_actors;
	Actor* m_currentActor;
};

inline const ActorBank::Actors& ActorBank::getActors() const
{
	return m_actors;
}

#define g_ActorBank (ActorBank::instance())
#endif
