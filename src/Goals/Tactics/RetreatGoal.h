#pragma once
#include "Planner/Actions/BaseAction.h"
#include "Planner/Actions/BuildResource.h"
#include "sc2api\sc2_api.h"
#include "Common/Resource.h"
#include "Common/Util/Util.h"
#include "Common\Entity\ThreatModel.h"

class RetreatGoal : public BaseAction
{
	float threatThreshold = .75;
	ThreatAnalyzer threatAnalyzer;
public:
	RetreatGoal() : BaseAction() {
		this->BaseAction::name = "Run Awayyyyyy!";
	}
	double virtual CalculateScore(const sc2::ObservationInterface *obs, GameState* state) {
		auto threat = threatAnalyzer.GetThreat(&state->threat);

		if (threat <= 0)
		{
			return 10;
		}
		if (threat < .75)
		{
			return 2 / threat;
		}
		return 0;
	};

	bool virtual Excecute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
	{

		auto closestTownHall = Util::FindClosetOfType(obs->GetUnits(sc2::Unit::Alliance::Enemy, IsEnemyArmy()), Util::ToPoint3D(state->StartingLocation), obs, query, false);
		bool isEnemyClose = false;
		if (closestTownHall) {
			auto disToTownHall = Distance2D(closestTownHall->pos, state->StartingLocation);

			// Retreat unless we are at home
			if (disToTownHall > 20)
			{
				isEnemyClose = true;
			}
		}

		if (!isEnemyClose)
			state->ArmyManager->RequestAction(state->StartingLocation, BattleMode::Defend);

		return false;
	}
};

