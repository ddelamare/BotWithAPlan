#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Common/Strategy/Building/AssimilatorStrategy.h"
#include "Common/Util/Util.h"
class AssimilatorGoal : public BaseAction
{
public:
	AssimilatorGoal()
	{
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR, 1));
		name = "Build Assimilator";
	}

	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) override
	{
		auto assimilators = obs->GetUnits(Unit::Alliance::Self, IsGasBuilding());
		auto nexs = obs->GetUnits(Unit::Alliance::Self, CompletedUnits(UNIT_TYPEID::PROTOSS_NEXUS));

		auto geyers = obs->GetUnits(IsGeyser());

		bool openGeysers = false;
		for (auto& nex : nexs)
		{
			auto geys = Util::FindNearbyUnits(&geyers, nex->pos, obs, 15);
			auto assims = Util::FindNearbyUnits(&assimilators, nex->pos, obs, 15);
			if (geys.size() > assims.size())
			{
				openGeysers = true;
				break;
			}
		}

		if (!openGeysers || obs->GetGameLoop() < 2500)
			return 0;
		else
		{
			return 1.6;
		}
	}

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)  override
	{
		auto buildingStrategy = new AssimilatorStrategy(ABILITY_ID::BUILD_ASSIMILATOR, true, true);
		return Util::TryBuildBuilding(ABILITY_ID::BUILD_ASSIMILATOR, UNIT_TYPEID::PROTOSS_ASSIMILATOR, obs, actions, query, debug, state, buildingStrategy);
	}

};