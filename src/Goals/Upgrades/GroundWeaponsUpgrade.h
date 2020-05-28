#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class GroundWeaponsUpgradeGoal : public BaseAction
{
public:
	uint32_t idx = 0;
	vector<UPGRADE_ID> upgrades;
	vector<ABILITY_ID> abilities;
	GroundWeaponsUpgradeGoal() : BaseAction() {
		upgrades.push_back(UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL1);
		upgrades.push_back(UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL2);
		upgrades.push_back(UPGRADE_ID::PROTOSSGROUNDWEAPONSLEVEL3);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL2);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL3);
		
		this->conditions.push_back(new BaseCondition("Build Forge", 2, sc2::UNIT_TYPEID::PROTOSS_FORGE, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::RESEARCH_PROTOSSGROUNDWEAPONSLEVEL1, 1));
		this->BaseAction::name = "Research Ground Weapons";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		if (idx >= upgrades.size()) return 0;
		auto upgradesHad = obs->GetUpgrades();
		auto hasUpgrade = VectorHelpers::FoundInVector(upgradesHad, ((UpgradeID)(upgrades[idx])));
		if (hasUpgrade) idx++; // Move on to the next
		if (hasUpgrade || Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::PROTOSS_FORGE), abilities[idx], obs))
			return 0;
		if (idx > 0 && Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::PROTOSS_FORGE), abilities[idx], obs)) return 0;
		if (obs->GetGameLoop() > 4000u * (idx + 1))
		{												   
			score = obs->GetGameLoop() / (6000u * (idx + .1));
		}
		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(abilities[idx], UNIT_TYPEID::PROTOSS_FORGE, obs, actions);
	}
};