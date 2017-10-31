#include "sc2api/sc2_api.h"

#include "XelsNaga.h"
#include "Util.h"

XelsNaga::XelsNaga()
    : m_map(*this)
    , m_bases(*this)
    , m_unitInfo(*this)
    , m_workers(*this)
    , m_gameCommander(*this)
    , m_strategy(*this)
    , m_techTree(*this)
{
    
}

void XelsNaga::OnGameStart() 
{
    m_config.readConfigFile();
    
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
    m_strategy.onStart();
    m_map.onStart();
    m_unitInfo.onStart();
    m_bases.onStart();
    m_workers.onStart();

    m_gameCommander.onStart();
}

void XelsNaga::OnStep()
{
    Control()->GetObservation();

    m_map.onFrame();
    m_unitInfo.onFrame();
    m_bases.onFrame();
    m_workers.onFrame();
    m_strategy.onFrame();

    m_gameCommander.onFrame();

    Debug()->SendDebug();
}

// TODO: Figure out my race
const sc2::Race & XelsNaga::GetPlayerRace(int player) const
{
    BOT_ASSERT(player == Players::Self || player == Players::Enemy, "invalid player for GetPlayerRace");
    return m_playerRace[player];
}

BotConfig & XelsNaga::Config()
{
     return m_config;
}

const MapTools & XelsNaga::Map() const
{
    return m_map;
}

const StrategyManager & XelsNaga::Strategy() const
{
    return m_strategy;
}

const BaseLocationManager & XelsNaga::Bases() const
{
    return m_bases;
}

const UnitInfoManager & XelsNaga::UnitInfo() const
{
    return m_unitInfo;
}

const TypeData & XelsNaga::Data(const sc2::UnitTypeID & type) const
{
    return m_techTree.getData(type);
}

const TypeData & XelsNaga::Data(const sc2::UpgradeID & type) const
{
    return m_techTree.getData(type);
}

const TypeData & XelsNaga::Data(const BuildType & type) const
{
    return m_techTree.getData(type);
}

WorkerManager & XelsNaga::Workers()
{
    return m_workers;
}

const sc2::Unit * XelsNaga::GetUnit(const UnitTag & tag) const
{
	return Observation()->GetUnit(tag);	
}

sc2::Point2D XelsNaga::GetStartLocation() const
{
    return Observation()->GetStartLocation();
}

void XelsNaga::OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors)
{
    
}