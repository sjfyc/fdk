#include "Option.h"
#include "Util.h"
#include "Actor.h"
#include "ActorBank.h"

Option::Option()
	: m_toggles(0)
	, m_moveCapability(0)
	, m_unitSize(1)
	, m_brush(Tile_Block)
	, m_navigatorType(0)
{
	fdk::setEnumMask(m_moveCapability, Tile_None);
}

Option::~Option()
{
}

void Option::onEvent(int eventType, void* params)
{
	if (eventType == GAME_SYSTEM_EVENT_KEYUP)
	{
		int key = (int)params;
		if (key == HGEK_1)
		{
			toggle(Toggle_ShowCellCoord);
			util::output("%s cell coord", isOn(Toggle_ShowCellCoord) ? "show" : "hide");
		}
		else if (key == HGEK_2)
		{
			toggle(Toggle_ShowCellID);
			util::output("%s cell id", isOn(Toggle_ShowCellID) ? "show" : "hide");
		}
		else if (key == HGEK_3)
		{
			toggle(Toggle_ShowVertex);
			util::output("%s vertex", isOn(Toggle_ShowVertex) ? "show" : "hide");
		}	
		else if (key == HGEK_4)
		{
			toggle(Toggle_ShowCurrentActorVertex);
			util::output("%s current actor vertex", isOn(Toggle_ShowCurrentActorVertex) ? "show" : "hide");
			Actor* currentActor = g_ActorBank.getCurrentActor();
			if (currentActor)
			{
				currentActor->plotToMapManager(isOn(Toggle_ShowCurrentActorVertex));
			}
		}
		else if (key == HGEK_F1)
		{
			++m_brush;
			if (m_brush >= Tile__Count)
			{
				m_brush = Tile_None;
			}
			util::output("brush change to %s", getTileName(m_brush));
		}
		else if (key == HGEK_F2)
		{			
			if (fdk::hasEnumMask(m_moveCapability, Tile_Water))
			{
				fdk::clearEnumMask(m_moveCapability, Tile_Water);
				util::output("can't move to water");
			}
			else
			{
				fdk::setEnumMask(m_moveCapability, Tile_Water);
				util::output("can move to water");
			}
		}
		else if (key == HGEK_F3)
		{
			++m_navigatorType;
			if (m_navigatorType >= NavigatorType__Count)
			{
				m_navigatorType = 0;
			}
			util::output("navigator type change to %d", m_navigatorType);
		}
		else if (key == HGEK_ADD)
		{
			if (++m_unitSize > MAX_UNIT_SIZE)
			{
				m_unitSize = MAX_UNIT_SIZE;
			}
			util::output("unit size: %d", m_unitSize);
		}
		else if (key == HGEK_SUBTRACT)
		{
			if (--m_unitSize < MIN_UNIT_SIZE)
			{
				m_unitSize = MIN_UNIT_SIZE;
			}
			util::output("unit size: %d", m_unitSize);
		}
	}
	else if (eventType == Event_OnActorSelected)
	{
		Actor* actor = static_cast<Actor*>(params);
		m_moveCapability = actor->getMoveCapability();
		util::output("%s move to water", fdk::hasEnumMask(m_moveCapability, Tile_Water) ? "can" : "can't");
		m_unitSize = actor->getUnitSize();
		util::output("unit size: %d", m_unitSize);
	}
}

void Option::toggle(Toggle tog)
{
	if (isOn(tog))
	{
		m_toggles &= ~(1 << tog);
	}
	else
	{
		m_toggles |= (1 << tog);
	}
}

bool Option::isOn(Toggle tog) const
{
	return (m_toggles & (1 << tog)) ? true : false;
}

void Option::outputUsage()
{
	util::output("1: show/hide cell coord");
	util::output("2: show/hide cell id");
	util::output("3: show/hide vertex");
	util::output("4: show/hide current actor vertex");
	util::output("F1: change brush");
	util::output("F2: change move capability");
	util::output("F3: change navigator");
	util::output("+: increase unit size");
	util::output("-: decrease unit size");
}

void Option::start()
{
	outputUsage();
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYDOWN);
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYUP);
	fdk::EventHook::regist(Event_OnActorSelected);
}

void Option::stop()
{
	fdk::EventHook::unregist();
}

int Option::getMoveCapability() const
{
	return m_moveCapability;
}

int Option::getUnitSize() const
{
	return m_unitSize;
}

TileType Option::getBrush() const
{
	return m_brush;
}

Option::NavigatorType Option::getNavigatorType() const
{
	return m_navigatorType;
}
