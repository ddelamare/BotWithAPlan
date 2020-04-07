#pragma once
#include <string>
struct UnitBattleData
{
public:
	int id;
	std::string name;
	int maxHealth;
	int maxShields;
	int movementType;
	float radius;
};