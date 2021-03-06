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
	enum ENavigatorType
	{
		NavigatorType_AStar,
		NavigatorType_Jps,
		NavigatorType_JpsPlus,
		NavigatorType__Count,
	};
	typedef unsigned char NavigatorType;
	enum Toggle
	{
		Toggle_ShowCellCoord,
		Toggle_ShowCellID,
		Toggle_ShowVertex,
		Toggle_ShowActorVertex,	
		Toggle_ShowVertexCoordInMouse,
		Toggle_ShowVertexIDInMouse,
		Toggle_ShowPath,
		Toggle_RandomMove,
		Toggle_Brush,
	};
	void start();
	void stop();
	bool isOn(Toggle tog) const;
	int getMoveCapability() const;
	int getUnitSize() const;
	TileType getBrush() const; 
	NavigatorType getNavigatorType() const; 
	float getTimeRate() const;
private:
	Option();
	~Option();
	void outputUsage();
	void toggle(Toggle tog);	
	virtual void onEvent(int eventType, void* params);
	int m_toggles;
	int m_moveCapability;
	int m_unitSize;	
	TileType m_brush;
	NavigatorType m_navigatorType;
	float m_timeRate;
};

#define g_Option (Option::instance())
#endif
