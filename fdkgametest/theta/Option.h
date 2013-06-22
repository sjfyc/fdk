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
		Toggle_ShowCellCoord,
		Toggle_ShowCellID,
	};
	void start();
	void stop();
	bool isOn(Toggle tog) const;
	int getUnitSize() const;
private:
	Option();
	~Option();
	void outputUsage();
	void toggle(Toggle tog);	
	virtual void onEvent(int eventType, void* params);
	int m_toggles;
	int m_unitSize;
};

#define g_Option (Option::instance())
#endif
