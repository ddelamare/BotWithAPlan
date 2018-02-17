#pragma once
#include "../../Planner/Actions/BaseAction.h"
#include "../../Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "../../Common/Resource.h"

class CyberneticsGoal : public BaseAction
{
public:
	CyberneticsGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build CYBERNETICS", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
		name = "Build Cybernetics";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs) {
		double score = 0;
		return score + 1;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query)
	{
		bool madePylon = false;
		//TODO: Find location
		//TODO: Find Nearby Probe
		//TODO: Build pylon

		return madePylon;
	}
};