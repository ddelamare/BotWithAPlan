#pragma once 
#include "Resource.h"
#include <Planner/Conditions/BaseCondition.h>
#include <Planner/Actions/BaseAction.h>

extern vector<BaseAction*> AvailableActions;

void GenerateDependencyList();