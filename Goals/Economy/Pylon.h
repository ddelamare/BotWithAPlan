#pragma once
#include <Planner/Actions/BaseAction.h>
#include <Planner/Actions/BuildResource.h>
#include "sc2api\sc2_api.h"
#include <Common/Resource.h>
#include <Common\Util.h>
using namespace sc2;
class PylonGoal : public BaseAction
{
public:
	PylonGoal() : BaseAction() {
		this->results.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_PYLON, 1));
		name = "Build Pylon";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		double score = 16.0f;
		int foodLeft = obs->GetFoodCap() - obs->GetFoodUsed();

		//Add food from building pylons
		auto pylonsInProgressw = obs->GetBuffData();
		auto pylonsInProgress = obs->GetUnits(Unit::Alliance::Self, UnitsInProgress(UNIT_TYPEID::PROTOSS_PYLON));
		foodLeft += pylonsInProgress.size() * 8;

		if (foodLeft > 0)
		{
			score /= foodLeft;
		}
		else if (foodLeft <= 0)
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
	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{
		float PLYLON_RADIUS = 1.125;
		bool madePylon = false;
		auto pylons = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_PYLON));
		auto nexii = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_NEXUS));
		auto minerals = obs->GetUnits(Unit::Alliance::Neutral, IsMineralField());
		bool works = query->Placement(ABILITY_ID::BUILD_PYLON, nexii[0]->pos + Point2D(0, -5));
		//TODO: Find location away from minerals
		Point3D buildPos;
		if (pylons.size())
		{
			// Build near other pylon
			if (state->MineralDirection.x > state->MineralDirection.y)
			{
				// Find newest pylon
				float maxX = std::numeric_limits<float>::max();
				float y;
				for (auto pylon : pylons)
				{
					maxX = min(maxX, pylon->pos.x);
					y = pylon->pos.y;
				}
				buildPos = Point3D(maxX - (2 * PLYLON_RADIUS) , y , 0);
			}
			else
			{
				float maxY = std::numeric_limits<float>::max();
				float x;
				for (auto pylon : pylons)
				{
					maxY = min(maxY, pylon->pos.y);
					x = pylon->pos.x;
				}
				buildPos = Point3D(x, maxY - (2 * PLYLON_RADIUS), 0);
			}
		}
		else
		{
			if (Distance2D(state->MineralDirection,Point2D(0,0)) == 0) // Check if it's been set
			{
				//Calc mineral vector and normalize
				int visibleMinerals = 0;
				Point3D sum;
				for (auto min : minerals)
				{
					if (min->display_type == Unit::DisplayType::Visible)
					{
						sum += min->pos;
						visibleMinerals++;
					}
				}
				if (visibleMinerals > 0)
				{
					sum.z = 0;
					sum /= visibleMinerals;
					Normalize3D(sum);
					state->MineralDirection = sum;
				}
			}
			auto mainNex = nexii[0];
			buildPos = mainNex->pos - (state->MineralDirection * (mainNex->radius + PLYLON_RADIUS));  // Add pylon and nexus radius
		}
		auto probe = FindClosetOfType(UNIT_TYPEID::PROTOSS_PROBE, buildPos, obs, query);
		actions->UnitCommand(probe, ABILITY_ID::BUILD_PYLON, buildPos);
		debug->DebugSphereOut(buildPos, 3, Colors::Teal);

		return madePylon;
	}
};