#pragma	once
#include "../Planner/Actions/BaseAction.h"
#include "BaseGoal.h"
class GoalPicker {
public:
	static BaseAction* FindHighestScore(vector<BaseGoal*> goals, GameState* state)
	{
		BaseAction* selected = 0;
		double selectedValue = DBL_MAX;
		for (auto goal : goals)
		{
			if (goal->CalculateScore(state) < selectedValue)
			{
				selected = goal->GetAction();
				selectedValue = goal->CalculateScore(state);
			}
		}
		return selected;
	}
};