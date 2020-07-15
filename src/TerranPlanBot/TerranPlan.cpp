#include "stdafx.h"
#include "TerranPlan.h"
#include "sc2api/sc2_api.h"
#include "Common\Util\UnitFilters.h"
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
#include "Goals\Army\Techlab.h"
#include "Goals\Army\Marine.h"
#include "Goals\Army\Medivac.h"
#include "Goals\Tactics\AllOut.h"
#include "Goals\Tactics\JustDoIt.h"
#include "Goals\Tactics\ScoutSweep.h"
#include "Goals\Tactics\PickOffExpo.h"
#include "Goals\Tactics\AttackProxy.h"
#include "Goals\Tactics\DefendWithWorkers.h"
#include "Goals\Tech\EngineeringBay.h"
#include "Goals\Upgrades\Stim.h"
#include "Goals\Upgrades\CombatShields.h"
#include "Goals\Upgrades\TerranInfantryWeapons.h"
#include "Common\Util\Util.h"
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
	UpgradeGoals.push_back(new StimGoal());
	UpgradeGoals.push_back(new CombatShieldsGoal());
	UpgradeGoals.push_back(new GroundWeaponsUpgradeGoal());

	//TacticsGoals.push_back(new AllOutGoal());
	TacticsGoals.push_back(new ScoutSweepGoal());
	TacticsGoals.push_back(new PickOffExpoGoal());
	TacticsGoals.push_back(new JustDoitGoal());
	TacticsGoals.push_back(new AllOutGoal());
	TacticsGoals.push_back(new ScoutSweepGoal());
	TacticsGoals.push_back(new PickOffExpoGoal());
	TacticsGoals.push_back(new JustDoitGoal());
	TacticsGoals.push_back(new AttackProxyGoal());
	TacticsGoals.push_back(new DefendWithUnitsGoal());

	// Steps the planner will consider to fufill goals
	AvailableActions.push_back(new BarracksGoal());
	AvailableActions.push_back(new FactoryGoal());
	AvailableActions.push_back(new StarportGoal());
	AvailableActions.push_back(new TechLabGoal());
	AvailableActions.push_back(new EngineeringBayGoal());

	PlanBotBase::Init();
}

void TerranPlan::OnStep() {
	
	PlanBotBase::OnStep();
}

void TerranPlan::OnGameStart() {

	//TODO: Add microManagers

	PlanBotBase::OnGameStart();

#if LADDER_MODE
	Actions()->SendChat("gl hf!");
	Actions()->SendChat("Bot version: 0.1");
#endif
}

void TerranPlan::OnGameEnd()
{
	// If we have no town halls, we probably lost
	auto th = Observation()->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
	this->Lost |= (th.size() == 0);
	auto gameResults = Observation()->GetResults();
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
