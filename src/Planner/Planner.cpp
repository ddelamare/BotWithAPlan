#include "Planner.h"

Planner::Planner() {
	LOG(4) << "Constructed Planner\n";
}

void Planner::PrintPlan(vector<BaseAction*> plan)
{
	LOG(1) << "Planned: ";
	for (int i = plan.size() - 1; i >= 0; i--)
	{
		LOG(1) << plan[i]->GetName() << " -> ";
	}
	LOG(1) << "done." << endl;
}

ResourceState Planner::GetResourceState(const sc2::ObservationInterface* obs)
{
	auto units = obs->GetUnits(sc2::Unit::Alliance::Self);
	auto state = ResourceState();
	for (const auto& unit : units)
	{
		state.AddResource(unit->unit_type, 1);
	}
	state.ArmyCount = obs->GetFoodArmy();
	return state;
}

bool Planner::Init() {
	LOG(4) << "Planner Initialized\n";

	//TODO: Load Available Actions external
	availableActions = AvailableActions;

	return true;
}

ResourceState Planner::CalculateTargetResourceState(ResourceState state, BaseAction* endGoal)
{
	//Goal needs some conditions met. Determine what it needs to reach the game state we want
	auto neededConditions = endGoal->GetUnmetConditions(&state);
	auto goalState = ResourceState();

	// Make sure all resources known in state make it to the returned state
	for (const auto& res : state.resources)
	{
		goalState.resources[res.first] = state.resources[res.first];
	}

	// Calculate the total number of resources to get to the goal state
	for (const auto& cond : neededConditions)
	{
		LOG(5) << "\t" << cond->GetName() << endl;
		auto needed = cond->UnmetResources(&state);
		for (const auto& res : needed)
		{
			goalState.resources[res.first] += res.second;
			if (state.resources[res.first] >= cond->GetRequiredResources(&state)[res.first])
			{
				LOG(5) << "\t- Has Resource " << (int)res.first << endl;
			}
			else
			{
				LOG(5) << "\t- Needs Resource " << RESOURCE_MAP[res.first].name << ". Amount: " << res.second << endl;
			}
		}
	}

	return goalState;
}

vector<BaseAction*> Planner::CalculatePlan(ResourceState* currentState, ResourceState goalState)
{
	auto plan = vector<BaseAction*>(0);

	bool isGoalState = true;
	for (const auto& res : goalState.resources)
	{
		if (res.second > currentState->resources[res.first])
		{
			isGoalState = false;
			break;
		}
	}

	if (isGoalState)
	{
		LOG(4) << "\t **--**GOAL FOUND**--**\n";
		return plan;
	}

	// Look for actions that meet the resouce need. This list holds the action and it's calculated score
	//TODO: This can all be migrated to calc heuristics. scores < 0 are not eligible.
	auto actionsThatGetCorrectResources = vector<tuple<BaseAction*, float>>();
	for (const auto& action : availableActions)
	{
		bool hasAny = false;
		float heuristic = -1;

		auto resourcesGained = action->GetPossibleResults()[0]->ResourcesGained();
		for (const auto& res : goalState.resources)
		{
			// Check if this action gives any of a needed resource
			if (resourcesGained[res.first] > 0 && goalState.resources[res.first] > currentState->resources[res.first])
			{
				hasAny = true;
				break;
			}
		}
		if (hasAny)
		{
			heuristic = CalculateHeuristic(action, currentState, &(goalState.resources));

			LOG(5) << "\t--Action " << action->GetName() << " is viable with weight " << heuristic << endl;
		}
		else
		{
			LOG(5) << "\t--Action " << action->GetName() << " is not viable with weight " << heuristic << endl;
		}
		actionsThatGetCorrectResources.push_back(tuple<BaseAction*, float>(action, heuristic));
	}


	// Get action with highest score
	BaseAction* chosen = 0;
	float maxValue = 0;
	for (const auto& action : actionsThatGetCorrectResources)
	{
		if (get<1>(action) > maxValue)
		{
			chosen = get<0>(action);
			maxValue = get<1>(action);
		}
	}

	if (chosen != 0)
	{
		LOG(4) << "\t---Choose Action " << chosen->GetName() << endl;
		plan.push_back(chosen);
	}
	else
	{
		LOG(4) << "\t****NO ACTIONS FOUND****\n";
		return plan;
	}

	auto prevPlan = vector<BaseAction*>();

	if (!chosen->MeetsConditions(currentState))
	{
		plan.pop_back();
		LOG(5) << "\t_-_-_-Chosen Action conditions not met\n";
		auto newPlan = CalculatePlan(*currentState, chosen);
		plan.insert(plan.begin(), newPlan.begin(), newPlan.end());
		//Since we successfully got a plan, we can add the plan values to out state
		for (const auto& action : newPlan)
		{
			for (const auto& result : action->GetPossibleResults())
			{
				for (const auto& res : result->ResourcesGained())
				{
					currentState->resources[res.first] += res.second;
				}
			}
		}
	}
	else
	{
		for (const auto& result : chosen->GetPossibleResults())
		{
			for (const auto& res : result->ResourcesGained())
			{
				currentState->resources[res.first] += res.second;
			}
		}
	}

	prevPlan = CalculatePlan(currentState, goalState);
	// Concatenate
	plan.insert(plan.begin(), prevPlan.begin(), prevPlan.end());
	return 	plan;
}

vector<BaseAction*> Planner::CalculatePlan(ResourceState state, BaseAction* endGoal)
{	/*What we really want to find here is not the end action, but the end game state that allows us to excecute that action.
	    So, the steps are:
		1) Determine if we are already at a game state that satisfies the endGoal action
		2) If not, calcuate the minimum game state that is needed to do so. This is done by analying the preconditions for the actions

	*/
	auto plan = vector<BaseAction*>(0);
	if (endGoal == NULL)
		return plan;

	plan.push_back(endGoal);

	// Are we done before we start?
	if (endGoal->MeetsConditions(&state))
	{
		LOG(5) << "Goal Met: " << endGoal->GetName() << endl;
		return plan;
	}

	LOG(5) << "Goal Not Met: " << endGoal->GetName() << endl;

	auto goalState = CalculateTargetResourceState(state, endGoal);

	/*
		HERE KICK OFF RECURSIVE GRAPH BUILD FUNCTION
	*/
	auto prevPlan = CalculatePlan(&state, goalState);
	// Concatenate
	plan.insert(plan.end(),prevPlan.begin(), prevPlan.end());
	return 	plan;
}


//TODO: find a better heuristic method
float Planner::CalculateHeuristic(BaseAction* action, ResourceState* state, ResourceMap* neededResources)
{
	auto value = 0.0f;
	auto actionResults = action->GetPossibleResults()[0]->ResourcesGained();
	for(const auto& res : *neededResources)
	{
		int resApplied = min(res.second, actionResults[res.first]);
		if (resApplied > 0)
		{
			value += resApplied; // TODO: multiply by resource weight
		}
	}

	// If the action already has it's conditions met, nudge it in that direction.
	if (action->MeetsConditions(state))
	{
		value += 1;
	}

	return value;
}

