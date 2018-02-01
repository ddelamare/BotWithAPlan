#include <sc2api/sc2_api.h>
#include "Common\Resource.h"
#include "Planner\Planner.h"
#include "Common\Resource.h"
#include "Common\ItemDependencies.h"
#include <iostream>
#define DllExport __declspec( dllexport ) 
using namespace sc2;

class BotWithAPlan : public Agent {
public:
	void OnGameStart();
	void OnStep();
};





// Bot Ladder hooks
void DllExport *CreateNewAgent();
int DllExport GetAgentRace();
const char DllExport *GetAgentName();