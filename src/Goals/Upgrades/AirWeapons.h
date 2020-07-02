#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class AirWeaponsGoal : public BaseAction
{
public:
	uint32_t idx = 0;
	vector<UPGRADE_ID> upgrades;
	vector<ABILITY_ID> abilities;
	AirWeaponsGoal() : BaseAction() {
		upgrades.push_back(UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL1);
		upgrades.push_back(UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL2);
		upgrades.push_back(UPGRADE_ID::PROTOSSAIRWEAPONSLEVEL3);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL2);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL3);

		this->conditions.push_back(new BaseCondition("Build Forge", 2, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::RESEARCH_PROTOSSAIRWEAPONSLEVEL1, 1));
		this->BaseAction::name = "Research Air Weapons";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		if (idx >= upgrades.size()) return 0;
		auto upgradesHad = obs->GetUpgrades();
		auto hasUpgrade = VectorHelpers::FoundInVector(upgradesHad, ((UpgradeID)(upgrades[idx])));
		if (hasUpgrade) idx++; // Move on to the next
		if (hasUpgrade || Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE), abilities[idx], obs))
			return 0;
		if (idx > 0 && Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE), abilities[idx], obs)) return 0;
		
		auto airUnits = obs->GetUnits(sc2::Unit::Alliance::Self, IsFlying());
		auto carriers = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE)).size() * 3; //Carriers need air weapons
		score = Util::ExponentialIncrease(airUnits.size(), (idx + 1)* .08);

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUpgrade(abilities[idx], UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, obs, actions, query);
	}
};