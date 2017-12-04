#pragma once

#include "sc2api/sc2_api.h"

#include "MapTools.h"
#include "BaseLocationManager.h"
#include "UnitInfoManager.h"
#include "WorkerManager.h"
#include "BotConfig.h"
#include "GameCommander.h"
#include "BuildingManager.h"
#include "StrategyManager.h"
#include "TechTree.h"
#include "BuildType.h"

class XelsNaga : public sc2::Agent 
{

protected:
    sc2::Race               m_playerRace[2];

    MapTools                m_map;
    BaseLocationManager     m_bases;
    UnitInfoManager         m_unitInfo;
    WorkerManager           m_workers;
    StrategyManager         m_strategy;
    BotConfig               m_config;
    TechTree                m_techTree;

    GameCommander           m_gameCommander;

	std::vector<sc2::Point2D> m_baseLocations;

    void OnError(const std::vector<sc2::ClientError> & client_errors, 
                 const std::vector<std::string> & protocol_errors = {}) override;

public:

    XelsNaga();
    void OnGameStart() override;
    void OnStep() override;

          BotConfig & Config();
          WorkerManager & Workers();
    const BaseLocationManager & Bases() const;
    const MapTools & Map() const;
    const UnitInfoManager & UnitInfo() const;
    const StrategyManager & Strategy() const;
    const TypeData & Data(const sc2::UnitTypeID & type) const;
    const TypeData & Data(const sc2::UpgradeID & type) const;
    const TypeData & Data(const BuildType & type) const;
    const sc2::Race & GetPlayerRace(int player) const;
    sc2::Point2D GetStartLocation() const;
    const sc2::Unit * GetUnit(const UnitTag & tag) const;
	
	// add
	const std::vector<sc2::Point2D> & GetStartLocations() const;
};


class Tester : public XelsNaga {

public:
	Tester() : XelsNaga()
	{}

	void OnGameStart() override {
		// get my race
		auto playerID = Observation()->GetPlayerID();
		for (auto & playerInfo : Observation()->GetGameInfo().player_info)
		{
			if (playerInfo.player_id == playerID)
			{
				m_playerRace[Players::Self] = playerInfo.race_actual;
			}
			else
			{
				m_playerRace[Players::Enemy] = playerInfo.race_requested;
			}
		}

		m_techTree.onStart();
		m_map.onStart();
	}

	void OnStep() override {
		Control()->GetObservation();

		m_map.onFrame();

		Debug()->SendDebug();
	}
};
