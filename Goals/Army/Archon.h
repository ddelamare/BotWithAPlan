#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Goals/Tech/Cybernetics.h"

class ArchonGoal : public BaseAction
{
	std::vector<UNIT_TYPEID> morphableUnits;
public:
	ArchonGoal() : BaseAction() {
		this->conditions.push_back(new BaseCondition("Build Templar", 2, sc2::UNIT_TYPEID::PROTOSS_HIGHTEMPLAR, 2));
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ARCHON, 1));
		this->BaseAction::name = "Build Archon";
		morphableUnits.push_back(UNIT_TYPEID::PROTOSS_DARKTEMPLAR);
		morphableUnits.push_back(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR);
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 0;
		return score; // This goal currently stalls and never complets. FIXME!!!
		if (   state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_MUTALISK] >= 5 
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_HYDRALISK] >= 5 
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::TERRAN_MARINE] >= 10
			|| state->MaxEnemyUnits[sc2::UNIT_TYPEID::ZERG_BROODLORD] >= 2)
		{
			int unitFood = 4 * obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_ARCHON)).size();
			auto units = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnits(morphableUnits));
			// Count templar who are on the way to being archons
			for (auto unit : units)
			{
				if (unit->orders.size() && unit->orders[0].ability_id == ABILITY_ID::MORPH_ARCHON)
				{
					unitFood += 2;
				}
			}
			auto percent = (double)unitFood / (1 + obs->GetFoodArmy()); // Get percent archons
			score = Util::FeedbackFunction(percent, .35, 2.5);
		}
		if (obs->GetMinerals() < 500 && obs->GetVespene() > 2000)
		{
			score *= 3;
		}
		return score;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		Units templar = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_HIGHTEMPLAR));
		if (templar.size() > 1) {
			Units templar_merge;
			for (int i = 0; i < 2; ++i) {
				templar_merge.push_back(templar.at(i));
			}
			actions->UnitCommand(templar_merge, ABILITY_ID::MORPH_ARCHON);
			return true;
		}
		return false;
	}
};