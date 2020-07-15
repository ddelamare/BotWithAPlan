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
	RushAnalyzer rushAnalyzer;
	GroundWeaponsUpgradeGoal() : BaseAction() {
		upgrades.push_back(UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL1);
		upgrades.push_back(UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL2);
		upgrades.push_back(UPGRADE_ID::TERRANINFANTRYWEAPONSLEVEL3);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL2);
		abilities.push_back(sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL3);

		this->conditions.push_back(new BaseCondition("Build Engineering Bay", 2, sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::RESEARCH_TERRANINFANTRYWEAPONSLEVEL1, 1));
		this->BaseAction::name = "Research Ground Weapons";
	}
	double virtual CalculateScore(const sc2::ObservationInterface* obs, GameState* state) {
		double score = 0;

		auto rushChance = rushAnalyzer.GetRushPossibiliy(obs);
		if (idx >= upgrades.size() || rushChance > 1) return 0;
		auto upgradesHad = obs->GetUpgrades();
		auto hasUpgrade = VectorHelpers::FoundInVector(upgradesHad, ((UpgradeID)(upgrades[idx])));
		if (hasUpgrade) idx++; // Move on to the next
		if (hasUpgrade || Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY), abilities[idx], obs))
			return 0;
		if (idx > 0 && Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::TERRAN_ENGINEERINGBAY), abilities[idx], obs)) return 0;
		if (obs->GetGameLoop() > 4000u * (idx + 1))
		{
			score = obs->GetGameLoop() / (6000u * (idx + .1));
		}

		if (score >= 9)
			score = 9;

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUpgrade(abilities[idx], UNIT_TYPEID::TERRAN_ENGINEERINGBAY, obs, actions, query);
	}
};