#pragma	once
#include "../Planner/Actions/BaseAction.h"
#include "BaseGoal.h"

class GoalPicker {
public:
	static BaseGoal* GetGoal(vector<BaseGoal*> goals,const sc2::ObservationInterface *obs)
	{
		BaseGoal* selected = 0;
		double selectedValue = 0;
		for (auto goal : goals)
		{
			auto score = goal->CalculateScore(obs);
			if (score > selectedValue)
			{
				selected = goal;
				selectedValue = score;
			}
		}
		return selected;
	}
};