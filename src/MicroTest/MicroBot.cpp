#include "sc2api/sc2_api.h"

#include "sc2utils/sc2_arg_parser.h"
#include "sc2utils/sc2_manage_process.h"
#include "Common\Managers\ArmyManager.h"
#include "Common\Strategy\Attacks\BlinkStalker.h"
#include "Common\Strategy\Attacks\DisruptorAttack.h"
#include "Common\Strategy\Attacks\VoidRay.h"
#include "Common\Strategy\Attacks\Templar.h"
#include "Common\Strategy\Attacks\SentryMicro.h"
#include "Common\Strategy\Attacks\Phoenix.h"
#include "Common\Analyzers\BattleAnalyzer.h"
#include <iostream>
#include <tuple>
#include <vector>
std::string testMap = "Test/Empty.SC2Map";
using namespace sc2;

typedef vector<std::tuple<UNIT_TYPEID, int, UNIT_TYPEID, int>> TestSetup;
class MicroBot : public Agent {
public:
	ArmyManager manager;
	GameState state;
	int trial;
	long lastLoop;
	int minLoop;
	TestSetup testSetups;
	MicroBot() {
		trial = 0;
		lastLoop = 0;
		minLoop = 100;
	}
	void OnGameStart()
	{
		//Debug()->DebugGiveAllTech();
		Debug()->DebugShowMap();
		Debug()->DebugEnemyControl();
		Debug()->SendDebug();

		state.UnitInfo = Observation()->GetUnitTypeData();

		testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_MARINE, 10, UNIT_TYPEID::PROTOSS_ADEPT, 5));
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_MARINE, 20, UNIT_TYPEID::PROTOSS_ADEPT, 10));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_MARINE, 40, UNIT_TYPEID::PROTOSS_ADEPT, 20));
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_MARINE, 80, UNIT_TYPEID::PROTOSS_ADEPT, 40));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_MARINE, 160, UNIT_TYPEID::PROTOSS_ADEPT, 80));
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_MARINE, 320, UNIT_TYPEID::PROTOSS_ADEPT, 160));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_THOR, 20, UNIT_TYPEID::ZERG_ZERGLING, 240));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_THOR, 20, UNIT_TYPEID::PROTOSS_ZEALOT, 150));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::PROTOSS_TEMPEST, 20, UNIT_TYPEID::PROTOSS_PHOENIX, 30));
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_MARINE, 320, UNIT_TYPEID::PROTOSS_ADEPT, 160));
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_MARINE, 320, UNIT_TYPEID::PROTOSS_ADEPT, 160));
		/*testSetups.push_back(std::make_tuple(UNIT_TYPEID::PROTOSS_COLOSSUS, 1, UNIT_TYPEID::TERRAN_MARINE, 6));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::PROTOSS_COLOSSUS, 2, UNIT_TYPEID::TERRAN_MARINE, 12));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::PROTOSS_COLOSSUS, 5, UNIT_TYPEID::TERRAN_MARINE, 30));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::PROTOSS_COLOSSUS, 10, UNIT_TYPEID::TERRAN_MARINE, 60));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::PROTOSS_COLOSSUS, 15, UNIT_TYPEID::TERRAN_MARINE, 90));
		testSetups.push_back(std::make_tuple(UNIT_TYPEID::PROTOSS_COLOSSUS, 30, UNIT_TYPEID::TERRAN_MARINE, 180));*/
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::PROTOSS_ARCHON, 25, UNIT_TYPEID::PROTOSS_VOIDRAY, 25));
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_HELLION, 1, UNIT_TYPEID::PROTOSS_VOIDRAY, 1));
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_HELLION, 25, UNIT_TYPEID::ZERG_HYDRALISK, 25));
		//testSetups.push_back(std::make_tuple(UNIT_TYPEID::TERRAN_SIEGETANKSIEGED, 25, UNIT_TYPEID::ZERG_HYDRALISK, 25));


	}

	void OnStep() {
		//manager.ManageGroups(Observation(), Query(), Actions(), &state, Debug());

		if (Observation()->GetGameLoop() < minLoop)
		{
			return;
		}
		if (Observation()->GetGameLoop() > lastLoop + minLoop)
		{

			auto selfUnits = Observation()->GetUnits(Unit::Alliance::Self);
			auto enemyUnits = Observation()->GetUnits(Unit::Alliance::Enemy);


			if (selfUnits.size() == 0 || enemyUnits.size() == 0)
			{
			    auto ab = new BattleAnalyzer("C:\\Users\\ddelam\\Documents\\Git\\BotWithAPlan\\src\\MicroTest\\Debug\\data\\unitinfo.json");

				// Tear down last test
				int winner = (enemyUnits.size() > 0) + 1;
				cout << "Actual results: " << winner << " " << selfUnits.size() << " " << enemyUnits.size() << endl;

				double estimatedExp;
				if (winner == 1)
				{
					estimatedExp = ab->EstimateActualExponent(selfUnits.size(), enemyUnits.size(), lhsRelStr, lhsCount, rhsRelStr, rhsCount, &lhsUnitData, &rhsUnitData, 10, &state);
				}
				else
				{
					estimatedExp = ab->EstimateActualExponent(enemyUnits.size(), selfUnits.size(), rhsRelStr, rhsCount, lhsRelStr, lhsCount, &lhsUnitData, &rhsUnitData, 10, &state);
				}
				cout << "Estimated Exponent: " << estimatedExp << endl;

				for (auto unit : selfUnits)
				{
					Debug()->DebugKillUnit(unit);
				}
				for (auto unit : enemyUnits)
				{
					Debug()->DebugKillUnit(unit);
				}

				// Setup new test
				setupIdx = trial % testSetups.size();
				lhsUnit = std::get<0>(testSetups[setupIdx]);
				lhsCount = std::get<1>(testSetups[setupIdx]);
				rhsUnit = std::get<2>(testSetups[setupIdx]);
				rhsCount = std::get<3>(testSetups[setupIdx]);

				Debug()->DebugCreateUnit(lhsUnit, Point2D(40, 50), 1, lhsCount);
				Debug()->DebugCreateUnit(rhsUnit, Point2D(80, 50), 2, rhsCount);

				lhsUnitData = ab->unitData[(int)lhsUnit];
				rhsUnitData = ab->unitData[(int)rhsUnit];

				lhsRelStr = ab->GetRelativeStrength(lhsUnit, rhsUnit, &state);
				rhsRelStr = ab->GetRelativeStrength(rhsUnit, lhsUnit, &state);
				predictedWinner = ab->PredictWinner(lhsRelStr, lhsCount, rhsRelStr, rhsCount, &lhsUnitData, &rhsUnitData, &state);
				predictedSurviorsLhs = ab->PredictSurvivors(lhsRelStr, lhsCount, rhsRelStr, rhsCount, &lhsUnitData, &rhsUnitData, &state);
				predictedSurviorsRhs = ab->PredictSurvivors(rhsRelStr, rhsCount, lhsRelStr, lhsCount, &rhsUnitData, &lhsUnitData, &state);

				cout << "Matchup: " << state.UnitInfo[(int)lhsUnit].name << ": " << lhsCount <<" vs. "<< state.UnitInfo[(int)rhsUnit].name << ": " << rhsCount << endl;
				cout << "Predicted Results: " << lhsRelStr << " " << rhsRelStr << " " << predictedWinner << " " << predictedSurviorsLhs << " " << predictedSurviorsRhs << endl;
			
				trial++;
			}

			Actions()->UnitCommand(selfUnits, ABILITY_ID::ATTACK, Point2D(60, 50));
			Actions()->UnitCommand(enemyUnits, ABILITY_ID::ATTACK, Point2D(60, 50));

			Actions()->SendActions();
			Debug()->SendDebug();
			lastLoop = Observation()->GetGameLoop();
		}
	}
	void OnError(const std::vector<sc2::ClientError> & client_errors, const std::vector<std::string> & protocol_errors)
	{
		for (const auto & error : protocol_errors)
		{
			std::cout << "Protocol error: " << error << std::endl;
		}
		for (const auto & error : client_errors)
		{
			//std::cerr << "Client error: " << ClientErrorToString(error) << std::endl;
		}

	}
private:
	unsigned long long setupIdx;
	UNIT_TYPEID lhsUnit;
	int lhsCount;
	UNIT_TYPEID rhsUnit;
	int rhsCount;
	double lhsRelStr;
	double rhsRelStr;
	int    predictedWinner;
	double predictedSurviorsLhs;
	double predictedSurviorsRhs;
	UnitBattleData lhsUnitData;
	UnitBattleData rhsUnitData;
};



//*************************************************************************************************
int main(int argc, char* argv[]) {
	sc2::Coordinator coordinator;
	if (!coordinator.LoadSettings(argc, argv)) {
		return 1;
	}

	coordinator.SetRealtime(false);

	// Add the custom bot, it will control the player.
	MicroBot bot;
	coordinator.SetParticipants({
		CreateParticipant(sc2::Race::Protoss, &bot),
		CreateComputer(sc2::Race::Protoss),
		});

	// Start the game.
	coordinator.LaunchStarcraft();
	coordinator.StartGame(testMap);

	while (coordinator.Update()) {
		if (sc2::PollKeyPress()) {
			break;
		}
	}

	return 0;
}

