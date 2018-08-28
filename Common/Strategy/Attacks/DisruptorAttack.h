#pragma once
#include <Common/Util.h>
using namespace sc2;

class DisruptorAttack
{
	UnitTypeData unitInfo;
public:
	DisruptorAttack(const sc2::ObservationInterface *obs, sc2::QueryInterface* query);
	void Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state);

};
DisruptorAttack::DisruptorAttack(const sc2::ObservationInterface *obs, sc2::QueryInterface* query)
{
	auto info = obs->GetUnitTypeData();
	unitInfo = info[(int)UNIT_TYPEID::PROTOSS_DISRUPTOR];
}
void DisruptorAttack::Execute(const sc2::ObservationInterface *obs, sc2::ActionInterface* actions, sc2::QueryInterface* query, sc2::DebugInterface* debug, GameState* state)
{
	// We only look for unphased disruptors
	auto disruptors = obs->GetUnits(Unit::Alliance::Self, IsUnit(UNIT_TYPEID::PROTOSS_DISRUPTOR));
	for (auto unit : disruptors)
	{
	     
	}

}