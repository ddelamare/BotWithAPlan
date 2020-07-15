#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common\Managers\ArmyManager.h"

class ClearObstructionsGoal : public BaseAction
{
	bool hasSpoken = false;
	const Unit* blockingPath;
public:
	ClearObstructionsGoal() : BaseAction() {
		this->BaseAction::name = "Clear Obstructions";
	}
	double virtual CalculateScore(const sc2::ObservationInterface* obs, GameState* state) {

		blockingPath = nullptr;
		auto townhalls = obs->GetUnits(Unit::Alliance::Self, CompletedUnits(IsTownHall()));
		auto minerals = obs->GetUnits(Unit::Alliance::Neutral, IsSmallMineralPatch());
		std::sort(minerals.begin(), minerals.end(), Sorters::sort_by_tag());
		for (auto& th : townhalls)
		{
			auto blockingMinerals = Util::FindNearbyUnits(&minerals, th->pos, obs, 20.0);
			if (blockingMinerals.size())
			{
				blockingPath = blockingMinerals.front();
				break;
			}
		}

		if (!blockingPath) return 0;

		return 3 *(obs->GetUnits(sc2::Unit::Alliance::Self, IsIdleWorker()).size() > 0);
	};

	bool virtual Excecute(const sc2::ObservationInterface* obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		debug->DebugBoxOut(blockingPath->pos - Point3D(1.0, 1.0, -1.0) , blockingPath->pos + Point3D(1.0, 1.0, 1.0));
		auto idleUnit = obs->GetUnits(sc2::Unit::Alliance::Self, IsIdleWorker()).front();
		bool alreadyMining = idleUnit->orders.size() &&  idleUnit->orders.front().target_unit_tag == blockingPath->tag;
		if (idleUnit && !alreadyMining)
		{
			actions->UnitCommand(idleUnit, ABILITY_ID::HARVEST_GATHER, blockingPath, false);
			return true;
		}

		return false;
	}
};