#pragma once
#include "BaseAction.h"


	bool BaseAction::MeetsConditions(ResourceState* state) {
		return this->GetUnmetConditions(state).size() == 0;
	}
	vector<BaseCondition*> BaseAction::GetAllConditions()
	{
		return conditions;
	}

	vector<BaseCondition*> BaseAction::GetUnmetConditions(ResourceState* state)
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
	bool BaseAction::Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state) {
		//TODO: Make this abstract and implement for all units
		return false;
	}
	double BaseAction::CalculateScore(const sc2::ObservationInterface *obs, GameState* state)
	{
		// If this is not overriden in subclass, then the goal derived from here will never show up in the goal picker.
		// It can, however, be selected as an action by the planner.
		return 0;
	}
