#include <sc2api/sc2_api.h>

#include <iostream>
#include "BotWithAPlan.h"
using namespace sc2;

// 
#define LADDER_MODE 1	  
int main(int argc, char* argv[]) {
#if LADDER_MODE
    Coordinator coordinator;
    coordinator.LoadSettings(argc, argv);

    BotWithAPlan bot;
    coordinator.SetParticipants({
        CreateParticipant((Race)GetAgentRace(), &bot),
        CreateComputer(Race::Terran)
    });

    coordinator.LaunchStarcraft();
    coordinator.StartGame(sc2::kMapBelShirVestigeLE);

    while (coordinator.Update()) {
    }
#else
	InitResources();
	GenerateDependencyList();
	auto planner = new Planner();
	auto state = GameState();

	state.AddResource(RESOURCES::PROBE, 1);
	state.AddResource(RESOURCES::MINERALS, 14);
	//state.AddResource(RESOURCES::NEXUS, 14);
	planner->Init();
	auto plan = planner->CalculatePlan(state, new WinAction());
	planner->PrintPlan(plan);
	std::cin.get();

#endif

    return 0;
}