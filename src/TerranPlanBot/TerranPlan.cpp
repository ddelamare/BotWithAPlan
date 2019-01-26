#include "stdafx.h"
#include "TerranPlan.h"
#include "sc2api/sc2_api.h"
#include "Common\UnitFilters.h"
#include <chrono>
#include <tuple>
#include "Goals\Economy\SCV.h"
#include "Goals\Economy\SupplyDepot.h"
#include "Goals\Economy\ExpandCC.h"
#include "Goals\Economy\Refinery.h"
#include "Goals\Army\Barracks.h"
#include "Goals\Army\Reactor.h"
#include "Goals\Army\Marine.h"
#include "Goals\Tactics\AllOut.h"
#include "Goals\Tactics\JustDoIt.h"
#include "Goals\Tactics\ScoutSweep.h"
#include "Goals\Tactics\PickOffExpo.h"
#include "Goals\Tactics\AttackProxy.h"
#include "Goals\Tactics\DefendWithWorkers.h"
#include "Common\Util.h"
using Clock = std::chrono::high_resolution_clock;

TerranPlan::TerranPlan() 
{
	goalPicker = GoalPicker();
	planner = Planner();
	EconomyGoals = vector<BaseAction*>();
	ArmyGoals = vector<BaseAction*>();
	state.ArmyManager = &armyManager;
	// Keep that sweet mineral coming
	EconomyGoals.push_back(new SupplyDepotGoal());
	EconomyGoals.push_back(new SCVGoal());
	EconomyGoals.push_back(new RefineryGoal());
	EconomyGoals.push_back(new ExpandGoal());
	EconomyGoals.push_back(new BarracksGoal());
	EconomyGoals.push_back(new ReactorGoal());

	// Build Because we Can
	ArmyGoals.push_back(new MarineGoal());

	// Tactics and Upgrade Goals

	//TacticsGoals.push_back(new AllOutGoal());
	TacticsGoals.push_back(new ScoutSweepGoal());
	TacticsGoals.push_back(new PickOffExpoGoal());
	TacticsGoals.push_back(new JustDoitGoal());
	//TacticsGoals.push_back(new AttackProxyGoal());
	//TacticsGoals.push_back(new DefendWithUnitsGoal());
	//TacticsGoals.push_back(new ForgeFastExpandGoal());
	//TacticsGoals.push_back(new OracleHarrassGoal());
	//TacticsGoals.push_back(new RushGoal());
	//TacticsGoals.push_back(new Do4GateGoal());
	//TacticsGoals.push_back(new RushGoal());

//

	// Steps the planner will consider to fufill goals
  	AvailableActions.push_back(new BarracksGoal());


	planner.Init();
	shouldRecalcuate = true;

	Lost = false;
	StepCounter = STEPS_PER_GOAL;
}

void TerranPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
	auto startTime = Clock::now();
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();

	// Store the number of each unit they have
	auto enemyUnits = obs->GetUnits(sc2::Unit::Alliance::Enemy);
	UnitMap cUnits;
	int maxFood = 0;
	for (auto unit : enemyUnits)
	{
		cUnits[unit->unit_type]++;
		maxFood += state.UnitInfo[unit->unit_type].food_required;
	}
	state.MaxEnemyFood = max(state.MaxEnemyFood, maxFood);
	for (auto type : cUnits)
	{
		state.MaxEnemyUnits[type.first] = max(state.MaxEnemyUnits[type.first], type.second);
		LOG(4) << (int)type.first << " " << state.MaxEnemyUnits[type.first];
	}
	
	// Clear out scouting probes if they're not scouting
	for (auto worker : state.ScoutingUnits)
	{
		if (IsIdle()(*worker))
		{
			VectorHelpers::RemoveFromVector(&state.ScoutingUnits, worker);
		}

	}

	if (StepCounter == STEPS_PER_GOAL)
	{
		debugMessages.clear();
		bool stopOthers = false;
		ChooseActionFromGoals(EconomyGoals, obs, actions, query, "Econ", &debugMessages, true, stopOthers);
		ChooseActionFromGoals(ArmyGoals, obs, actions, query, "Army", &debugMessages, true, stopOthers);
		ChooseActionFromGoals(TacticsGoals, obs, actions, query, "Tactics", &debugMessages, false, stopOthers);
		ChooseActionFromGoals(UpgradeGoals, obs, actions, query, "Upgrade", &debugMessages, false, stopOthers);
		StepCounter = 0;
	}
	StepCounter++;

	//
	auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
	Units extraWorkers;
	for (auto th : townhalls)
	{
		if (th->assigned_harvesters > th->ideal_harvesters)
		{
			// Tell all extra workers to stop so they can be reassigned
			//TODO: only reassign if another town hall needs workers
			auto overWorkers = Util::FindNearbyUnits(IsWorker(), th->pos, obs, 15);
			int diff = th->assigned_harvesters - th->ideal_harvesters;
			if (overWorkers.size() < diff) continue; // If no workers in area continue
			extraWorkers = Units(overWorkers.begin(), overWorkers.begin() + diff);
		}
	}

	auto idleUnits = obs->GetUnits(Unit::Alliance::Self, IsIdleWorker());
	idleUnits.insert(idleUnits.end(), extraWorkers.begin(), extraWorkers.end());
	for (int i = 0; i < idleUnits.size() ;i++)
	{
		// Don't reassign scouts
		if (VectorHelpers::FoundInVector(state.ScoutingUnits, idleUnits[i])) continue;

		auto resource = Util::FindNearestResourceNeedingHarversters(idleUnits[i], obs, query);
		if (resource)
		{
			actions->UnitCommand(idleUnits[i], ABILITY_ID::HARVEST_GATHER, resource);
		}
		else
		{
			// Send to scout
			if (state.ScoutingUnits.size() == 0)
			{
				actions->UnitCommand(idleUnits[i], ABILITY_ID::MOVE, state.EnemyBase);
				state.ScoutingUnits.push_back(idleUnits[i]);
			}
			break;
		}
	}
	//auto nexuses = Observation()->GetUnits(Unit::Alliance::Self, IsTownHall());
	//for (auto th : nexuses)
	//{
	//	auto nearbyUnits = Util::FindNearbyUnits(Unit::Alliance::Enemy, IsEnemyArmy(), th->pos, obs, 25);
	//	if (nearbyUnits.size())
	//	{
	//		// RALLY THE TROOPS!
	//		armyManager.SetTarget(&armyManager.battleGroups[0], nearbyUnits[0]->pos);
	//	}
	//}

	armyManager.ManageGroups(obs,query,actions,&state,Debug());


	// Blink micro


	// Morph all gateways to warpgates
	auto gateways = obs->GetUnits(IsUnit(sc2::UNIT_TYPEID::PROTOSS_GATEWAY));
	actions->UnitCommand(gateways, ABILITY_ID::MORPH_WARPGATE);

