#pragma once

#include "Common.h"
#include "SquadOrder.h"
#include "Micro.h"

struct AirThreat
{
    UnitTag	unit;
    double weight;
};

struct GroundThreat
{
    UnitTag	unit;
    double weight;
};

class XelsNaga;

class MicroManager
{
    std::vector<UnitTag> m_units;

protected:

    XelsNaga & m_bot;
    SquadOrder order;

    virtual void executeMicro(const std::vector<UnitTag> & targets) = 0;
    void trainSubUnits(const UnitTag & unit) const;


public:

    MicroManager(XelsNaga & bot);

    const std::vector<UnitTag> & getUnits() const;

    void setUnits(const std::vector<UnitTag> & u);
    void execute(const SquadOrder & order);
    void regroup(const sc2::Point2D & regroupPosition) const;

};