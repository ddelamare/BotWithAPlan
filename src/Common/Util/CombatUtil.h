#pragma once
#pragma once
#include "sc2api\sc2_api.h"
#include "UnitFilters.h"
#include "VectorHelpers.h"
#include <vector>
namespace Util 
{

	static double CalculateWeaponHitDamage(Weapon* weapon, UnitTypeData* enemyType)
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

	static Weapon* GetBestWeapon(const Unit* attacker,const Unit* reciever, GameState* state)
	{
		Weapon* bestWeapon = nullptr;
		auto unittypeAttacker = &state->UnitInfo[attacker->unit_type];
		auto unittypeReciever = &state->UnitInfo[reciever->unit_type];

		bool isFlying = IsFlying()(*reciever);

		double maxDamage = 0.0;
		for (int i = 0; i < unittypeAttacker->weapons.size(); i++)
		{
			if (unittypeAttacker->weapons[i].type == Weapon::TargetType::Any
				|| (!isFlying && unittypeAttacker->weapons[i].type == Weapon::TargetType::Ground)
				|| (isFlying && unittypeAttacker->weapons[i].type == Weapon::TargetType::Air))
			{
				auto damagePerHit = CalculateWeaponHitDamage(&unittypeAttacker->weapons[i], unittypeReciever);
				if (damagePerHit > maxDamage)
				{
					bestWeapon = &unittypeAttacker->weapons[i];
					maxDamage = damagePerHit;
				}
			}
		}
		return bestWeapon;
	}

	static bool CanAttack(const Unit* attacker,const  Unit* reciever, GameState* state)
	{
		return GetBestWeapon(attacker, reciever, state) != nullptr;
	}

	static int GetEstimatedAttacksToKill(const Unit* attacker, const Unit* reciever, GameState* state)
	{
		auto weapon = GetBestWeapon(attacker, reciever, state);
		if (!weapon) return INT_MAX;
		auto unittypeReciever = &state->UnitInfo[reciever->unit_type];
		auto damage = CalculateWeaponHitDamage(weapon, unittypeReciever);
		if (damage == 0) return INT_MAX;

		int armorDefense = weapon->attacks * (unittypeReciever->armor + reciever->armor_upgrade_level);
		int shieldDefense = weapon->attacks * (reciever->armor_upgrade_level);

		if (armorDefense > damage || shieldDefense > damage) return INT_MAX;

		auto hitsToKill = ((double)reciever->shield / (damage - shieldDefense) + (double(reciever->health) / (damage - armorDefense)));
		return ceil(hitsToKill);
	}

	static double GetDPS(const Unit* attacker, const Unit* reciever, GameState* state)
	{
		auto weapon = GetBestWeapon(attacker, reciever, state);
		if (!weapon || weapon->speed == 0) return 0;
		auto damage = CalculateWeaponHitDamage(weapon, &state->UnitInfo[reciever->unit_type]) * weapon->attacks;


		// Take a wild guess and assume splash damage will 3x the damage
		if (IsSplashUnit()(*attacker))
		{
			damage *= 3;
		}

		return damage / weapon->speed;
	}
}