#include <sc2api/sc2_api.h>
#include "Common\Resource.h"
#include "Planner\Planner.h"
#include "Common\Resource.h"
#include "Common\ItemDependencies.h"
#include <iostream>

using namespace sc2;

class BotWithAPlan : public Agent {
public:
	virtual void OnGameStart() final {
		LOG(1) << "Bot initialized" << endl;
	}

	virtual void OnStep() final {
		LOG(4) << "Step Begin" << endl;
	}
};