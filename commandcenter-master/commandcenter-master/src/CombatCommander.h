#pragma once

#include "Common.h"
#include "Squad.h"
#include "SquadData.h"

class XelsNaga;

class CombatCommander
{
    XelsNaga &         m_bot;

    SquadData       m_squadData;
    std::vector<UnitTag>  m_combatUnits;
    bool            m_initialized;
    bool            m_attackStarted;

    void            updateWorkerDefenseSquad();
    void            updateDefenseSquads();
    void            updateAttackSquads();
    void            updateIdleSquad();
    bool            isSquadUpdateFrame();


    UnitTag         findClosestDefender(const Squad & defenseSquad, const sc2::Point2D & pos);
//    UnitTag         findClosestWorkerTo(std::vector<UnitTag> & unitsToAssign, const sc2::Point2D & target);
	UnitTag         findClosestWorkerTo(const sc2::Point2D & target);
	UnitTag         findClosestCombatTo(std::vector<UnitTag> & unitsToAssign, const sc2::Point2D & target);

    sc2::Point2D    getMainAttackLocation();

    void            updateDefenseSquadUnits(Squad & defenseSquad, const size_t & flyingDefendersNeeded, const size_t & groundDefendersNeeded);
    bool            shouldWeStartAttacking();

public:

    CombatCommander(XelsNaga & bot);


    void onStart();
    void onFrame(const std::vector<UnitTag> & combatUnits);

    void drawSquadInformation();
};

