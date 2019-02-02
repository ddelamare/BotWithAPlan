#pragma once
#include <map>
#include "Resource.h"
#include "GameState.h"
using namespace std;
struct ResourceState
{
	ResourceState() {}
	ResourceState(GameState* state)
	{
		resources = state->CurrentUnits;
	}
	ResourceMap resources = ResourceMap();
	int ArmyCount;
	void AddResource(sc2::UNIT_TYPEID resource, int qty)
	{
		resources[resource] += qty;
	}
};