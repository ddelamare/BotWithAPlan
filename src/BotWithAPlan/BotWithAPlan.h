#include <sc2api/sc2_api.h>
#include "Common\Resource.h"
#include "Planner\Planner.h"
#include "Goals\GoalPicker.h"
#include "Common\Resource.h"
#include "Common\ItemDependencies.h"
#include "Common\Managers\ArmyManager.h"
#include "sc2api/sc2_api.h"
#include "Common\GameState.h"
#include <iostream>
#include "Common\Analyzers\ThreatAnalyzer.h"
#include "sc2api/sc2_api.h"
#include "sc2lib/sc2_lib.h"
#include "sc2utils/sc2_manage_process.h"
#include "sc2utils/sc2_arg_parser.h"
#include "Common\BotBase\PlanBotBase.h"
#define DllExport __declspec( dllexport ) 
using namespace sc2;

class BotWithAPlan : public PlanBotBase {
public:
	BotWithAPlan();
	void OnGameStart();
	void OnStep();
};


// Bot Ladder hooks
void DllExport *CreateNewAgent();
int DllExport GetAgentRace();
const char DllExport *GetAgentName();