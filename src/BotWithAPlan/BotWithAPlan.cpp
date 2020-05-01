#include "stdafx.h"
#include "BotWithAPlan.h"
#include "sc2api/sc2_api.h"
#include "Common\UnitFilters.h"
#include <tuple>
#include "Goals\Economy\Probe.h"
#include "Goals\Economy\Pylon.h"
#include "Goals\Economy\ExpandNexus.h"
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
#include "Goals\Army\Adept.h"
#include "Goals\Army\Observer.h"
#include "Goals\Army\Phoenix.h"
#include "Goals\Army\Sentry.h"
#include "Goals\Army\Carrier.h"
#include "Goals\Army\Tempest.h"
#include "Goals\Army\PhotonCannon.h"
#include "Goals\Army\ShieldBattery.h"
#include "Goals\Army\Oracle.h"
#include "Goals\Tech\Cybernetics.h"
#include "Goals\Tech\Forge.h"
#include "Goals\Tech\TwilightCouncil.h"
#include "Goals\Tech\RoboticsBay.h"
#include "Goals\Tech\DarkShrine.h"
#include "Goals\Tech\TemplarArchives.h"
#include "Goals\Tech\FleetBeacon.h"
#include "Goals\Tactics\AllOut.h"
#include "Goals\Tactics\JustDoIt.h"
#include "Goals\Tactics\ScoutSweep.h"
#include "Goals\Tactics\ScoutMain.h"
#include "Goals\Tactics\PickOffExpo.h"
#include "Goals\Tactics\Rush.h"
#include "Goals\Tactics\AttackProxy.h"
#include "Goals\Tactics\DefendWithWorkers.h"
#include "Goals\Tactics\4Gate.h"
#include "Goals\Tactics\ForgeFastExpand.h"
#include "Goals\Tactics\OracleHarrass.h"
#include "Goals\Tactics\SuperCannonDefense.h"
#include "Goals\Tactics\ScoutEverything.h"
#include "Goals\Tactics\RetreatGoal.h"
#include "Goals\Upgrades\Chargelots.h"
#include "Goals\Upgrades\GroundWeaponsUpgrade.h"
#include "Goals\Upgrades\GroundArmor.h"
#include "Goals\Upgrades\Shields.h"
#include "Goals\Upgrades\Blink.h"
#include "Goals\Upgrades\Glaives.h"
#include "Goals\Upgrades\ThermalLance.h"
#include "Goals\Upgrades\PsiStorm.h"
#include "Goals\Upgrades\WarpGate.h"
#include "Goals\Upgrades\AirWeapons.h"
#include "Goals\Upgrades\AirArmor.h"
#include "Common\Util.h"
using Clock = std::chrono::high_resolution_clock;

