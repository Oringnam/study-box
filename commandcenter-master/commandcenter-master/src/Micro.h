#pragma once

#include "Common.h"
#include "sc2api/sc2_api.h"

class XelsNaga;

namespace Micro
{   
    void SmartStop      (const UnitTag & attacker,  XelsNaga & bot);
    void SmartAttackUnit(const UnitTag & attacker,  const UnitTag & target, XelsNaga & bot);
    void SmartAttackMove(const UnitTag & attacker,  const sc2::Point2D & targetPosition, XelsNaga & bot);
    void SmartMove      (const UnitTag & attacker,  const sc2::Point2D & targetPosition, XelsNaga & bot);
    void SmartRightClick(const UnitTag & unit,      const UnitTag & target, XelsNaga & bot);
    void SmartRepair    (const UnitTag & unit,      const UnitTag & target, XelsNaga & bot);
    void SmartKiteTarget(const UnitTag & rangedUnit,const UnitTag & target, XelsNaga & bot);
    void SmartBuild     (const UnitTag & builder,   const sc2::UnitTypeID & buildingType, sc2::Point2D pos, XelsNaga & bot);
    void SmartBuildTag  (const UnitTag & builder,   const sc2::UnitTypeID & buildingType, UnitTag targetTag, XelsNaga & bot);
    void SmartTrain     (const UnitTag & builder,   const sc2::UnitTypeID & buildingType, XelsNaga & bot);
    void SmartAbility   (const UnitTag & builder,   const sc2::AbilityID & abilityID, XelsNaga & bot);
};