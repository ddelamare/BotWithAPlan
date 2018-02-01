#pragma once
#include "ItemDependencies.h"

vector<BaseAction*> AvailableActions;

void GenerateDependencyList()
{
	vector<BaseCondition*> conds;
	vector<BaseResult*> result;

conds.push_back(new BaseCondition("Build MINERALS", 2, RESOURCES::PROBE, 1));
result.push_back(new BaseResult(RESOURCES::MINERALS,1));
AvailableActions.push_back(new BaseAction("Build MINERALS", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROBE", 2, RESOURCES::NEXUS, 1));
result.push_back(new BaseResult(RESOURCES::PROBE,1));
AvailableActions.push_back(new BaseAction("Build PROBE", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build GATEWAY", 4, RESOURCES::PYLON, 1, RESOURCES::PROBE, 1));
result.push_back(new BaseResult(RESOURCES::GATEWAY,1));
AvailableActions.push_back(new BaseAction("Build GATEWAY", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build ZEALOT", 2, RESOURCES::GATEWAY, 1));
result.push_back(new BaseResult(RESOURCES::ZEALOT,1));
AvailableActions.push_back(new BaseAction("Build ZEALOT", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build STALKER", 4, RESOURCES::GATEWAY, 1, RESOURCES::CYBERNETICS, 1));
result.push_back(new BaseResult(RESOURCES::STALKER,1));
AvailableActions.push_back(new BaseAction("Build STALKER", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build CYBERNETICS", 6, RESOURCES::PYLON, 1, RESOURCES::GATEWAY, 1, RESOURCES::PROBE, 1));
result.push_back(new BaseResult(RESOURCES::CYBERNETICS,1));
AvailableActions.push_back(new BaseAction("Build CYBERNETICS", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PYLON", 2, RESOURCES::PROBE, 1));
result.push_back(new BaseResult(RESOURCES::PYLON,1));
AvailableActions.push_back(new BaseAction("Build PYLON", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build ASSIMILATOR", 2, RESOURCES::PROBE, 1));
result.push_back(new BaseResult(RESOURCES::ASSIMILATOR,1));
AvailableActions.push_back(new BaseAction("Build ASSIMILATOR", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build NEXUS", 2, RESOURCES::PROBE, 1));
result.push_back(new BaseResult(RESOURCES::NEXUS,1));
AvailableActions.push_back(new BaseAction("Build NEXUS", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build STARGATE", 6, RESOURCES::PROBE, 1, RESOURCES::CYBERNETICS, 1, RESOURCES::PYLON, 1));
result.push_back(new BaseResult(RESOURCES::STARGATE,1));
AvailableActions.push_back(new BaseAction("Build STARGATE", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build FLEETBEACON", 6, RESOURCES::PROBE, 1, RESOURCES::PYLON, 1, RESOURCES::STARGATE, 1));
result.push_back(new BaseResult(RESOURCES::FLEETBEACON,1));
AvailableActions.push_back(new BaseAction("Build FLEETBEACON", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build CARRIER", 4, RESOURCES::STARGATE, 1, RESOURCES::FLEETBEACON, 1));
result.push_back(new BaseResult(RESOURCES::CARRIER,1));
AvailableActions.push_back(new BaseAction("Build CARRIER", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build TWILIGHT", 6, RESOURCES::PROBE, 1, RESOURCES::PYLON, 1, RESOURCES::CYBERNETICS, 1));
result.push_back(new BaseResult(RESOURCES::TWILIGHT,1));
AvailableActions.push_back(new BaseAction("Build TWILIGHT", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build DARKTEMPLAR", 4, RESOURCES::GATEWAY, 1, RESOURCES::TWILIGHT, 1));
result.push_back(new BaseResult(RESOURCES::DARKTEMPLAR,1));
AvailableActions.push_back(new BaseAction("Build DARKTEMPLAR", conds, result));
result.clear();
conds.clear();




}