BotWithAPlan::BotWithAPlan() 
{
	// Keep that sweet mineral coming
	EconomyGoals.push_back(new PylonGoal());
	EconomyGoals.push_back(new ProbeGoal());
	EconomyGoals.push_back(new ChronoGoal());
	EconomyGoals.push_back(new AssimilatorGoal());
	EconomyGoals.push_back(new GatewayGoal());
	EconomyGoals.push_back(new RoboticsGoal());
	EconomyGoals.push_back(new StarGateGoal());
	EconomyGoals.push_back(new ExpandGoal());
	EconomyGoals.push_back(new PhotonCannonGoal());
	EconomyGoals.push_back(new ShieldBatteryGoal());


	// Build Because we Can
	ArmyGoals.push_back(new ZealotGoal());
	ArmyGoals.push_back(new StalkerGoal());
	//ArmyGoals.push_back(new AdeptGoal());
	ArmyGoals.push_back(new ColossusGoal());
	ArmyGoals.push_back(new VoidRayGoal());
	ArmyGoals.push_back(new ImmortalGoal());
	ArmyGoals.push_back(new DarkTemplarGoal());
	ArmyGoals.push_back(new DisruptorGoal());
	ArmyGoals.push_back(new HighTemplarGoal());
	ArmyGoals.push_back(new ArchonGoal());
	ArmyGoals.push_back(new ObserverGoal());
	ArmyGoals.push_back(new PhoenixGoal());
	ArmyGoals.push_back(new SentryGoal());
	ArmyGoals.push_back(new CarrierGoal());
	ArmyGoals.push_back(new TempestGoal());
	ArmyGoals.push_back(new OracleGoal());
	ArmyGoals.push_back(new PhotonCannonGoal());

	// Tactics and Upgrade Goals

	TacticsGoals.push_back(new AllOutGoal());
	TacticsGoals.push_back(new ScoutSweepGoal());
	TacticsGoals.push_back(new ScoutEverythingGoal());
	TacticsGoals.push_back(new ScoutMainGoal());
	TacticsGoals.push_back(new PickOffExpoGoal());
	TacticsGoals.push_back(new JustDoitGoal());
	TacticsGoals.push_back(new AttackProxyGoal());
	TacticsGoals.push_back(new DefendWithUnitsGoal());
	TacticsGoals.push_back(new ForgeFastExpandGoal());
	TacticsGoals.push_back(new OracleHarrassGoal());
	//TacticsGoals.push_back(new CannonDefenseGoal());
	//TacticsGoals.push_back(new RushGoal());
	TacticsGoals.push_back(new Do4GateGoal());
	TacticsGoals.push_back(new RetreatGoal());


	UpgradeGoals.push_back(new ChargelotGoal());
	UpgradeGoals.push_back(new BlinkGoal());
	UpgradeGoals.push_back(new PsiStormGoal());
	UpgradeGoals.push_back(new ThermalLanceGoal());
	UpgradeGoals.push_back(new GroundWeaponsUpgradeGoal());
	UpgradeGoals.push_back(new GroundArmorUpgradeGoal());
	UpgradeGoals.push_back(new ShieldUpgradeGoal());
	UpgradeGoals.push_back(new GlaivesGoal());
	UpgradeGoals.push_back(new WarpGateGoal());
	UpgradeGoals.push_back(new AirWeaponsGoal());
	UpgradeGoals.push_back(new AirArmorsGoal());

	// Steps the planner will consider to fufill goals
	AvailableActions.push_back(new AdeptGoal());
	AvailableActions.push_back(new ArchonGoal());
	AvailableActions.push_back(new AssimilatorGoal());
	AvailableActions.push_back(new CarrierGoal());
	AvailableActions.push_back(new ColossusGoal());
	AvailableActions.push_back(new CyberneticsGoal());
	AvailableActions.push_back(new DarkShrineGoal());
	AvailableActions.push_back(new DarkTemplarGoal());
	AvailableActions.push_back(new DisruptorGoal());
	AvailableActions.push_back(new ExpandGoal());
	AvailableActions.push_back(new FleetBeaconGoal());
	AvailableActions.push_back(new ForgeGoal());
	AvailableActions.push_back(new GatewayGoal());
	AvailableActions.push_back(new HighTemplarGoal());
	AvailableActions.push_back(new ImmortalGoal());
	AvailableActions.push_back(new ObserverGoal());
	AvailableActions.push_back(new OracleGoal());
	AvailableActions.push_back(new PhoenixGoal());
	AvailableActions.push_back(new PhotonCannonGoal());
	AvailableActions.push_back(new ProbeGoal());
	AvailableActions.push_back(new RoboticsBayGoal());
	AvailableActions.push_back(new RoboticsGoal());
	AvailableActions.push_back(new SentryGoal());
	AvailableActions.push_back(new ShieldBatteryGoal());
	AvailableActions.push_back(new StalkerGoal());
	AvailableActions.push_back(new StarGateGoal());
	AvailableActions.push_back(new TempestGoal());
	AvailableActions.push_back(new TemplarArchivesGoal());
	AvailableActions.push_back(new TwilightCouncilGoal());
	AvailableActions.push_back(new VoidRayGoal());
	AvailableActions.push_back(new ZealotGoal());


	PlanBotBase::Init();
}

void BotWithAPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
	auto startTime = Clock::now();
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();
	auto debug = Debug();

	UpdateGameState();
	RemoveIdleScouts();
	ChooseGoals();
	BalanceWorkerAssignments();
	DefendBase();
	CancelBuildingsUnderAttack();
	ManageUnits();

	// Morph all gateways to warpgates
	auto gateways = obs->GetUnits(sc2::Unit::Alliance::Self, IsUnit(sc2::UNIT_TYPEID::PROTOSS_GATEWAY));
	actions->UnitCommand(gateways, ABILITY_ID::MORPH_WARPGATE);

	DebugDrawState(startTime);

#if LADDER_MODE
	// This does not seem to work in local tests

	if (!this->Lost && PlanBotBase::ShouldSurrender(obs))
	{
		this->Lost = true;
		//auto y = Control()->RequestLeaveGame();
		Actions()->SendChat("My plan has failed...");
		Actions()->SendChat("gg");
	}
#endif


#if DEBUG_MODE	
	Debug()->SendDebug();
#endif
#if REALTIME
	Actions()->SendActions();
#endif
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
