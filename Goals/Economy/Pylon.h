#pragma once
#include "../BaseGoal.h"
#include "../../Planner/Actions/BaseAction.h"
#include "../../Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "../../Common/Resource.h"
class ChronoGoal : public BaseGoal
{
public:
	ChronoGoal() {
		goal = new BuildResource(RESOURCES::PYLON);
	}
	double virtual CalculateScore(sc2::ObservationInterface* state) {
		double score = 1.0f;
		if (state->GetFoodUsed > state->GetFoodArmy())
		{
			score *= 50;
		}

		return score;
	};
	BaseAction virtual *GetAction() {
		return goal;
	}
};