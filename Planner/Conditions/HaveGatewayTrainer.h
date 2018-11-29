#pragma once
#include "BaseCondition.h"
class HaveGateWayTrainer : public BaseCondition
{
public:
	HaveGateWayTrainer() {
		name = "Needs Gateway Unit Maker";
		requiredResources[sc2::UNIT_TYPEID::PROTOSS_GATEWAY] = 1;
	}
	bool IsMet(ResourceState * state) override;
	ResourceMap GetRequiredResources(ResourceState*) override;

};

// Because this can be met multiple ways, we need to overide the default
bool HaveGateWayTrainer::IsMet(ResourceState* state)
{
	return ((state->resources[sc2::UNIT_TYPEID::PROTOSS_GATEWAY] + state->resources[sc2::UNIT_TYPEID::PROTOSS_WARPGATE]) > 0);
}
// If we have either gateway or warp gate, we are good
ResourceMap HaveGateWayTrainer::GetRequiredResources(ResourceState* state)
{
	if ((state->resources[sc2::UNIT_TYPEID::PROTOSS_GATEWAY] + state->resources[sc2::UNIT_TYPEID::PROTOSS_WARPGATE]) > 0)
	{
		return ResourceMap();
	}
	else
	{
		return requiredResources;
	}
}

