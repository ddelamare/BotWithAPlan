#include "stdafx.h"
#include "BotWithAPlan.h"
#include "Common\Strategy\Attacks\BlinkStalker.h"
#include "Common\Strategy\Attacks\DisruptorAttack.h"
#include "Common\Strategy\Attacks\Templar.h"
#include "Common\Strategy\Attacks\VoidRay.h"
#include "Common\Strategy\Attacks\SentryMicro.h"
#include "Common\Strategy\Attacks\Phoenix.h"
#include "Common\Strategy\Attacks\OracleAttack.h"
#include "Common\Strategy\Building\ExpandStrategy.h"
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

	if (state.UnitInfo.size() == 0)
	{
		// On Game Start Does not seem to get called some times
		OnGameStart();
		cout << "Ran Game Start after game has started...";
	}


	auto type = state.UnitInfo[unit->unit_type];
	state.CurrentUnits[unit->unit_type]++;
}

//! Called whenever one of the player's units has been destroyed.
//!< \param unit The destroyed unit.
void BotWithAPlan::OnUnitDestroyed(const Unit* unit) {
	PlanBotBase::OnUnitDestroyed(unit);
}


void BotWithAPlan::OnUnitEnterVision(const Unit* unit) 
{
	state.ObservedUnits[unit->unit_type] = 1;
}

void BotWithAPlan::OnGameStart() {
	auto obs = Observation();
	auto query = Query();
	auto debug = Debug();

	PlanBotBase::OnGameStart();

	// Init Micro
	microManagers.push_back(new BlinkStalker(obs, query, &state));
	microManagers.push_back(new DisruptorAttack(obs, query, &state));
	microManagers.push_back(new TemplarMicro(obs, query, &state));
	microManagers.push_back(new VoidRayAttack(obs, query, &state));
	microManagers.push_back(new SentryMicro(obs, query, &state));
	microManagers.push_back(new PhoenixLift(obs, query, &state));
	microManagers.push_back(new OracleBeam(obs, query, &state));

	auto strat = new ExpandStrategy(ABILITY_ID::BUILD_NEXUS, false, false);
	for (auto exp : state.ExpansionLocations)
	{
		state.PrecomputedStartLocations.push_back(std::pair<Point3D,Point3D>(exp, strat->GetFirstMatchingPlacement(exp, obs, query, debug, 5, strat->NEXUS_MAX_TRIES)));
	}



#if LADDER_MODE
	Actions()->SendChat("gl hf!");
	Actions()->SendChat("Bot version: 1.6");
#endif
}

void BotWithAPlan::OnGameEnd()
{

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
