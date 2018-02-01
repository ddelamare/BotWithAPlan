#include "BotWithAPlan.h"

void BotWithAPlan::OnGameStart() {
	LOG(1) << "Bot initialized" << endl;
}

void BotWithAPlan::OnStep() {
	LOG(4) << "Step Begin" << endl;
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
