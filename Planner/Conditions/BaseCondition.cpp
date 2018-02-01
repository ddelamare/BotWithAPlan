#pragma once
#include "BaseCondition.h"

bool BaseCondition::IsMet(GameState* state)
{
	return HasAllResources(state);
}

bool BaseCondition::HasAllResources(GameState* state)
{
	return NeededResources(state).size() == 0;
}

// A Static list of resources need to satisfy this condition
ResourceMap BaseCondition::GetRequiredResources()
{
	return requiredResources;
}

ResourceMap BaseCondition::NeededResources(GameState* state)
{
	auto unmet = ResourceMap();
	for (auto res : GetRequiredResources())
	{
		if (state->resources[res.first] < GetRequiredResources()[res.first])
		{
			unmet[res.first] = GetRequiredResources()[res.first] - state->resources[res.first];
		}
	}
	return unmet;
}
