#pragma once
#include <vector>
#include "Planner/Results/BaseResult.h"
#include "Planner/Conditions/BaseCondition.h"
#include "Common/ResourceState.h"
#include "Common/GameState.h"

#include "sc2api\sc2_api.h"
using namespace std;


class BaseAction 
{
protected:
	vector<BaseCondition*> conditions;
	vector<BaseResult*> results;
	string name;
	// These are used to help calculate score and maintain which units to act on
	sc2::Units units;
	sc2::Units targets;
	bool holdOtherGoals = false; // If true then puts all other goals on hold while it's active.
public:
	BaseAction(string actionName) {
		conditions = vector<BaseCondition*>();
		results = vector<BaseResult*>();
		name = actionName;
	}
	BaseAction(string actionName, vector<BaseCondition*> conds, vector<BaseResult*> res) {
		conditions = conds;
		results = res;
		name = actionName;
		this->units   = sc2::Units();
		this->targets = sc2::Units();
	}
	BaseAction() {
		conditions = vector<BaseCondition*>();
		results = vector<BaseResult*>();
	}
	bool MeetsConditions(ResourceState*);
	vector<BaseCondition*> GetAllConditions();
	vector<BaseCondition*> GetUnmetConditions(ResourceState*);
	vector<BaseResult*> GetPossibleResults();
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state);
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);
	bool virtual HoldOtherGoals(const sc2::ObservationInterface *obs) { return holdOtherGoals; }
	string GetName()
	{
		return name;
	}
};