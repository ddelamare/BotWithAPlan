#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"

class PsiStormGoal : public BaseAction
{
public:
	PsiStormGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Archive", 2, sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, 1));
		this->results.push_back(new BaseResult((sc2::UNIT_TYPEID)sc2::ABILITY_ID::EFFECT_PSISTORM, 1));
		this->BaseAction::name = "Psi Storm";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		auto upgrades = obs->GetUpgrades();
		auto hasUpgrade = VectorHelpers::FoundInVector(upgrades, (UpgradeID)UPGRADE_ID::PSISTORMTECH);
		if (hasUpgrade || Util::DoesAnyUnitHaveOrder(IsUnit(sc2::UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE), ABILITY_ID::RESEARCH_PSISTORM, obs))
			return 0;
		int unitCount = obs->GetFoodArmy();
		score = Util::ExponentialIncrease(unitCount, .1);

		if (score > 9.9)
			score = 9.9;

		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::RESEARCH_PSISTORM, UNIT_TYPEID::PROTOSS_TEMPLARARCHIVE, obs, actions);
	}
};