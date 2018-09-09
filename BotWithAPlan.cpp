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
#include "Goals\Army\Disrupter.h"
#include "Goals\Army\HighTemplar.h"
#include "Goals\Army\Archon.h"
#include "Goals\Tech\Cybernetics.h"
#include "Goals\Tech\Forge.h"
#include "Goals\Tech\TwilightCouncil.h"
#include "Goals\Tech\RoboticsBay.h"
#include "Goals\Tech\DarkShrine.h"
#include "Goals\Tech\TemplarArchives.h"
#include "Goals\Tactics\AllOut.h"
#include "Goals\Tactics\JustDoIt.h"
#include "Goals\Tactics\ScoutSweep.h"
#include "Goals\Tactics\PickOffExpo.h"
#include "Goals\Upgrades\Chargelots.h"
#include "Goals\Upgrades\GroundWeaponsUpgrade.h"
#include "Goals\Upgrades\Blink.h"
#include "Goals\Upgrades\ThermalLance.h"
#include "Goals\Upgrades\PsiStorm.h"
#include "Common\Strategy\Attacks\BlinkStalker.h"
#include "Common\Strategy\Attacks\DisruptorAttack.h"
#include "Common\Strategy\Attacks\Templar.h"
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
	EconomyGoals.push_back(new RoboticsGoal());
	EconomyGoals.push_back(new ExpandGoal());

	// Build Because we Can
	ArmyGoals.push_back(new ZealotGoal());
	ArmyGoals.push_back(new StalkerGoal());
	ArmyGoals.push_back(new ColossusGoal());
	ArmyGoals.push_back(new ImmortalGoal());
	ArmyGoals.push_back(new DarkTemplarGoal());
	ArmyGoals.push_back(new DisruptorGoal());
	ArmyGoals.push_back(new HighTemplarGoal());
	ArmyGoals.push_back(new ArchonGoal());

	// Tactics and Upgrade Goals
	TacticsGoals.push_back(new ChargelotGoal());
	TacticsGoals.push_back(new BlinkGoal());
	TacticsGoals.push_back(new PsiStormGoal());
	TacticsGoals.push_back(new ThermalLanceGoal());
	TacticsGoals.push_back(new GroundWeaponsUpgradeGoal());
	TacticsGoals.push_back(new AllOutGoal());
	TacticsGoals.push_back(new ScoutSweepGoal());
	TacticsGoals.push_back(new PickOffExpoGoal());
	TacticsGoals.push_back(new JustDoitGoal());

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
	AvailableActions.push_back(new HighTemplarGoal());
	AvailableActions.push_back(new TemplarArchivesGoal());

	planner.Init();
	shouldRecalcuate = true;

	Lost = false;
	StepCounter = STEPS_PER_GOAL;
}

void BotWithAPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
	auto startTime = Clock::now();
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();

	// Store the number of each unit they have
	auto enemyUnits = obs->GetUnits(sc2::Unit::Alliance::Enemy, IsArmy());
	UnitMap cUnits;
	for (auto unit : enemyUnits)
	{
		cUnits[unit->unit_type]++;
	}
	for (auto type : cUnits)
	{
		state.MaxEnemyUnits[type.first] = max(state.MaxEnemyUnits[type.first], type.second);
		LOG(4) << (int)type.first << " " << state.MaxEnemyUnits[type.first];
	}
	

	if (StepCounter == STEPS_PER_GOAL)
	{
		debugMessages.clear();
		ChooseActionFromGoals(EconomyGoals, obs, actions, query, "Econ", &debugMessages);
		ChooseActionFromGoals(ArmyGoals, obs, actions, query, "Army", &debugMessages);
		ChooseActionFromGoals(TacticsGoals, obs, actions, query, "Tactics", &debugMessages);
		StepCounter = 0;
	}
	StepCounter++;

	//
	auto townhalls = obs->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
	if (townhalls.size() == 0) Lost = true;
	for (auto th : townhalls)
	{
		if (th->assigned_harvesters > th->ideal_harvesters)
		{
			// Tell all extra workers to stop so they can be reassigned
			//TODO: only reassign if another town hall needs workers
			auto overWorkers = Util::FindNearbyUnits(IsWorker(), th->pos, obs, query, 15);
			int diff = th->assigned_harvesters - th->ideal_harvesters;
			if (overWorkers.size() < diff) continue; // If no workers in area continue
			auto unitsToStop = Units(overWorkers.begin(), overWorkers.begin() + diff);
			actions->UnitCommand(unitsToStop, ABILITY_ID::STOP);
		}
	}

	auto idleUnits = obs->GetUnits(Unit::Alliance::Self, IsIdleWorker());
	for (int i = 0; i < idleUnits.size() ;i++)
	{
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


	// Clear out scouting probes if they're not scouting
	for (auto worker : state.ScoutingUnits)
	{
		if (IsIdle()(*worker))
		{
			VectorHelpers::RemoveFromVector(&state.ScoutingUnits, worker);
		}

	}

	// Blink micro
	auto bsMicro = BlinkStalker(obs, query);
	bsMicro.Execute(obs, actions, query, Debug(), &state);
	auto disruptorMicro = DisruptorAttack(obs, query);
	disruptorMicro.Execute(obs, actions, query, Debug(), &state);
	auto tMicro = TemplarMicro(obs, query);
	tMicro.Execute(obs, actions, query, Debug(), &state);

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
}

void BotWithAPlan::ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages)
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
		auto msg = name + " GP:" + goal->GetName();
		if (nextInPlan && goal != nextInPlan)
			msg +=" GS:" + nextInPlan->GetName();
		auto success = nextInPlan->Excecute(obs, actions, query, Debug(), &state);
		if (success)
		{
			shouldRecalcuate = true;
			goal = nullptr;
		}
		messages->push_back(msg);
	}
	else
	{
		messages->push_back("No " + name + " Goal.");
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
