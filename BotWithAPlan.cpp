#include "BotWithAPlan.h"
#include "sc2api/sc2_api.h"
#include "Common\UnitFilters.h"
#include <chrono>
#include "Goals\Economy\Probe.h"
#include "Goals\Economy\Pylon.h"
#include "Goals\Economy\Chrono.h"
#include "Common\Util.h"
using Clock = std::chrono::high_resolution_clock;

BotWithAPlan::BotWithAPlan() 
{
	goalPicker = GoalPicker();
	planner = Planner();
	EconomyGoals = vector<BaseGoal*>();
	//TODO: Fill goal and dependency dictionaries

	EconomyGoals.push_back(new PylonGoal());
	EconomyGoals.push_back(new ProbeGoal());
	EconomyGoals.push_back(new ChronoGoal());
}

void BotWithAPlan::OnGameStart() {
	LOG(1) << "Bot initialized" << endl;
}

void BotWithAPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
	auto startTime = Clock::now();
	auto obs = Observation();
	auto query = Query();
	auto actions = Actions();
	
	auto econGoal  = goalPicker.GetGoal(EconomyGoals, obs);

	if (econGoal)
	{
		Debug()->DebugTextOut("Econ Goal Picked:" + econGoal->GetAction()->GetName());
		auto success = econGoal->Excecute(obs,actions,query);
	}
	else
	{
		Debug()->DebugTextOut("No Econ Goal Picked.");
	}

	auto idleUnits = obs->GetUnits(Unit::Alliance::Self, IsIdleWorker());
	if (idleUnits.size() > 0)
	{
		Debug()->DebugSphereOut(idleUnits[0]->pos + Point3D(0,0,1), 10);
		auto resource = FindNearestResourceNeedingHarversters(idleUnits[0], obs, query);
		if (resource)
		{
			// TODO: Send probe to mine
			Debug()->DebugSphereOut(resource->pos + Point3D(0, 0, 1), 3, Colors::Yellow);
			actions->UnitCommand(idleUnits[0], ABILITY_ID::HARVEST_GATHER, resource);
		}
		else
		{
			// What to do here?
		}
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
