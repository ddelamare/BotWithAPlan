#include <iostream>
#include <string>
#include <algorithm>
#include "../Common/ItemDependencies.h"
#include "Actions\WinAction.h"
#include "Actions\BuildWorkerAction.h"
#include "Actions\BuildMoreWorkersAction.h"
#include "Actions\GatherMineralsAction.h"
using namespace std;

class Planner
{
private:
	BaseAction* goal;
	vector<BaseAction*> availableActions;
	GameState CalculateTargetGameState(GameState, BaseAction*);
	vector<BaseAction*> CalculatePlan(GameState*, GameState);
	float CalculateHeuristic(BaseAction*, GameState*, ResourceMap*);
public: 
	vector<BaseAction*> CalculatePlan(GameState, BaseAction*);
	void PrintPlan(vector<BaseAction*>);
	Planner();
	bool Init();
};