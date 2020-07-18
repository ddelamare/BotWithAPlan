#pragma once
#include <sc2api/sc2_api.h>
#include "Common\Resource.h"
#include "Planner\Planner.h"
#include "Goals\GoalPicker.h"
#include "Common\ItemDependencies.h"
#include "Common\Managers\ArmyManager.h"
#include "Common\Managers\ActionManager.h"
#include "sc2api/sc2_api.h"
#include "Common\GameState.h"
#include <iostream>
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include "Common\Analyzers\ThreatAnalyzer.h"
#include "Common\Strategy\Attacks\UnitMicro.h"
#include "Common/Strategy/Building/ExpandStrategy.h"
#include <chrono>
#include <Common/Util/UtilConfig.h>
using Clock = std::chrono::high_resolution_clock;


using namespace sc2;
#define LADDER_MODE 1
#define PLANNER_MODE 0
#define REALTIME 0	 
class PlanBotBase : public Agent
{
public:
	PlanBotBase();
	void Init();
	void OnGameStart();
	void OnStep();
	void virtual OnBuildingConstructionComplete(const Unit*) {};
	void OnUnitCreated(const Unit *);
	void OnUnitEnterVision(const Unit *);
	void virtual OnUnitDestroyed(const Unit* unit);
	void virtual OnGameEnd() {};
	void virtual OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors);
	void DebugWriteInView(string message, Point2D relativePosition, DebugInterface* debug, const ObservationInterface* obs);
    bool Lost;
	bool errorOccurred = 0;

protected:
	void ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages, bool withRetry, bool& stopOthers);
	bool virtual ShouldSurrender(const sc2::ObservationInterface * obs);
	void virtual DebugDrawState(chrono::time_point<chrono::steady_clock> startTime);
	void UpdateGameState();
	void ManageScouts();
	void virtual ChooseGoals();
	void BalanceWorkerAssignments();
	void DefendBase();
	void CancelBuildingsUnderAttack();
	void ManageUnits();
	void CheckChat();
	ActionManager* GetActionManager();


	GoalPicker goalPicker;
	Planner    planner;
	ArmyManager armyManager;
	GameState  state;
	ActionManager* actionManager;
	vector<BaseAction*> EconomyGoals;
	vector<BaseAction*> BuildOrderGoals;
	vector<BaseAction*> ArmyGoals;
	vector<BaseAction*> TacticsGoals;
	vector<BaseAction*> UpgradeGoals;
	bool shouldRecalcuate;
	int StepCounter = 0;
	const int STEPS_PER_GOAL = 2;
	vector<string> debugMessages;
	ThreatAnalyzer threatAnalyzer;
	vector<UnitMicro*> microManagers;
	int goalCategory = 0;

	//Config Values
	map<string, string> Configs;
	bool DebugEnabled = 0;
	bool IsRealTime = 0;
	bool EnableChat = 0;
	bool EnableSurrender = 0;
	bool IgnoreSelected = 0;
	std::string TargetingType;

};

																  