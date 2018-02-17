#pragma once
#include "ItemDependencies.h"

vector<BaseAction*> AvailableActions;

void GenerateDependencyList()
{
	vector<BaseCondition*> conds;
	vector<BaseResult*> result;

conds.push_back(new BaseCondition("Build PROTOSS_PROBE", 2, sc2::UNIT_TYPEID::PROTOSS_NEXUS, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_PROBE,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_PROBE", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_GATEWAY", 4, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_GATEWAY,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_GATEWAY", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_ZEALOT", 2, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ZEALOT,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_ZEALOT", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_STALKER", 4, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_STALKER,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_STALKER", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_CYBERNETICSCORE", 6, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_CYBERNETICSCORE", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_PYLON", 2, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_PYLON,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_PYLON", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_ASSIMILATOR", 2, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_ASSIMILATOR,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_ASSIMILATOR", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_NEXUS", 2, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_NEXUS,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_NEXUS", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_STARGATE", 6, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_STARGATE,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_STARGATE", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_FLEETBEACON", 6, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_STARGATE, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_FLEETBEACON", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_CARRIER", 4, sc2::UNIT_TYPEID::PROTOSS_STARGATE, 1, sc2::UNIT_TYPEID::PROTOSS_FLEETBEACON, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_CARRIER,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_CARRIER", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_TWILIGHTCOUNCIL", 6, sc2::UNIT_TYPEID::PROTOSS_PROBE, 1, sc2::UNIT_TYPEID::PROTOSS_PYLON, 1, sc2::UNIT_TYPEID::PROTOSS_CYBERNETICSCORE, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_TWILIGHTCOUNCIL", conds, result));
result.clear();
conds.clear();

conds.push_back(new BaseCondition("Build PROTOSS_DARKTEMPLAR", 4, sc2::UNIT_TYPEID::PROTOSS_GATEWAY, 1, sc2::UNIT_TYPEID::PROTOSS_TWILIGHTCOUNCIL, 1));
result.push_back(new BaseResult(sc2::UNIT_TYPEID::PROTOSS_DARKTEMPLAR,1));
AvailableActions.push_back(new BaseAction("Build PROTOSS_DARKTEMPLAR", conds, result));
result.clear();
conds.clear();




}
