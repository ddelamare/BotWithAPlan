#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <stdarg.h>
#include "Common/ResourceState.h"
#include "Common/Resource.h"
using namespace std;

class BaseCondition
{
private:
protected:
	string name = "Generic Condition";
	bool HasAllResources(ResourceState* state);
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
			sc2::UNIT_TYPEID res = va_arg(vl, sc2::UNIT_TYPEID);
			int numReq = va_arg(vl, int);
			requiredResources[res] = numReq;
		}
		va_end(vl);
	}
	virtual bool IsMet(ResourceState*);
	virtual ResourceMap GetRequiredResources(ResourceState* state);
	ResourceMap UnmetResources(ResourceState* state);

	string GetName() 
	{
		return name;
	}


};