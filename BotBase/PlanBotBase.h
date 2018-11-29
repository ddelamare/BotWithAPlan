#pragma once
#include <sc2api/sc2_api.h>
#include "Common\Resource.h"
#include "Planner\Planner.h"
#include "Goals\GoalPicker.h"
#include "Common\Resource.h"
#include "Common\ItemDependencies.h"
#include "Common\Managers\ArmyManager.h"
#include "sc2api/sc2_api.h"
#include "Common\GameState.h"
#include <iostream>
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
using namespace sc2;
#define LADDER_MODE 1
#define DEBUG_MODE 	0
#define PLANNER_MODE 0
#define REALTIME false
class PlanBotBase : public Agent
{
public:
	PlanBotBase();
	void OnGameStart() = 0;
	void OnStep() = 0;
	void OnBuildingConstructionComplete(const Unit *) = 0;
	void OnUnitCreated(const Unit *) = 0;
	void OnUnitEnterVision(const Unit *) = 0;
	void OnUnitDestroyed(const Unit* unit) = 0;
	void OnGameEnd() = 0;
	void OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors) = 0;

	bool Lost;
protected:
	void ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages, bool withRetry, bool& stopOthers);
	bool virtual ShouldSurrender(const sc2::ObservationInterface * obs) = 0;

	GoalPicker goalPicker;
	Planner    planner;
	ArmyManager armyManager;
	GameState  state;
	vector<BaseAction*> EconomyGoals;
	vector<BaseAction*> ArmyGoals;
	vector<BaseAction*> TacticsGoals;
	vector<BaseAction*> UpgradeGoals;
	bool shouldRecalcuate;
	int StepCounter = 0;
	const int STEPS_PER_GOAL = 1;
	vector<string> debugMessages;
};

