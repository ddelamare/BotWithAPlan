#include "BotWithAPlan.h"
#include "sc2api/sc2_api.h"
#include "Common\UnitFilters.h"
#include <chrono>
#include "Goals\Economy\Probe.h"
#include "Goals\Economy\Pylon.h"
#include "Goals\Economy\Chrono.h"
#include "Goals\Economy\Assimilator.h"
#include "Goals\Army\Stalker.h"
#include "Goals\Army\Gateway.h"
#include "Goals\Tech\Cybernetics.h"
#include "Goals\Tactics\AllOut.h"
#include "Common\Util.h"
using Clock = std::chrono::high_resolution_clock;

BotWithAPlan::BotWithAPlan() 
{
	goalPicker = GoalPicker();
	planner = Planner();
	EconomyGoals = vector<BaseAction*>();
	ArmyGoals = vector<BaseAction*>();
	//TODO: Fill goal and dependency dictionaries

	EconomyGoals.push_back(new PylonGoal());
	EconomyGoals.push_back(new ProbeGoal());
	EconomyGoals.push_back(new ChronoGoal());
	EconomyGoals.push_back(new AssimilatorGoal());
	EconomyGoals.push_back(new GatewayGoal());
	EconomyGoals.push_back(new CyberneticsGoal());

	ArmyGoals.push_back(new StalkerGoal());

	TacticsGoals.push_back(new AllOutGoal());

	AvailableActions.push_back(new CyberneticsGoal());
	AvailableActions.push_back(new StalkerGoal());

	planner.Init();
	shouldRecalcuate = true;

}

void BotWithAPlan::OnGameStart() {
	LOG(1) << "Bot initialized" << endl;
	auto nexus = Observation()->GetUnits(IsUnit(UNIT_TYPEID::PROTOSS_NEXUS))[0];
	Actions()->UnitCommand(nexus, ABILITY_ID::SMART, nexus->pos);
	auto enemyLocations = Observation()->GetGameInfo().enemy_start_locations;
	state.EnemyBase = enemyLocations[0];
}

void BotWithAPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
	auto startTime = Clock::now();
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();

	if (shouldRecalcuate)
	{
		//shouldRecalcuate = false;

		econGoal = goalPicker.GetGoal(EconomyGoals, obs, &state);
     
		if (econGoal)
		{
			auto state = planner.GetResourceState(obs);

			auto plan = planner.CalculatePlan(state, econGoal);
			if (plan.size() > 0)
			{
				nextInPlan = plan[plan.size() - 1];
			}
		}
	}

	// Exceute Econ State
	if (econGoal)
	{
		Debug()->DebugTextOut("Econ Picked:" + econGoal->GetName());
		if (nextInPlan)
			Debug()->DebugTextOut("Econ Step:" + nextInPlan->GetName());
		auto success = nextInPlan->Excecute(obs, actions, query, Debug(), &state);
		if (success)
		{
			shouldRecalcuate = true;
			econGoal = nullptr;
		}
	}
	else
	{
		Debug()->DebugTextOut("No Econ Goal.");
	}

	if (shouldRecalcuate)
	{
		armyGoal = goalPicker.GetGoal(ArmyGoals, obs, &state);

		if (armyGoal)
		{
			auto state = planner.GetResourceState(obs);

			auto plan = planner.CalculatePlan(state, armyGoal);
			if (plan.size() > 0)
			{
				nextInArmyPlan = plan[plan.size() - 1];
			}
		}
	}

	// Do army State
	Debug()->DebugTextOut("Army Picked:" + armyGoal->GetName());
	if (nextInArmyPlan)
		Debug()->DebugTextOut("Army Next:" + nextInArmyPlan->GetName());
	auto success = nextInArmyPlan->Excecute(obs, actions, query, Debug(), &state);

	if (success)
	{
		shouldRecalcuate = true;
		armyGoal = nullptr;
	}

	if (shouldRecalcuate)
	{
		armyGoal = goalPicker.GetGoal(TacticsGoals, obs, &state);

		if (armyGoal)
		{
			auto state = planner.GetResourceState(obs);

			auto plan = planner.CalculatePlan(state, armyGoal);
			if (plan.size() > 0)
			{
				nextInArmyPlan = plan[plan.size() - 1];
			}
		}
	}

	if (armyGoal)
	{
		Debug()->DebugTextOut("Tactics Picked:" + armyGoal->GetName());
		if (nextInArmyPlan)
			Debug()->DebugTextOut("Tactics Next:" + nextInArmyPlan->GetName());
		success = nextInArmyPlan->Excecute(obs, actions, query, Debug(), &state);
	}

	auto idleUnits = obs->GetUnits(Unit::Alliance::Self, IsIdleWorker());
	if (idleUnits.size() > 0)
	{
		Debug()->DebugSphereOut(idleUnits[0]->pos + Point3D(0, 0, 1), 10);
		auto resource = FindNearestResourceNeedingHarversters(idleUnits[0], obs, query);
		if (resource)
		{
			// TODO: Send probe to mine
			Debug()->DebugSphereOut(resource->pos + Point3D(0, 0, 1), 3, Colors::Yellow);
			actions->UnitCommand(idleUnits[0], ABILITY_ID::HARVEST_GATHER, resource);
		}
		else
		{
			// What to do here?
		}
	}

	auto endTime = Clock::now();
	Debug()->DebugTextOut("Loop Time: " + to_string(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()));
	Debug()->SendDebug();
}

void BotWithAPlan::OnBuildingConstructionComplete(const Unit*) 
{
	shouldRecalcuate = true;
}

void::BotWithAPlan::OnUnitCreated(const Unit* unit)
{
	if (unit->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
	{
		// Clear Rally Point
		Actions()->UnitCommand(unit, ABILITY_ID::RALLY_WORKERS, unit->pos);
	}
}

#pragma region Bot Ladder Hooks
void *CreateNewAgent()
{
	return (void *)new BotWithAPlan();
}

const char *GetAgentName()
{
	return "BotWithAPlan";
}

int GetAgentRace()
{
	return (int)sc2::Race::Protoss;
}
#pragma endregion
