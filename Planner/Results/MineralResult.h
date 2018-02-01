#pragma once
#include "BaseResult.h"

class MineralResult : public BaseResult
{
public:
	MineralResult(int number)
	{
		resourcesGained[RESOURCES::MINERALS] = number;
	}
};