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
#define DllExport __declspec( dllexport ) 
using namespace sc2;
#define LADDER_MODE 0
#define DEBUG_MODE 1	
#define PLANNER_MODE 0
#define REALTIME false	 
class BotWithAPlan : public Agent {
public:
	BotWithAPlan();
	void OnGameStart();
	void OnStep();
	void OnBuildingConstructionComplete(const Unit *);
	void OnUnitCreated(const Unit *);
	void OnUnitEnterVision(const Unit *);
	void BotWithAPlan::OnUnitDestroyed(const Unit* unit);
	void OnGameEnd();
	void OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors);

	bool Lost;
private:
	void ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages, bool withRetry, bool& stopOthers);

	GoalPicker goalPicker;
	Planner    planner;
	ArmyManager armyManager;
	vector<BaseAction*> EconomyGoals;
	vector<BaseAction*> ArmyGoals;
	vector<BaseAction*> TacticsGoals;
	vector<BaseAction*> UpgradeGoals;
	bool shouldRecalcuate;
	GameState  state;
	int StepCounter = 0;
	const int STEPS_PER_GOAL = 1;
	vector<string> debugMessages;
};


// Bot Ladder hooks
void DllExport *CreateNewAgent();
int DllExport GetAgentRace();
const char DllExport *GetAgentName();