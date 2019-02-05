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
#include <iostream>
std::string testMap = "Test/ColossusKite.SC2Map";
using namespace sc2;
class MicroBot : public Agent {
public:
	ArmyManager manager;
	GameState state;
	MicroBot() {
	}
	void OnGameStart()
 	{
		//Debug()->DebugGiveAllTech();
		Debug()->SendDebug();
		
		state.UnitInfo = Observation()->GetUnitTypeData();


	}

	void OnStep() {
		manager.ManageGroups(Observation(), Query(), Actions(), &state, Debug());

		auto bsMicro = BlinkStalker(Observation(), Query(), &state);
		bsMicro.Execute(Observation(), Actions(), Query(), Debug(), &state);
		auto disruptorMicro = DisruptorAttack(Observation(), Query(), &state);
		disruptorMicro.Execute(Observation(), Actions(), Query(), Debug(), &state);
		auto tMicro = TemplarMicro(Observation(), Query(), &state);
		tMicro.Execute(Observation(), Actions(), Query(), Debug(), &state);
		auto vMicro = VoidRayAttack(Observation(), Query(), &state);
		vMicro.Execute(Observation(), Actions(), Query(), Debug(), &state);
		auto sMicro = SentryMicro(Observation(), Query(), &state);
		sMicro.Execute(Observation(), Actions(), Query(), Debug(), &state);
		auto pMicro = PhoenixLift(Observation(), Query(), &state);
		pMicro.Execute(Observation(), Actions(), Query(), Debug(), &state);

		Actions()->SendActions();
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

