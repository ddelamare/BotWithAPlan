#include "BotWithAPlan.h"
#include "Common\UnitFilters.h"
#include "Common\Util.h"
void BotWithAPlan::OnBuildingConstructionComplete(const Unit*)
{
	shouldRecalcuate = true;
}

void::BotWithAPlan::OnUnitCreated(const Unit* unit)							   
{
	if (unit->unit_type == UNIT_TYPEID::PROTOSS_NEXUS)
	{
		// Clear Rally Point
		Actions()->UnitCommand(unit, ABILITY_ID::RALLY_WORKERS, unit->pos);
	}
	state.CurrentUnits[unit->unit_type]++;
}

//! Called whenever one of the player's units has been destroyed.
//!< \param unit The destroyed unit.
void BotWithAPlan::OnUnitDestroyed(const Unit* unit) {
	state.CurrentUnits[unit->unit_type]--;
	if (VectorHelpers::FoundInVector(state.ScoutingUnits, unit))
	{
		VectorHelpers::RemoveFromVector(&state.ScoutingUnits, unit);
	}

	if (!state.HasCloakedUnits &&
		Util::FindNearbyUnits(sc2::Unit::Alliance::Enemy, IsEnemy(), unit->pos, Observation(), 20.0).size() == 0)
	{
		// Killed but no nearby enemies? Must be cloaked
		this->state.HasCloakedUnits = true;
	}
}


void BotWithAPlan::OnUnitEnterVision(const Unit* unit) 
{
	state.ObservedUnits[unit->unit_type] = 1;
}

void BotWithAPlan::OnGameStart() {
	LOG(1) << "Bot initialized" << endl;
	auto nexus = Observation()->GetUnits(IsTownHall())[0];
	Actions()->UnitCommand(nexus, ABILITY_ID::SMART, nexus->pos);
	auto enemyLocations = Observation()->GetGameInfo().enemy_start_locations;
	if (enemyLocations.size() == 1)
		state.EnemyBase = enemyLocations[0];
	else if (enemyLocations.size() == 4)
		state.EnemyBase = enemyLocations[0];
	else
		state.EnemyBase = enemyLocations[0];

	state.StartingLocation = nexus->pos;

	// Get all minerals and sort by x , then y pos
	auto minerals = Observation()->GetUnits(Unit::Alliance::Neutral, IsMineralField());
	int count = 0;
	Point3D sum = Point3D();
	const int MINERAL_DISTANCE_THRESHOLD = 150;
	while (minerals.size() > 0)
	{
		Point3D origMineral = minerals[0]->pos;
		sum = Point3D();
		count = 0;
		for (int j = 0; j < minerals.size();)
		{
			auto cluster = std::vector<Point3D>();
			auto dis = DistanceSquared3D(origMineral, minerals[j]->pos);
			if (dis < MINERAL_DISTANCE_THRESHOLD)
			{
				sum += minerals[j]->pos;
				count++;
				// Erase this element
				minerals.erase(minerals.begin() + j, minerals.begin() + j + 1);
			}
			else
			{
				j++;
			}
		}

		state.ExpansionLocations.push_back(sum / count);
	}
	// Remove starting pos as expansion location
	auto closest_mineral = Util::FindClosestPoint(state.ExpansionLocations, nexus->pos);
	state.ExpansionLocations.erase(std::remove_if(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), [closest_mineral](Point3D p) {return p == closest_mineral; }));
	closest_mineral = Util::FindClosestPoint(state.ExpansionLocations, Point3D(state.EnemyBase.x, state.EnemyBase.y, 0));
	state.ExpansionLocations.erase(std::remove_if(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), [closest_mineral](Point3D p) {return p == closest_mineral; }));
    
	// Sort expansions by distance to home base
	std::sort(state.ExpansionLocations.begin(), state.ExpansionLocations.end(), Sorters::sort_by_distance(state.StartingLocation));


	// Calculate building offset
	auto nearMinerals = Util::FindNearbyUnits(IsMineralField(), Util::ToPoint3D(state.StartingLocation), Observation(), 15);

	//Calc mineral vector and normalize
	int visibleMinerals = 0;
	Point3D nearSum;
	for (auto min : nearMinerals)
	{
		if (min->display_type == Unit::DisplayType::Visible)
		{
			nearSum += min->pos;
			visibleMinerals++;
		}
	}
	if (visibleMinerals > 0)
	{
		nearSum /= visibleMinerals;
		nearSum = nearSum - nexus->pos;
		nearSum.z = 0;
		Normalize3D(nearSum);
		state.MineralDirection = nearSum;
	}

	//Cache unit info
	state.UnitInfo = Observation()->GetUnitTypeData();

	Actions()->SendChat("gl hf!");
	Actions()->SendChat("Bot version: 1.4");
}

void BotWithAPlan::OnGameEnd()
{
	  // If we have no town halls, we probably lost
	auto th = Observation()->GetUnits(sc2::Unit::Alliance::Self, IsTownHall());
	this->Lost |= (th.size() == 0);
	auto gameResults = Observation()->GetResults();
}

std::string ClientErrorToString(sc2::ClientError error)
{
	switch (error)
	{
	case(sc2::ClientError::ErrorSC2): return "ErrorSC2";
	case(sc2::ClientError::InvalidAbilityRemap): return "InvalidAbilityRemap"; /*! An ability was improperly mapped to an ability id that doesn't exist. */
	case(sc2::ClientError::InvalidResponse): return "InvalidResponse";     /*! The response does not contain a field that was expected. */
	case(sc2::ClientError::NoAbilitiesForTag): return "NoAbilitiesForTag";   /*! The unit does not have any abilities. */
	case(sc2::ClientError::ResponseNotConsumed): return "ResponseNotConsumed"; /*! A request was made without consuming the response from the previous request, that puts this library in an illegal state. */
	case(sc2::ClientError::ResponseMismatch): return "ResponseMismatch";    /*! The response received from SC2 does not match the request. */
	case(sc2::ClientError::ConnectionClosed): return "ConnectionClosed";    /*! The websocket connection has prematurely closed, this could mean starcraft crashed or a websocket timeout has occurred. */
	case(sc2::ClientError::SC2UnknownStatus): return "SC2UnknownStatus";
	case(sc2::ClientError::SC2AppFailure): return "SC2AppFailure";       /*! SC2 has either crashed or been forcibly terminated by this library because it was not responding to requests. */
	case(sc2::ClientError::SC2ProtocolError): return "SC2ProtocolError";    /*! The response from SC2 contains errors, most likely meaning the API was not used in a correct way. */
	case(sc2::ClientError::SC2ProtocolTimeout): return "SC2ProtocolTimeout";  /*! A request was made and a response was not received in the amount of time given by the timeout. */
	case(sc2::ClientError::WrongGameVersion): return "WrongGameVersion";
	}
	return "Unknown ClientError";
}

void BotWithAPlan::OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors)
{
	for (const auto & error : protocol_errors)
	{
		std::cerr << "Protocol error: " << error << std::endl;
	}
	for (const auto & error : client_errors)
	{
		std::cerr << "Client error: " << ClientErrorToString(error) << std::endl;
	}

}
