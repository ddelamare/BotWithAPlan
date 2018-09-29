#pragma	once
#include "Planner/Actions/BaseAction.h"

class GoalPicker {
public:
	static BaseAction* GetGoal(vector<BaseAction*> goals,const sc2::ObservationInterface *obs, GameState* state )
	{
		BaseAction* selected = 0;
		double selectedValue = 0;
		for (auto goal : goals)
		{
			auto score = goal->CalculateScore(obs, state);
			if (score > selectedValue)
			{
				selected = goal;
				selectedValue = score;
			}
		}
		return selected;
	}

	static vector<tuple<double, BaseAction*>> GetGoals(vector<BaseAction*> goals, const sc2::ObservationInterface *obs, GameState* state)
	{
		auto actions = vector<tuple<double, BaseAction*>>();
		for (auto goal : goals)
		{
			auto score = goal->CalculateScore(obs, state);
			if (score > 0)
			{
				actions.push_back(make_tuple(score, goal));
			}
		}
		sort(actions.begin(), actions.end());
		reverse(actions.begin(), actions.end());
		return actions;
	}
};