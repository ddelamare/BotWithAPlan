#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>

class VoidRayGoal : public BaseAction, public BaseCondition
{
public:
	VoidRayGoal() : BaseAction(), BaseCondition() {
		this->conditions.push_back(new BaseCondition("Build Star Gate", 2, sc2::UNIT_TYPEID::PROTOSS_STARGATE, 1));
		//this->conditions.push_back(new BaseCondition("Build CYBERNETICS", 10, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, 2));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_VOIDRAY, 1));
		this->BaseAction::name = "Build VoidRay";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		return 0;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		bool madeVoid = false;
		auto starGates = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_STARGATE));
		for (auto stargate : starGates) {
			if (stargate->orders.size() == 0)
			{
				actions->UnitCommand(stargate, ABILITY_ID::TRAIN_VOIDRAY);
				madeVoid = true;
			}
		}

		return madeVoid;
	}
};