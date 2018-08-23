#include "BotWithAPlan.h"
#include "sc2api/sc2_api.h"
#include "Common\UnitFilters.h"
#include <chrono>
#include "Goals\Economy\Probe.h"
#include "Goals\Economy\Pylon.h"
#include "Goals\Economy\Expand.h"
#include "Goals\Economy\Chrono.h"
#include "Goals\Economy\Assimilator.h"
#include "Goals\Army\Zealot.h"
#include "Goals\Army\Stalker.h"
#include "Goals\Army\DarkTemplar.h"
#include "Goals\Army\Gateway.h"
#include "Goals\Army\StarGate.h"
#include "Goals\Army\VoidRay.h"
#include "Goals\Army\Robotics.h"
#include "Goals\Army\Immortal.h"
#include "Goals\Army\Collossus.h"
#include "Goals\Tech\Cybernetics.h"
#include "Goals\Tech\Forge.h"
#include "Goals\Tech\TwilightCouncil.h"
#include "Goals\Tech\RoboticsBay.h"
#include "Goals\Tech\DarkShrine.h"
#include "Goals\Tactics\AllOut.h"
#include "Goals\Tactics\ScoutSweep.h"
#include "Goals\Tactics\PickOffExpo.h"
#include "Goals\Upgrades\Chargelots.h"
#include "Goals\Upgrades\GroundWeaponsUpgrade.h"
#include "Common\Util.h"
using Clock = std::chrono::high_resolution_clock;

BotWithAPlan::BotWithAPlan() 
{
	goalPicker = GoalPicker();
	planner = Planner();
	EconomyGoals = vector<BaseAction*>();
	ArmyGoals = vector<BaseAction*>();

	// Keep that sweet mineral coming
	EconomyGoals.push_back(new PylonGoal());
	EconomyGoals.push_back(new ProbeGoal());
	EconomyGoals.push_back(new ChronoGoal());
	EconomyGoals.push_back(new AssimilatorGoal());
	EconomyGoals.push_back(new GatewayGoal());
	EconomyGoals.push_back(new ExpandGoal());

	// Build Because we Can
	ArmyGoals.push_back(new ZealotGoal());
	ArmyGoals.push_back(new StalkerGoal());
	ArmyGoals.push_back(new ColossusGoal());
	ArmyGoals.push_back(new DarkTemplarGoal());

	// Army Composition Goals
	TacticsGoals.push_back(new ChargelotGoal());
	TacticsGoals.push_back(new GroundWeaponsUpgradeGoal());
	TacticsGoals.push_back(new AllOutGoal());
	TacticsGoals.push_back(new ScoutSweepGoal());
	TacticsGoals.push_back(new PickOffExpoGoal());

	// Steps the planner will consider to fufill goals
	AvailableActions.push_back(new GatewayGoal());
	AvailableActions.push_back(new CyberneticsGoal());
	AvailableActions.push_back(new ForgeGoal());
	AvailableActions.push_back(new RoboticsGoal());
	AvailableActions.push_back(new RoboticsBayGoal());
	AvailableActions.push_back(new TwilightCouncilGoal());
	AvailableActions.push_back(new DarkShrineGoal());
	AvailableActions.push_back(new StarGateGoal());
	AvailableActions.push_back(new VoidRayGoal());
	AvailableActions.push_back(new ColossusGoal());
	AvailableActions.push_back(new ImmortalGoal());

	planner.Init();
	shouldRecalcuate = true;

}

void BotWithAPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
	auto startTime = Clock::now();
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();

	ChooseActionFromGoals(EconomyGoals, obs, actions, query, "Econ");
	ChooseActionFromGoals(ArmyGoals, obs, actions, query, "Army");
	ChooseActionFromGoals(TacticsGoals, obs, actions, query, "Tactics");

	auto idleUnits = obs->GetUnits(Unit::Alliance::Self, IsIdleWorker());
	if (idleUnits.size() > 0)
	{
		//Debug()->DebugSphereOut(idleUnits[0]->pos + Point3D(0, 0, 1), 10);
		auto resource = Util::FindNearestResourceNeedingHarversters(idleUnits[0], obs, query);
		if (resource)
		{
			//Debug()->DebugSphereOut(resource->pos + Point3D(0, 0, 1), 3, Colors::Yellow);
			actions->UnitCommand(idleUnits[0], ABILITY_ID::HARVEST_GATHER, resource);
		}
		else
		{
			// Send to scout
			if (state.ScoutingProbes.size() == 0)
			{
				actions->UnitCommand(idleUnits[0], ABILITY_ID::MOVE, state.EnemyBase);
				state.ScoutingProbes.push_back(idleUnits[0]);
			}
		}
	}
	//
	auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
	for (auto th : townhalls)
	{
		if (th->assigned_harvesters > th->ideal_harvesters)
		{
			// Tell all extra workers to stop so they can be reassigned
			//TODO: only reassign if another town hall needs workers
			auto overWorkers = Util::FindNearbyUnits(IsWorker(), th->pos, obs, query, 15);
			actions->UnitCommand(overWorkers, ABILITY_ID::STOP);
		}
	}

	// Clear out scouting probes if they're not scouting
	for (auto worker : state.ScoutingProbes)
	{
		if (IsIdle()(*worker))
		{
			VectorHelpers::RemoveFromVector(state.ScoutingProbes, worker);
		}

	}


	auto endTime = Clock::now();
	Debug()->DebugTextOut("Loop Time: " + to_string(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()));
	
	Debug()->SendDebug();
}

void BotWithAPlan::ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name)
{
	BaseAction* goal = nullptr;
	if (shouldRecalcuate)
	{
		goal = goalPicker.GetGoal(goals, obs, &state);

		if (goal)
		{
			auto state = planner.GetResourceState(obs);

			auto plan = planner.CalculatePlan(state, goal);
			if (plan.size() > 0)
			{
				nextInPlan = plan[plan.size() - 1];
			}
		}
	}

	// Exceute Goal State
	if (goal)
	{
		Debug()->DebugTextOut(name + " Goal Picked:" + goal->GetName());
		if (nextInPlan && goal != nextInPlan)
			Debug()->DebugTextOut(name + " Goal Step:" + nextInPlan->GetName());
		auto success = nextInPlan->Excecute(obs, actions, query, Debug(), &state);
		if (success)
		{
			shouldRecalcuate = true;
			goal = nullptr;
		}
	}
	else
	{
		Debug()->DebugTextOut("No " + name + " Goal.");
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
