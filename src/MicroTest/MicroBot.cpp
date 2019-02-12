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
std::string testMap = "Test/Empty.SC2Map";
using namespace sc2;
class MicroBot : public Agent {
public:
	ArmyManager manager;
	GameState state;
	int trial;
	long lastLoop;
	int minLoop;
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
	}
	UNIT_TYPEID lhsUnit = UNIT_TYPEID::PROTOSS_TEMPEST;
	int lhsCount = 25;
	UNIT_TYPEID rhsUnit = UNIT_TYPEID::PROTOSS_VOIDRAY;
	int rhsCount = 45;
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
				int winner = (enemyUnits.size() > 0) + 1;
				printf("Actual results: %d %d %d\n", winner, selfUnits.size(), enemyUnits.size());
				for (auto unit : selfUnits)
				{
					Debug()->DebugKillUnit(unit);
				}
				for (auto unit : enemyUnits)
				{
					Debug()->DebugKillUnit(unit);
				}
				Debug()->DebugCreateUnit(lhsUnit, Point2D(40, 50), 1, lhsCount);
				Debug()->DebugCreateUnit(rhsUnit, Point2D(80, 50), 2, rhsCount);

				auto lhsUnitData = state.UnitInfo[(int)lhsUnit];
				auto rhsUnitData = state.UnitInfo[(int)rhsUnit];

				auto ab = new BattleAnalyzer("C:\\Users\\ddelam\\Documents\\Git\\BotWithAPlan\\src\\MicroTest\\Debug\\data\\unitinfo.json");
				auto lhsRelStr = ab->GetRelativeStrength(lhsUnit, rhsUnit, &state);
				auto rhsRelStr = ab->GetRelativeStrength(rhsUnit, lhsUnit, &state);
				auto predictedWinner = ab->PredictWinner(lhsRelStr, lhsCount, rhsRelStr, rhsCount, &lhsUnitData, &rhsUnitData);
				auto predictedSurviorsLhs = ab->PredictSurvivors(lhsRelStr, lhsCount, rhsRelStr, rhsCount, &lhsUnitData, &rhsUnitData);
				auto predictedSurviorsRhs = ab->PredictSurvivors(rhsRelStr, rhsCount, lhsRelStr, lhsCount, &rhsUnitData, &lhsUnitData);

				printf("Predicted Results: %f %f %d %f %f\n", lhsRelStr, rhsRelStr, predictedWinner, predictedSurviorsLhs, predictedSurviorsRhs);
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

