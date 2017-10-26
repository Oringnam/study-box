#pragma once

#include "Common.h"
#include "BuildOrder.h"

typedef std::pair<sc2::UnitTypeID, size_t>  UnitPair;
typedef std::vector<UnitPair>               UnitPairVector;

class XelsNaga;

struct Strategy
{
    std::string m_name;
    sc2::Race   m_race;
    int         m_wins;
    int         m_losses;
    BuildOrder  m_buildOrder;
	BuildOrder	m_loopOrder;

    Strategy();
    Strategy(const std::string & name, const sc2::Race & race, const BuildOrder & buildOrder, const BuildOrder & loopOrder);
};

class StrategyManager
{
    XelsNaga & m_bot;

    sc2::Race					    m_selfRace;
    sc2::Race					    m_enemyRace;
    std::map<std::string, Strategy> m_strategies;
    int                             m_totalGamesPlayed;
    const BuildOrder                m_emptyBuildOrder;

    bool  shouldExpandNow() const;
    const UnitPairVector getProtossBuildOrderGoal() const;
    const UnitPairVector getTerranBuildOrderGoal() const;
    const UnitPairVector getZergBuildOrderGoal() const;

public:

    StrategyManager(XelsNaga & bot);

    void onStart();
    void onFrame();
    void onEnd(const bool isWinner);
    void addStrategy(const std::string & name, const Strategy & strategy);
    const UnitPairVector getBuildOrderGoal() const;
    const BuildOrder & getOpeningBookBuildOrder() const;
	const BuildOrder & getLoopBookBuildOrder() const;
    void readStrategyFile(const std::string & str);
};
