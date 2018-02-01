#pragma once
#include "BaseAction.h"


	bool BaseAction::MeetsConditions(GameState* state) {
		return this->GetUnmetConditions(state).size() == 0;
	}
	vector<BaseCondition*> BaseAction::GetAllConditions()
	{
		return conditions;
	}

	vector<BaseCondition*> BaseAction::GetUnmetConditions(GameState* state)
	{
		vector<BaseCondition*> unmet = vector<BaseCondition*>();	   
		for ( auto cond : conditions)
		{
			if (!cond->IsMet(state))
			{
				unmet.push_back(cond);
			}
		}
		return unmet;
	}
	vector<BaseResult*> BaseAction::GetPossibleResults()
	{
		return results;
	}
	//bool Excecute();
	float BaseAction::GetActionWeight()
	{
		return cost;
	}
