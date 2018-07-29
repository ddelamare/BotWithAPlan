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
private:
	GoalPicker goalPicker;
	Planner    planner;
	vector<BaseAction*> EconomyGoals;
	vector<BaseAction*> ArmyGoals;
	vector<BaseAction*> TacticsGoals;
	bool shouldRecalcuate;
	BaseAction* econGoal;
	BaseAction* armyGoal;
	BaseAction* nextInPlan;
	BaseAction* nextInArmyPlan;
	GameState  state;
};





// Bot Ladder hooks
void DllExport *CreateNewAgent();
int DllExport GetAgentRace();
const char DllExport *GetAgentName();