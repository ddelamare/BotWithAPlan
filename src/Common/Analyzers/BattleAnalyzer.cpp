#include "BattleAnalyzer.h"
#include "Common/UnitFIlters.h"
using namespace sc2;
using namespace rapidjson;
std::map<int, UnitBattleData> BattleAnalyzer::unitData;



BattleAnalyzer::BattleAnalyzer(string filepath)
{
	LoadConfigFromFile(filepath, false);
}


BattleAnalyzer::~BattleAnalyzer()
{
}


// Calculate relative strength of one unit to another. Basically returns an enemy units killed per game second.
// TODO: Account for special units like carrier who have 0 attack str in api
double BattleAnalyzer::GetRelativeStrength(UnitTypeID leftUnit, UnitTypeID rightUnit, GameState* state)
{
	auto relStr = 0.0;

	// Load unit info
	auto luType = state->UnitInfo[leftUnit];
	auto ruType = state->UnitInfo[rightUnit];


	Weapon* bestWeapon = GetBestWeapon(&luType, &ruType, state);

	// Fail conditions
	if (bestWeapon == nullptr) return 0;
	auto weaponDamage = CalculateWeaponHitDamage(bestWeapon, &ruType);

	// Save outselves from exceptions later
	if (weaponDamage == 0.0 || bestWeapon->speed == 0) return 0;
	
	auto attacksToKill = GetEstimatedAttacksToKill(weaponDamage, &ruType);

	relStr = (float)1 / (attacksToKill * bestWeapon->speed);

	return  relStr;
}

// Get the correct weapon to use. TODO: Find a way to do flying vs not
Weapon* BattleAnalyzer::GetBestWeapon(UnitTypeData* luType, UnitTypeData* ruType, GameState* state)
{
	Weapon* bestWeapon = nullptr;
	double maxDamage = 0.0;
	for (int i = 0; i < luType->weapons.size(); i++)
	{
		if (luType->weapons[i].type == Weapon::TargetType::Any
			|| unitData[(int)ruType->unit_type_id].movementType == (int)luType->weapons[i].type)
		{

			auto damagePerHit = CalculateWeaponHitDamage(&luType->weapons[i], ruType);
			if (damagePerHit > maxDamage)
			{
				bestWeapon = &luType->weapons[i];
				maxDamage = damagePerHit;
			}
		}
	}
	return bestWeapon;
}


double BattleAnalyzer::CalculateWeaponHitDamage(Weapon* weapon, UnitTypeData* enemyType)
{
	//TODO: skip weapons that can't target unit
	float damagePerHit = weapon->damage_;
	for (int i = 0; i < weapon->damage_bonus.size(); i++)
	{
		if (VectorHelpers::FoundInVector(enemyType->attributes, weapon->damage_bonus[i].attribute))
		{
			damagePerHit += weapon->damage_bonus[i].bonus;
		}
	}

	// Damage is per projectile or beam. Must scale
	damagePerHit *= weapon->attacks;
	return damagePerHit;
}

// TODO: multiply armor reduction based on weapon attacks
int BattleAnalyzer::GetEstimatedAttacksToKill(double damage, UnitTypeData* enemyType)
{
	auto battleData = unitData[enemyType->unit_type_id];
	auto hitsToKill = ((double)battleData.maxShields / damage) + (double(battleData.maxHealth) / (damage - enemyType->armor));
	return ceil(hitsToKill);
}

double exponent = 1.7;
// For optimized for ranged units
// TODO: Confidence interval?
// TODO: Could be a toss up
// TODO: Dynamic exponent based on unit range and stackability (including size and flying or not)
int BattleAnalyzer::PredictWinner(double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount, UnitTypeData* lhsUnit, UnitTypeData* rhsUnit)
{
	auto lhsArmyStr = lhsRelStr * pow(lhsCount, CalcuateExponent(lhsUnit, lhsCount));
	auto rhsArmyStr = rhsRelStr * pow(rhsCount, CalcuateExponent(rhsUnit, rhsCount));

	//Extra explicit cases;
	if (lhsArmyStr == rhsArmyStr) return 0;
	if (lhsArmyStr > rhsArmyStr) return 1;
	if (rhsArmyStr > lhsArmyStr) return 2;

	// fail case? How can we get here?
	return -1;
}

