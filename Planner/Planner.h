#include <iostream>
#include <string>
#include <algorithm>
#include "../Common/ItemDependencies.h"
#include "Actions\WinAction.h"
using namespace std;

class Planner
{
private:
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