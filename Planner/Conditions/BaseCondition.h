#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdarg.h>
#include "../../Common/GameState.h"
#include "../../Common/Resource.h"
using namespace std;

class BaseCondition
{
private:
protected:
	string name = "Generic Condition";
	bool HasAllResources(GameState* state);
	ResourceMap requiredResources;

public:
	BaseCondition() {
		requiredResources = ResourceMap();
	}
	BaseCondition(string condName, int numDeps, ...)
	{
		name = condName;
		requiredResources = ResourceMap();
		// Dynamically create a Condition based on dependencies
		va_list vl;
		va_start(vl, numDeps);
		for (int i = 0; i < numDeps; i += 2)
		{
			RESOURCES res = va_arg(vl, RESOURCES);
			int numReq = va_arg(vl, int);
			requiredResources[res] = numReq;
		}
		va_end(vl);
	}
	virtual bool IsMet(GameState*);
	ResourceMap GetRequiredResources();
	ResourceMap NeededResources(GameState* state);

	string GetName() 
	{
		return name;
	}


};