#pragma once
#include <vector>
#include "../Results/BaseResult.h"
#include "../Conditions/BaseCondition.h"
#include "../../Common/GameState.h"
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
	bool MeetsConditions(GameState*);
	vector<BaseCondition*> GetAllConditions();
	vector<BaseCondition*> GetUnmetConditions(GameState*);
	vector<BaseResult*> GetPossibleResults();
	double virtual CalculateScore(const sc2::ObservationInterface *obs);
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug);

	string GetName()
	{
		return name;
	}
};