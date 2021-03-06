#include "Option.h"
#include "Util.h"

Option::Option()
	: m_toggles(0)
	, m_navigatorType(0)
{
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
			toggle(Toggle_ShowCellClearanceValue);
			util::output("%s cell clearance value", isOn(Toggle_ShowCellClearanceValue) ? "show" : "hide");
		}
		else if (key == HGEK_F1)
		{
			++m_navigatorType;
			if (m_navigatorType >= NavigatorType__Count)
			{
				m_navigatorType = 0;
			}
			util::output("navigator type change to %d", m_navigatorType);
		}
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
	util::output("2: show/hide cell clearance value");
	util::output("F1: change navigator");
}

void Option::start()
{
	outputUsage();
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYDOWN);
	fdk::EventHook::regist(GAME_SYSTEM_EVENT_KEYUP);
}

void Option::stop()
{
	fdk::EventHook::unregist();
}

Option::NavigatorType Option::getNavigatorType() const
{
	return m_navigatorType;
}
