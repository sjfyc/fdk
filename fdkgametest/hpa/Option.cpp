#include "Option.h"
#include "Util.h"
#include "Board.h"
#include "HpaMap.h"

Option::Option()
	: m_toggles(0)
	, m_unitSize(1)
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
			toggle(Toggle_ShowCellID);
			util::output("%s cell id", isOn(Toggle_ShowCellID) ? "show" : "hide");
		}
		else if (key == HGEK_3)
		{
			toggle(Toggle_ShowCellClearanceValue);
			util::output("%s cell clearance value", isOn(Toggle_ShowCellClearanceValue) ? "show" : "hide");
		}
		else if (key == HGEK_4)
		{
			toggle(Toggle_ShowClusterCoord);
			util::output("%s cluster coord", isOn(Toggle_ShowClusterCoord) ? "show" : "hide");
		}
		else if (key == HGEK_5)
		{
			toggle(Toggle_ShowPortID);
			util::output("%s port id", isOn(Toggle_ShowPortID) ? "show" : "hide");
		}
		else if (key == HGEK_6)
		{
			toggle(Toggle_ShowIntraEdge);
			util::output("%s intra-edge", isOn(Toggle_ShowIntraEdge) ? "show" : "hide");
		}		
		else if (key == HGEK_ADD)
		{
			if (++m_unitSize > 4)
			{
				m_unitSize = 4;
			}
			util::output("unit size: %d", m_unitSize);
		}
		else if (key == HGEK_SUBTRACT)
		{
			if (--m_unitSize < 1)
			{
				m_unitSize = 1;
			}
			util::output("unit size: %d", m_unitSize);
		}

		if (key == HGEK_ADD || key == HGEK_SUBTRACT)
		{
			g_Board.setMinClearanceValueRequired(m_unitSize);
			g_HpaMap.rebuildAbstract();
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
	util::output("2: show/hide cell id");
	util::output("3: show/hide cell clearance value");
	util::output("4: show/hide cluster coord");
	util::output("5: show/hide port id");
	util::output("6: show/hide intra-edge");	
	util::output("+: increase unit size");
	util::output("-: decrease unit size");
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

int Option::getUnitSize() const
{
	return m_unitSize;
}
