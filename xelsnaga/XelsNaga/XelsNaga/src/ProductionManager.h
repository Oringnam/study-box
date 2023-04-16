#pragma once
#include "sc2api/sc2_api.h"
#include "Common.h"
#include "BuildOrder.h"
#include "BuildingManager.h"
#include "BuildOrderQueue.h"

class XelsNaga;

class ProductionManager
{
    XelsNaga &       m_bot;

    BuildingManager m_buildingManager;
    BuildOrderQueue m_queue;

	UnitTag nexusNum;

    UnitTag getClosestUnitToPosition(const std::vector<UnitTag> & units, sc2::Point2D closestTo);
    bool    meetsReservedResources(const BuildType & type);
    bool    canMakeNow(UnitTag producer, const BuildType & type);
    bool    detectBuildOrderDeadlock();
    void    setBuildOrder(const BuildOrder & buildOrder);
	// add
	void    renewBuildOrder(const BuildOrder & buildOrder);
    void    create(UnitTag producer, BuildOrderItem & item);
    void    manageBuildOrderQueue();
    int     getFreeMinerals();
    int     getFreeGas();

public:

    ProductionManager(XelsNaga & bot);

    void    onStart();
    void    onFrame();
    void    onUnitDestroy(const sc2::Unit & unit);
    void    drawProductionInformation();

    UnitTag getProducer(const BuildType & type, sc2::Point2D closestTo = sc2::Point2D(0, 0));
};
