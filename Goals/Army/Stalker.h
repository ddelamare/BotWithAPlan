#pragma once
#include "../../Planner/Actions/BaseAction.h"
#include "../../Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "../../Common/Resource.h"
#include "../Tech/Cybernetics.h"

class StalkerGoal : public BaseAction, public BaseCondition
{
public:
	StalkerGoal() : BaseAction(), BaseCondition() {
		this->conditions.push_back((BaseCondition*)new CyberneticsGoal());
		this->conditions.push_back(new BaseCondition("Build CYBERNETICS", 2, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		//this->conditions.push_back(new BaseCondition("Build CYBERNETICS", 10, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, 2));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_STALKER, 1));
		this->BaseAction::name = "Build Stalker";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		return score + 2;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeStaker = false;
		auto gateways = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_GATEWAY));
		for (auto gateway : gateways) {
			if (gateway->orders.size() == 0)
			{
				actions->UnitCommand(gateway, ABILITY_ID::TRAIN_STALKER);
				madeStaker = true;
			}
		}

		return madeStaker;
	}
};