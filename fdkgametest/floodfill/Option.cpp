#include "Option.h"
#include "Util.h"

Option::Option()
	: m_toggles(0)
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
		if (key == HGEK_0)
		{
			toggle(Toggle_ShowCellColor);
			util::output("%s cell color", isOn(Toggle_ShowCellColor) ? "show" : "hide");
		}	
		else if (key == HGEK_1)
		{
			toggle(Toggle_ShowCellCoord);
			util::output("%s cell coord", isOn(Toggle_ShowCellCoord) ? "show" : "hide");
		}	
		else if (key == HGEK_2)
		{
			toggle(Toggle_ShowCenterVertex);
			util::output("%s center vertex", isOn(Toggle_ShowCenterVertex) ? "show" : "hide");
		}
		else if (key == HGEK_3)
		{
			toggle(Toggle_ShowConnectLine);
			util::output("%s connect line", isOn(Toggle_ShowConnectLine) ? "show" : "hide");
		}
		else if (key == HGEK_4)
		{
			toggle(Toggle_ShowConnectors);
			util::output("%s connectors", isOn(Toggle_ShowConnectors) ? "show" : "hide");
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
	util::output("0: show/hide cell color");
	util::output("1: show/hide cell coord");
	util::output("2: show/hide center vertex");
	util::output("3: show/hide connect line");
	util::output("3: show/hide connectors");
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
