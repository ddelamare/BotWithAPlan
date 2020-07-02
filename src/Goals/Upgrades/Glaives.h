#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class GlaivesGoal : public BaseAction
{
public:
	GlaivesGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Twilight", 2, sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES, 1));
		this->BaseAction::name = "Research Glaives";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		auto upgrades = obs->GetUpgrades();
		auto hasUpgrade = VectorHelpers::FoundInVector(upgrades, (UpgradeID)UPGRADE_ID::ADEPTPIERCINGATTACK);
		if (hasUpgrade || Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL), ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES, obs))
			return 0;
		int unitCount = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ADEPT)).size();
		if (unitCount < 5) return 0;

		score = Util::ExponentialIncrease(unitCount, .3);

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUpgrade(ABILITY_ID::RESEARCH_ADEPTRESONATINGGLAIVES, UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, obs, actions, query);
	}
};