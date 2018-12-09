#include "PlanBotBase.h"

PlanBotBase::PlanBotBase()
{
	goalPicker = GoalPicker();
	planner = Planner();
	EconomyGoals = vector<BaseAction*>();
	ArmyGoals = vector<BaseAction*>();
	state.CurrentUnits = UnitMap();
	state.ArmyManager = &armyManager;
}
void PlanBotBase::Init()
{
	planner.Init();
	shouldRecalcuate = true;

	Lost = false;
	StepCounter = STEPS_PER_GOAL;
}
void PlanBotBase::ChooseActionFromGoals(vector<BaseAction*> goals, const sc2::ObservationInterface * obs, sc2::ActionInterface * actions, sc2::QueryInterface * query, string name, vector<string>* messages, bool withRetry, bool& stopOthers)
{
	if (stopOthers) return;
	BaseAction* goal = nullptr;
	BaseAction* nextInPlan = nullptr;
	auto actionList = goalPicker.GetGoals(goals, obs, &state);
	bool allowDependencies = true;
	for (int i = 0; i < actionList.size(); i++)
	{
		if (get<0>(actionList[i]) <= 0) break; // Don't do goals with score 0;

		auto goal = std::get<1>(actionList[i]);
		auto resState = planner.GetResourceState(obs);

		auto plan = planner.CalculatePlan(resState, goal);
		if (plan.size() == 1)
		{
			nextInPlan = plan[0];
		}
		else if (allowDependencies)
		{
			nextInPlan = plan[plan.size() - 1];
		}

		if (goal && nextInPlan)
		{
			auto success = nextInPlan->Excecute(obs, actions, query, Debug(), &state);
			if (success)
			{
				auto msg = name + " GP:" + goal->GetName();
				if (nextInPlan && goal != nextInPlan)
					msg += " GS:" + nextInPlan->GetName();
				messages->push_back(msg);
				goal = nullptr;
				break;
			}
			else if (goal->HoldOtherGoals(obs))
			{
				stopOthers = true;
				break;
			}
		}

		if (!withRetry) break; // If we aren't retrying, break out
							   // Only allow the top choice to build it's dependencies. Otherwise we build one of everything
		allowDependencies = false;
	}
}


void PlanBotBase::DebugWriteInView(string message, Point2D relativePosition, DebugInterface* debug, const ObservationInterface* obs)
{
	debug->DebugTextOut(message, relativePosition);
}
