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
#include "Common\BotBase\PlanBotBase.h"
#define DllExport __declspec( dllexport ) 
using namespace sc2;


class TerranPlan : public PlanBotBase {
public:
	TerranPlan();
	void OnGameStart();
	void OnStep();
	void OnBuildingConstructionComplete(const Unit *);
	void OnUnitCreated(const Unit *);
	void OnUnitEnterVision(const Unit *);
	void OnUnitDestroyed(const Unit* unit);
	void OnGameEnd();
	void OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors);

	bool Lost;
private:
	void ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages, bool withRetry, bool& stopOthers);
	bool ShouldSurrender(const sc2::ObservationInterface * obs);
};


// Bot Ladder hooks
void DllExport *CreateNewAgent();
int DllExport GetAgentRace();
const char DllExport *GetAgentName();