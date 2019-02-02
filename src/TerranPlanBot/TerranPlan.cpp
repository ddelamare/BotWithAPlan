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
#include "Goals\Army\Factory.h"
#include "Goals\Army\Starport.h"
#include "Goals\Army\Reactor.h"
#include "Goals\Army\Marine.h"
#include "Goals\Army\Medivac.h"
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
	// Keep that sweet mineral coming
	EconomyGoals.push_back(new SupplyDepotGoal());
	EconomyGoals.push_back(new SCVGoal());
	EconomyGoals.push_back(new RefineryGoal());
	EconomyGoals.push_back(new ExpandGoal());
	EconomyGoals.push_back(new BarracksGoal());
	
	BuildOrderGoals.push_back(new ReactorGoal());

	// Build Because we Can
	ArmyGoals.push_back(new MarineGoal());
	ArmyGoals.push_back(new MedivacGoal());

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

	// Steps the planner will consider to fufill goals
	AvailableActions.push_back(new BarracksGoal());
	AvailableActions.push_back(new FactoryGoal());
	AvailableActions.push_back(new StarportGoal());

	PlanBotBase::Init();
}

void TerranPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
	auto startTime = Clock::now();
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();

	UpdateGameState();
	RemoveIdleScouts();
	ChooseGoals();
	BalanceWorkerAssignments();
	DefendBase();
	CancelBuildingsUnderAttack();
	ManageUnits();

	DebugDrawState(startTime);

#if LADDER_MODE
	// This does not seem to work in local tests

	if (ShouldSurrender(obs))
	{
		auto y = Control()->RequestLeaveGame();
	}
#endif

#if DEBUG_MODE	
	Debug()->SendDebug();
#endif
#if REALTIME
	Actions()->SendActions();
#endif
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
