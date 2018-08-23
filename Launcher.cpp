#include <sc2api/sc2_api.h>

#include <iostream>
#include "BotWithAPlan.h"
using namespace sc2;

// 
#define LADDER_MODE 1	  
int main(int argc, char* argv[]) {
	InitResources();
	GenerateDependencyList();
#if LADDER_MODE
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);
	coordinator.SetMultithreaded(false);
	coordinator.SetStepSize(50);
	BotWithAPlan bot;
	BotWithAPlan bot2;
    coordinator.SetParticipants({
		CreateParticipant((Race)GetAgentRace(), &bot),
		//CreateParticipant((Race)GetAgentRace(), &bot2),
        CreateComputer(Race::Terran, sc2::Difficulty::Hard)
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);
    while (coordinator.Update()) {
    }
#else

	auto planner = new Planner();
	auto state = ResourceState();

	state.AddResource(sc2::UNIT_TYPEID::PROTOSS_PROBE, 1);
	//state.AddResource(sc2::UNIT_TYPEID::NEXUS, 14);
	planner->Init();
	auto plan = planner->CalculatePlan(state, new WinAction());
	planner->PrintPlan(plan);
	std::cin.get();

#endif

    return 0;
}