#if LADDER_MODE
	// This does not seem to work in local tests

	if (ShouldSurrender(obs))
	{
		auto y = Control()->RequestLeaveGame();
	}
#endif


	for (int i = 0; i < state.ExpansionLocations.size(); i++)
	{
		Debug()->DebugTextOut(std::to_string(i + 1), state.ExpansionLocations[i]);
	}

	auto endTime = Clock::now();
	for (auto message : debugMessages)
	{
		Debug()->DebugTextOut(message);
	}
	Debug()->DebugTextOut("Loop Time: " + to_string(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()));
#if DEBUG_MODE	
	Debug()->SendDebug();
#endif
#if REALTIME
	Actions()->SendActions();
#endif
}

void TerranPlan::ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages, bool withRetry, bool& stopOthers)
{
	if (stopOthers) return;
	BaseAction* goal = nullptr;
	BaseAction* nextInPlan = nullptr;
	auto actionList = goalPicker.GetGoals(goals, obs, &state);
	bool allowDependencies = true;
	for (int i = 0; i < actionList.size(); i++)
	{
		if (get<0>(actionList[i]) <= 0) break; // Don't do goals with score 0;

		auto goal = std::get<1>(actionList[i]);
		auto resState = planner.GetResourceState(obs);

		auto plan = planner.CalculatePlan(resState, goal);
		if (plan.size() == 1)
		{
			nextInPlan = plan[0];
		}
		else if (allowDependencies)
		{
			nextInPlan = plan[plan.size() - 1];
		}

		if (goal && nextInPlan)
		{
			auto success = nextInPlan->Excecute(obs, actions, query, Debug(), &state);
			if (success)
			{
				auto msg = name + " GP:" + goal->GetName();
				if (nextInPlan && goal != nextInPlan)
					msg += " GS:" + nextInPlan->GetName();
				messages->push_back(msg);
				goal = nullptr;
				break;
			}
			else if (goal->HoldOtherGoals(obs))
			{
				stopOthers = true;
				break;
			}
		}

		if (!withRetry) break; // If we aren't retrying, break out
		// Only allow the top choice to build it's dependencies. Otherwise we build one of everything
		allowDependencies = false;
	}
}

bool TerranPlan::ShouldSurrender(const sc2::ObservationInterface * obs)
{
	auto probes = obs->GetFoodWorkers();
	auto minerals = obs->GetMinerals();
	auto armyCount = obs->GetArmyCount();

	return probes == 0 && minerals < 50 && armyCount == 0;
}

#pragma region Bot Ladder Hooks
void *CreateNewAgent()
{
	return (void *)new TerranPlan();
}

const char *GetAgentName()
{
	return "TerranPlan";
}

int GetAgentRace()
{
	return (int)sc2::Race::Terran;
}
#pragma endregion
