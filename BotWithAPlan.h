#include <sc2api/sc2_api.h>
#include "Common\Resource.h"
#include "Planner\Planner.h"
#include "Goals\GoalPicker.h"
#include "Common\Resource.h"
#include "Common\ItemDependencies.h"
#include "sc2api/sc2_api.h"
#include "Common\GameState.h"
#include <iostream>
#define DllExport __declspec( dllexport ) 
using namespace sc2;

class BotWithAPlan : public Agent {
public:
	BotWithAPlan();
	void OnGameStart();
	void OnStep();
	void OnBuildingConstructionComplete(const Unit *);
	void OnUnitCreated(const Unit *);
	void OnUnitEnterVision(const Unit *);
	void BotWithAPlan::OnUnitDestroyed(const Unit* unit);
	bool Lost;
private:
	void ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages);

	GoalPicker goalPicker;
	Planner    planner;
	vector<BaseAction*> EconomyGoals;
	vector<BaseAction*> ArmyGoals;
	vector<BaseAction*> TacticsGoals;
	bool shouldRecalcuate;
	BaseAction* goal;
	BaseAction* armyGoal;
	BaseAction* nextInPlan;
	BaseAction* nextInArmyPlan;
	GameState  state;
	int StepCounter = 0;
	const int STEPS_PER_GOAL = 2;
	vector<string> debugMessages;
};





// Bot Ladder hooks
void DllExport *CreateNewAgent();
int DllExport GetAgentRace();
const char DllExport *GetAgentName();