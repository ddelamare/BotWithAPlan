#pragma once
#include <map>
#include "Resource.h"
using namespace std;
struct GameState
{
	ResourceMap resources = ResourceMap();	 
	void AddResource(sc2::UNIT_TYPEID resource, int qty)
	{
		resources[resource] += qty;
	}
};