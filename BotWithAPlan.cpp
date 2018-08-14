#include "BotWithAPlan.h"
#include "sc2api/sc2_api.h"
#include "Common\UnitFilters.h"
#include <chrono>
#include "Goals\Economy\Probe.h"
#include "Goals\Economy\Pylon.h"
#include "Goals\Economy\Expand.h"
#include "Goals\Economy\Chrono.h"
#include "Goals\Economy\Assimilator.h"
#include "Goals\Army\Stalker.h"
#include "Goals\Army\DarkTemplar.h"
#include "Goals\Army\Gateway.h"
#include "Goals\Army\StarGate.h"
#include "Goals\Army\VoidRay.h"
#include "Goals\Tech\Cybernetics.h"
#include "Goals\Tech\TwilightCouncil.h"
#include "Goals\Tech\DarkShrine.h"
#include "Goals\Tactics\AllOut.h"
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
	ArmyGoals.push_back(new StalkerGoal());
	ArmyGoals.push_back(new DarkTemplarGoal());

	// Army Composition Goals
	TacticsGoals.push_back(new AllOutGoal());

	// Steps the planner will consider to fufill goals
	AvailableActions.push_back(new GatewayGoal());
	AvailableActions.push_back(new CyberneticsGoal());
	AvailableActions.push_back(new TwilightCouncilGoal());
	AvailableActions.push_back(new DarkShrineGoal());
	AvailableActions.push_back(new StarGateGoal());
	AvailableActions.push_back(new VoidRayGoal());

	planner.Init();
	shouldRecalcuate = true;

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
		if (nextInPlan && econGoal != nextInPlan)
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
	if (nextInArmyPlan && nextInArmyPlan != armyGoal)
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
		if (nextInArmyPlan && nextInArmyPlan != armyGoal)
			Debug()->DebugTextOut("Tactics Next:" + nextInArmyPlan->GetName());
		success = nextInArmyPlan->Excecute(obs, actions, query, Debug(), &state);
	}

	auto idleUnits = obs->GetUnits(Unit::Alliance::Self, IsIdleWorker());
	if (idleUnits.size() > 0)
	{
		Debug()->DebugSphereOut(idleUnits[0]->pos + Point3D(0, 0, 1), 10);
		auto resource = Util().FindNearestResourceNeedingHarversters(idleUnits[0], obs, query);
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
	
	auto nexii = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
	Point3D offset = Point3D(nexii[0]->radius, nexii[0]->radius, nexii[0]->pos.z + 3);
	Debug()->DebugSphereOut(nexii[0]->pos + state.MineralDirection, nexii[0]->radius, Colors::Gray);
	
	Debug()->SendDebug();
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
