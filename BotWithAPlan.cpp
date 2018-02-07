#include "BotWithAPlan.h"
#include "sc2api/sc2_api.h"
#include "Common\UnitFilters.h"
#include <chrono>
using Clock = std::chrono::high_resolution_clock;
void BotWithAPlan::OnGameStart() {
	LOG(1) << "Bot initialized" << endl;
}

void BotWithAPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
	auto startTime = Clock::now();
	auto obs = Observation();
	auto units = obs->GetUnits(Unit::Alliance::Self, IsWorker());

	// Get All nexuses
	auto nexuses = obs->GetUnits(Unit::Alliance::Self, IsTownHall());
	auto allBuildings = obs->GetUnits(Unit::Alliance::Self);
	auto target = allBuildings[0];
	// Do Chrono Boost and build probe
	for (auto nexus : nexuses)
	{
		// Each on itself
		auto actions = Query()->GetAbilitiesForUnit(nexus);
		for (auto action : actions.abilities)
		{
			Debug()->DebugTextOut(AbilityTypeToName(action.ability_id));
		}
		Actions()->UnitCommand(nexus, CHRONO_OVERRIDE,nexus);
		auto orders = nexus->orders;
		//Actions()->UnitCommand(nexus, ABILITY_ID::EFFECT_CHRONOBOOST, nexus->tag);
		//auto commands = Actions()->Commands();
		// Build Probe
		Actions()->UnitCommand(nexus, ABILITY_ID::TRAIN_PROBE) ;
	}

	auto endTime = Clock::now();
	Debug()->DebugTextOut("Loop Time: " + to_string(std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count()));
	Debug()->SendDebug();
}





#pragma region Bot Ladder Hooks
void *CreateNewAgent()
{
	return (void *)new BotWithAPlan();
}

const char *GetAgentName()
{
	return "BotWithAPlan";
}

int GetAgentRace()
{
	return (int)sc2::Race::Protoss;
}
#pragma endregion
