#include "stdafx.h"
#include "sc2api/sc2_api.h"
#include <iostream>
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include "LadderInterface.h"
#include "BotWithAPlan.h"
#include "Common\Util\Util.h"
using namespace sc2;
const char* kAcidPlant = "Ladder/(2)AcidPlantLE.SC2Map";
const char* kDreamCatcher = "Ladder/(2)DreamCatcherLE.SC2Map";

// 
  
int main(int argc, char* argv[]) {

#if PLANNER_MODE
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
#else  // Run local sims

	bool isLadder = false;
	for (int i = 0; i < argc; i++)
	{
		LOG(4) << argv[i] << endl;
	}

	if (argc > 5)
		isLadder = true;

	if (isLadder)
	{
		RunBot(argc, argv, new BotWithAPlan(), sc2::Race::Protoss);

		return 0;
	}


	// Not ladder code
	auto races = new Race[3]{  Race::Terran, Race::Zerg,  Race::Protoss };
	std::map<std::string, sc2::Point2D> mapScore;
	std::map<sc2::Race, sc2::Point2D> raceScore;
	std::vector<std::string> maps = { "ZenLE.SC2Map", "ThunderbirdLE.SC2Map","GoldenWallLE.SC2Map",  "SimulacrumLE.SC2Map",  "NightshadeLE.SC2Map" ,"EverDreamLE.SC2Map" ,"EternalEmpireLE.SC2Map" };
	while (true)
	{
		for (int i = 0; i < maps.size(); i++)
		{
			auto map = "Ladder/" + maps[i];
			for (int j = 0; j < 3; j++)
			{

				auto race = races[j];

				Coordinator coordinator;
				coordinator.LoadSettings(argc, argv);
				coordinator.SetMultithreaded(true);
				coordinator.SetRealtime(REALTIME);
				//coordinator.SetStepSize(10);
				BotWithAPlan bot;
				BotWithAPlan bot2;

				coordinator.SetParticipants({
					//CreateParticipant(Race::Protoss, nullptr),
					CreateParticipant((Race)GetAgentRace(), &bot),
					//CreateParticipant((Race)GetAgentRace(), &bot2),
					CreateComputer(race, sc2::Difficulty::VeryHard, sc2::AIBuild::RandomBuild)
					});

				coordinator.LaunchStarcraft();
				coordinator.StartGame(map);
				while (coordinator.Update()) {
					if (bot.Lost) break;
					std::this_thread::sleep_for(std::chrono::milliseconds(0));
				}
				if (bot.errorOccurred)
				{
					// Try it again
					j--;
					continue;
				}
				if (bot.Observation() && bot.Observation()->GetResults().size() > 0 && bot.Observation()->GetResults().front().result == sc2::GameResult::Win)
				{
					mapScore[map] += sc2::Point2D(1, 0);
					raceScore[race] += sc2::Point2D(1, 0);
				}
				else
				{
					mapScore[map] += sc2::Point2D(0, 1);
					raceScore[race] += sc2::Point2D(0, 1);
				}
				std::cout << std::endl;
				for (const auto & rs : raceScore)
				{
					std::cout << Util::GetStringFromRace(rs.first) << " = " << rs.second.x << " : " << rs.second.y << " (" << roundf(rs.second.x / (rs.second.x + rs.second.y) * 100.0f) << "%)" << std::endl;
				}
				std::cout << std::endl;
				for (const auto & ms : mapScore)
				{
					std::cout << ms.first << " = " << ms.second.x << " : " << ms.second.y << " (" << roundf(ms.second.x / (ms.second.x + ms.second.y) * 100.0f)<< "%)" << std::endl;
				}
				std::cout << std::endl << std::endl;
				coordinator.LeaveGame();
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
	}


#endif

    return 0;
}