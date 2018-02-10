#pragma once
#include "../BaseGoal.h"
#include "../../Planner/Actions/BaseAction.h"
#include "../../Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "../../Common/Resource.h"
class PylonGoal : public BaseGoal
{
public:
	PylonGoal() : BaseGoal() {
		goal = new BuildResource(RESOURCES::PYLON);
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs) {
		double score = 16.0f;
		int foodLeft = obs->GetFoodCap() - obs->GetFoodUsed();

		//TODO: Add food from pylons that are building

		if (foodLeft > 0)
		{
			score /= foodLeft;
		}
		else if (foodLeft < 0)
		{
			// If we are in the red, we really want that pylon.
			score = 50;
		}

		if (score < 1)
		{
			// if we have more than 16 free food, no reason to build one.
			score = 0;
		}


		return score;
	};
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query)
	{
		bool madePylon = false;
		//TODO: Find location
		//TODO: Find Nearby Probe
		//TODO: Build pylon

		return madePylon;
	}
	BaseAction virtual *GetAction() {
		return goal;
	}
};