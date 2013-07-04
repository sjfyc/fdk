#ifndef __Option_H_INCLUDE__
#define __Option_H_INCLUDE__
#include "Types.h"
#include <fdk/EventHook.h>

class Option
	: public fdk::Singleton<Option>
	, public fdk::EventHook
{
	friend class fdk::Singleton<Option>;
public:	
	enum Toggle
	{
		Toggle_ShowCellColor,
		Toggle_ShowCellCoord,
		Toggle_ShowCenterVertex,
		Toggle_ShowConnectLine,
		Toggle_ShowConnectors,
	};
	void start();
	void stop();
	bool isOn(Toggle tog) const;
private:
	Option();
	~Option();
	void outputUsage();
	void toggle(Toggle tog);	
	virtual void onEvent(int eventType, void* params);
	int m_toggles;
};

#define g_Option (Option::instance())
#endif
