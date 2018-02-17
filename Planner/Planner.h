#include <iostream>
#include <string>
#include <algorithm>
#include "../Common/ItemDependencies.h"
#include "Actions\WinAction.h"
#include "sc2api\sc2_api.h"
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
	GameState GetGameState(const sc2::ObservationInterface* obs);
	Planner();
	bool Init();
};