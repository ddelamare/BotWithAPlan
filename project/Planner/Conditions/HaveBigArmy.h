#pragma once
#include "BaseCondition.h"
class HaveBigArmy : public BaseCondition
{
	int armyGoalSize;
public:
	HaveBigArmy(int armyCount) {
		name = "Needs Bigger Army";
		armyGoalSize = armyCount;
	}
	bool IsMet(ResourceState * state) override;
};

bool HaveBigArmy::IsMet(ResourceState* state)
{
	return state->ArmyCount > armyGoalSize;
}