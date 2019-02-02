#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/UnitHelpers.h"
#include "Planner/Conditions/HaveGatewayTrainer.h"


class MedivacGoal: public BaseAction
{
public:
	MedivacGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Starport", 2, sc2::UNIT_TYPEID::TERRAN_STARPORT, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::TERRAN_MEDIVAC, 1));
		this->BaseAction::name = "Build Medivac";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto marines = obs->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_MARINE));
		auto medivac = obs->GetUnits(IsUnit(UNIT_TYPEID::TERRAN_MEDIVAC));
		double MARINE_MEDIC_RATIO = 6;	// 1 Medivac per 6 marines
		if (marines.size() > MARINE_MEDIC_RATIO * 2) // Start medivacs after twice the starting ratio
		{
			if (medivac.size())
			{
				auto ratio = (marines.size() / medivac.size()) / MARINE_MEDIC_RATIO;
				return Util::ExponentialIncrease(ratio, .25);
			}
			else
				return 6;
		}
		else
			return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		return Util::TryBuildUnit(ABILITY_ID::TRAIN_MEDIVAC, UNIT_TYPEID::TERRAN_STARPORT, obs, actions);
	}
};