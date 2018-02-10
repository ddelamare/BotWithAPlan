#pragma once
#include "BaseAction.h"
#include "../../Common/Resource.h"
#include "../Results/WorkerResult.h"
class UseAbility : public BaseAction
{
public:
	UseAbility(string name)
	{
		this->name = name;
	}

};