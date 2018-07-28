#pragma	once
#include <Planner/Actions/BaseAction.h>

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
};