//TODO: Account for more variables?
// - Packing efficiency	 ie with naivly controlled units, few can actually be firing
// - hp regen			 ie likely only significant where it increases attacks to kill
// - range disparity	 ie slightly increase or decrease lhs exponent if there is a significant range disparity
// - Dynamic exponent based on unit range and stackability (including size and flying or not)
// TODO: Add pessimistic, mid, and optimistic results.
// TODO: Calculate actual exponent based on survivors
double BattleAnalyzer::PredictSurvivors(double lhsRelStr, int lhsCount, double rhsRelStr, int rhsCount, UnitTypeData* lhsUnit, UnitTypeData* rhsUnit)
{
	if (lhsRelStr == 0.0) return 0;
	if (rhsRelStr == 0.0) return lhsCount;

	auto lhsExp = CalcuateExponent(lhsUnit, lhsCount);
	auto rhsExp = CalcuateExponent(rhsUnit, rhsCount);

	auto survivorsSqaured = pow(lhsCount, lhsExp) - ((rhsRelStr / lhsRelStr)*pow(rhsCount, rhsExp));
	
	// If we modified the exponent, it's more accureate to use average exp for root
	auto combinedExp = (1 / (((lhsExp + rhsExp) / 2)));
	auto combinedExp2 = (2 / (lhsExp + rhsExp));

	// They'd lose thier imaginary army
 	if (survivorsSqaured < 0) return -pow(-survivorsSqaured, combinedExp);
	
	return pow(survivorsSqaured, combinedExp);
}

double BattleAnalyzer::CalcuateExponent(UnitTypeData* lhsUnit, int lhsCount)
{
	if (lhsUnit->unit_type_id == UNIT_TYPEID::ZERG_ROACH)
	{
		return exponent - .05;
	}
	if (lhsUnit->unit_type_id == UNIT_TYPEID::ZERG_ZERGLING)
	{
		return 1.4;
	}
	// Melee units
	if (lhsUnit->weapons[0].range < 1)
	{
		return 1.5;
	}

	return exponent;
}


void BattleAnalyzer::LoadConfigFromFile(string filepath, bool forceClear)
{
	// Don't double load
	if (!unitData.empty() && !forceClear)
		return;
	unitData.clear();
	std::fstream jsonFiles;
	jsonFiles.open(filepath);
	std::stringstream stream;

	// Read whole file into stream
	stream << jsonFiles.rdbuf();
	Document unitDoc;
	unitDoc.Parse(stream.str().c_str());
	//static const char* kTypeNames[] =
	//{ "Null", "False", "True", "Object", "Array", "String", "Number" };
	// Loop through each unit
	for (Value::ConstValueIterator itr = unitDoc.Begin(); itr != unitDoc.End(); ++itr) {

		// Skip non-units
		if (!itr->HasMember("maxHealth")) continue;

		// Pull all fields
		int id = itr->FindMember("id")->value.GetInt();
		string name = itr->FindMember("name")->value.GetString();
		int maxHealth = itr->FindMember("maxHealth")->value.GetInt();
		int maxShields = 0;
		int movementType = 0;
		if (itr->HasMember("maxShields"))
			maxShields = itr->FindMember("maxShields")->value.GetInt();
		movementType = itr->FindMember("movementType")->value.GetInt();
		auto unitBattleData = UnitBattleData();
		unitBattleData.id = id;
		unitBattleData.name = name;
		unitBattleData.maxHealth = maxHealth;
		unitBattleData.maxShields = maxShields;
		unitBattleData.movementType = movementType;

		unitData[id] = unitBattleData;
		//for (Value::ConstMemberIterator itr2 = itr->MemberBegin();
		//	itr2 != itr->MemberEnd(); ++itr2)
		//{
		//	printf("Type of member %s is %s\n",
		//		itr2->name.GetString(), kTypeNames[itr2->value.GetType()]);
		//}
	}
	LOG(1) << "Loaded Unit Data\n";

}
