#pragma once
#include "BaseCondition.h"

bool BaseCondition::IsMet(ResourceState* state)
{
	return HasAllResources(state);
}

bool BaseCondition::HasAllResources(ResourceState* state)
{
	return UnmetResources(state).size() == 0;
}

// A Static list of resources need to satisfy this condition
ResourceMap BaseCondition::GetRequiredResources()
{
	return requiredResources;
}

ResourceMap BaseCondition::UnmetResources(ResourceState* state)
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
