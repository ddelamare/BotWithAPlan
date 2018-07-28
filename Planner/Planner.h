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
	ResourceState CalculateTargetResourceState(ResourceState, BaseAction*);
	vector<BaseAction*> CalculatePlan(ResourceState*, ResourceState);
	float CalculateHeuristic(BaseAction*, ResourceState*, ResourceMap*);
public: 
	vector<BaseAction*> CalculatePlan(ResourceState, BaseAction*);
	void PrintPlan(vector<BaseAction*>);
	ResourceState GetResourceState(const sc2::ObservationInterface* obs);
	Planner();
	bool Init();
};