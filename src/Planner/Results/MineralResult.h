#pragma once
#include "BaseResult.h"

class MineralResult : public BaseResult
{
public:
	MineralResult(int number)
	{
		resourcesGained[sc2::UNIT_TYPEID::MINERALS] = number;
	}
};