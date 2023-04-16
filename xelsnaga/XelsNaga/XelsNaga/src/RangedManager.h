#pragma once

#include "Common.h"
#include "MicroManager.h"

class XelsNaga;

class RangedManager: public MicroManager
{
public:

    RangedManager(XelsNaga & bot);
    void    executeMicro(const std::vector<UnitTag> & targets);
    void    assignTargets(const std::vector<UnitTag> & targets);
    int     getAttackPriority(const UnitTag & rangedUnit, const UnitTag & target);
    UnitTag getTarget(const UnitTag & rangedUnit, const std::vector<UnitTag> & targets);
};
