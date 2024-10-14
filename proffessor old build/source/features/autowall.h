#pragma once

class CEntity;
namespace AUTOWALL
{
	void ScaleDamage(int hitgroup, CEntity* enemy, float weapon_armor_ratio, float& current_damage);
}
