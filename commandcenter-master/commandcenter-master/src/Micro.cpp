#include "Micro.h"
#include "Util.h"
#include "XelsNaga.h"

const float dotRadius = 0.1f;

void Micro::SmartStop(const UnitTag & attacker, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(attacker, sc2::ABILITY_ID::STOP);
}

void Micro::SmartAttackUnit(const UnitTag & attacker, const UnitTag & target, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(attacker, sc2::ABILITY_ID::ATTACK_ATTACK, target);
}

void Micro::SmartAttackMove(const UnitTag & attacker, const sc2::Point2D & targetPosition, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(attacker, sc2::ABILITY_ID::ATTACK_ATTACK, targetPosition);
}

void Micro::SmartMove(const UnitTag & attacker, const sc2::Point2D & targetPosition, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(attacker, sc2::ABILITY_ID::MOVE, targetPosition);
}

void Micro::SmartRightClick(const UnitTag & unit, const UnitTag & target, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, target);
}

void Micro::SmartRepair(const UnitTag & unit, const UnitTag & target, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(unit, sc2::ABILITY_ID::SMART, target);
}

void Micro::SmartKiteTarget(const UnitTag & rangedUnit, const UnitTag & target, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(rangedUnit, sc2::ABILITY_ID::ATTACK_ATTACK, target);
}

void Micro::SmartBuild(const UnitTag & builder, const sc2::UnitTypeID & buildingType, sc2::Point2D pos, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(builder, bot.Data(buildingType).buildAbility, pos);
}

void Micro::SmartBuildTag(const UnitTag & builder, const sc2::UnitTypeID & buildingType, UnitTag targetTag, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(builder, bot.Data(buildingType).buildAbility, targetTag);
}

void Micro::SmartTrain(const UnitTag & builder, const sc2::UnitTypeID & buildingType, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(builder, bot.Data(buildingType).buildAbility);
}

void Micro::SmartAbility(const UnitTag & builder, const sc2::AbilityID & abilityID, XelsNaga & bot)
{
    bot.Actions()->UnitCommand(builder, abilityID);
}