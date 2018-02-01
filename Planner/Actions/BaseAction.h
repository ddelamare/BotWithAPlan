#pragma once
#include <vector>
#include "../Results/BaseResult.h"
#include "../Conditions/BaseCondition.h"
#include "../../Common/GameState.h"
using namespace std;


class BaseAction 
{
protected:
	vector<BaseCondition*> conditions;
	vector<BaseResult*> results;
	string name;
	float cost = 1;
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
	}
	BaseAction() {
		conditions = vector<BaseCondition*>();
		results = vector<BaseResult*>();
	}
	bool MeetsConditions(GameState*);
	vector<BaseCondition*> GetAllConditions();
	vector<BaseCondition*> GetUnmetConditions(GameState*);
	vector<BaseResult*> GetPossibleResults();
	bool Excecute();
	float GetActionWeight();

	string GetName()
	{
		return name;
	}
};