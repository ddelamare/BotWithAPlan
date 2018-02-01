#pragma once
#include <map>
#include "Resource.h"
using namespace std;
struct GameState
{
	ResourceMap resources = ResourceMap();	 
	void AddResource(RESOURCES resource, int qty)
	{
		resources[resource] += qty;
	}
};