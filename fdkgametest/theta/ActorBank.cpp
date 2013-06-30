#include "ActorBank.h"
#include "Actor.h"
#include "Option.h"
#include "Game.h"

ActorBank::ActorBank()
	: m_currentActor(0)
{
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYDOWN);
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYUP);
}

ActorBank::~ActorBank()
{
	fdk::EventHook::unregist();
}

bool ActorBank::tick(float delta)
{
	if (!g_Game.isInGameMode())
	{
		return true;
	}
	for (Actors::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it)
	{
		Actor* actor = *it;
		actor->tick(delta);
	}
	return true;
}

void ActorBank::draw()
{
	if (m_currentActor)
	{
		float offset = 0.f;
		g_HGE.FrameRect(
			m_currentActor->getLocation().x-m_currentActor->getRadius()-offset, 
			m_currentActor->getLocation().y-m_currentActor->getRadius()-offset,
			m_currentActor->getLocation().x+m_currentActor->getRadius()+offset, 
			m_currentActor->getLocation().y+m_currentActor->getRadius()+offset,
			0xFF100FFF);
	}
	for (Actors::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it)
	{
		Actor* actor = *it;
		actor->draw();
	}
}

Actor* ActorBank::createActor(const Location& location)
{
	Actor* actor = new Actor(location, g_Option.getMoveCapability(), g_Option.getUnitSize());
	insertActor(*actor);
	return actor;
}

void ActorBank::destroyActor(Actor& actor)
{
	if (m_currentActor == &actor)
	{
		m_currentActor = 0;
	}
	removeActor(actor);
	delete &actor;
}

void ActorBank::insertActor(Actor& actor)
{
	m_actors.push_back(&actor);
}

void ActorBank::removeActor(Actor& actor)
{
	m_actors.remove(&actor);
}

Actor* ActorBank::findFirstActorConverLocation(const Location& location) const
{
	for (Actors::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it)
	{
		Actor* actor = *it;
		if ((actor->getLocation()-location).lengthSquared() < actor->getRadius()*actor->getRadius())
		{
			return actor;
		}
	}
	return 0;
}

void ActorBank::onEvent(int eventType, void* params)
{
	if (!g_Game.isInGameMode())
	{
		if (eventType == GAME_SYSTEM_EVENT_KEYUP)
		{
			int key = (int)params;	
			if (key == HGEK_LBUTTON)
			{
				Location mouseLocation;
				g_HGE->Input_GetMousePos(&mouseLocation.x, &mouseLocation.y);
				Actor* actor = findFirstActorConverLocation(mouseLocation);
				if (actor && m_currentActor != actor)
				{
					m_currentActor = actor;
					fdk::EventCenter::instance().send(Event_OnActorSelected, actor);
				}
			}
		}
		return;
	}
	if (eventType == GAME_SYSTEM_EVENT_KEYUP)
	{
		int key = (int)params;	
		if (key == HGEK_A)
		{
			createActor(Location(CELL_SIZE_X+CELL_SIZE_X/2, CELL_SIZE_Y+CELL_SIZE_Y/2));
		}
		else if (key == HGEK_LBUTTON)
		{
			Location mouseLocation;
			g_HGE->Input_GetMousePos(&mouseLocation.x, &mouseLocation.y);
			Actor* actor = findFirstActorConverLocation(mouseLocation);
			if (actor && m_currentActor != actor)
			{
				m_currentActor = actor;
				fdk::EventCenter::instance().send(Event_OnActorSelected, actor);
			}
		}
		else if (key == HGEK_RBUTTON)
		{
			Location mouseLocation;
			g_HGE->Input_GetMousePos(&mouseLocation.x, &mouseLocation.y);
			if (m_currentActor)
			{
				m_currentActor->searchPath(mouseLocation, false);
			}
		}
	}
}

void ActorBank::getActors(std::vector<Actor*>& output, const Location& center, float radius, const Actor* except) const
{
	for (Actors::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it)
	{
		Actor* actor = *it;
		if (actor == except)
		{
			continue;
		}
		if ((actor->getLocation()-center).lengthSquared() < radius*radius)
		{
			output.push_back(actor);
		}
	}
}

Actor* ActorBank::getFirstCollideActor(const Location& center, float radius, const Actor* except) const
{
	for (Actors::const_iterator it = m_actors.begin(); it != m_actors.end(); ++it)
	{
		Actor* actor = *it;
		if (actor == except)
		{
			continue;
		}
		if ((actor->getLocation()-center).lengthSquared() < 
			(actor->getRadius()+radius)*(actor->getRadius()+radius) )
		{
			return actor;
		}
	}
	return 0;
}

Actor* ActorBank::getCurrentActor() const
{
	return m_currentActor;
}
