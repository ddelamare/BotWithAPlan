#include "sc2api/sc2_api.h"
#include <iostream>
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include "LadderInterface.h"
#include "BotWithAPlan.h"
#include "Common\Util.h"
using namespace sc2;
const char* kAcidPlant = "Ladder/(2)AcidPlantLE.SC2Map";

// 
  
int main(int argc, char* argv[]) {
#if LADDER_MODE
	for (int i = 0; i < argc; i++)
	{
		LOG(4) << argv[i] << endl;
	}
	RunBot(argc, argv, new BotWithAPlan(), sc2::Race::Protoss);

	return 0;
#elif DEBUG_MODE
	auto races = new Race[3]{ Race::Terran, Race::Zerg, Race::Protoss };
	auto wins = map<Race, Point2D>();
	int NUM_TRIALS = 10;
	for (int i = 0; i < NUM_TRIALS; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			auto race = races[j];

			Coordinator coordinator;
			coordinator.LoadSettings(argc, argv);
			coordinator.SetMultithreaded(true);
			coordinator.SetStepSize(10);
			BotWithAPlan bot;
			coordinator.SetParticipants({
				//CreateParticipant(Race::Protoss, nullptr),
				CreateParticipant((Race)GetAgentRace(), &bot),
				//CreateParticipant((Race)GetAgentRace(), &bot2),
				CreateComputer(race, sc2::Difficulty::Hard) 
				});

			coordinator.LaunchStarcraft();
			coordinator.StartGame(kMapBelShirVestigeLE);
			while (coordinator.Update()) {
				if (bot.Lost) break;
			}
			if (bot.Lost)
			{
				wins[race] += Point2D(1, 0);
			}
			else
			{
				wins[race] += Point2D(0, 1);
			}
			coordinator.LeaveGame();
			std::cout << "Game:" << (i*NUM_TRIALS) + j + 1 << " Wins vs Terran: " << (wins[Race::Terran].y) << "/" << (wins[Race::Terran].x + wins[Race::Terran].y) << endl;
			std::cout << "Game:" << (i*NUM_TRIALS) + j + 1 << " Wins vs Zerg: " << (wins[Race::Zerg].y) << "/" << (wins[Race::Zerg].x + wins[Race::Zerg].y) << endl;
			std::cout << "Game:" << (i*NUM_TRIALS) + j + 1 << " Wins vs Protoss: " << (wins[Race::Protoss].y) << "/" << (wins[Race::Protoss].x + wins[Race::Protoss].y) << endl;
		}
	}
#else
	InitResources();

	GenerateDependencyList();

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