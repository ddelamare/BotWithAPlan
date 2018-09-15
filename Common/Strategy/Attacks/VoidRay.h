#pragma once
#include "Common/Util.h"
using namespace sc2;

class VoidRayAttack
{
	UnitTypeData unitInfo;
	UnitTypes allUnitInfo;
public:
	VoidRayAttack(const sc2::ObservationInterface *obs, sc2::QueryInterface* query);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
VoidRayAttack::VoidRayAttack(const sc2::ObservationInterface *obs, sc2::QueryInterface* query)
{
	allUnitInfo = obs->GetUnitTypeData();
	unitInfo = allUnitInfo[(int)UNIT_TYPEID::PROTOSS_VOIDRAY];
}
void VoidRayAttack::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	auto voidRays = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_VOIDRAY));
	for (auto vr : voidRays)
	{
		auto enemyUnits = Util::FindNearbyUnits(IsEnemyArmy(), vr->pos, obs, query, unitInfo.sight_range);
		Units armoredUnits;
		for (auto unit : enemyUnits)
		{
			//TODO: move to outside the loop via caching
			auto unitAttr = allUnitInfo[unit->unit_type].attributes;
			bool isArmored = VectorHelpers::FoundInVector(unitAttr, Attribute::Armored);
			if (isArmored) armoredUnits.push_back(unit);
		}
		if (armoredUnits.size())
		{
			actions->UnitCommand(vr, ABILITY_ID::EFFECT_VOIDRAYPRISMATICALIGNMENT);
		}
	}

}