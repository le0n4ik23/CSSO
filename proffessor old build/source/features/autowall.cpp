#include "autowall.h"
#include "../interfaces/interfaces.h"

void AUTOWALL::ScaleDamage(int hitgroup, CEntity* enemy, float weapon_armor_ratio, float& current_damage)
{
    current_damage *= 4.0f;

    if (enemy->m_ArmorValue > 0)
    {
        if (hitgroup == 1)
        {
            if (enemy->m_bHasHelmet)
                current_damage *= (weapon_armor_ratio * 0.5f);
        }
        else
        {
            current_damage *= (weapon_armor_ratio * 0.5f);
        }
    }

    // Simulate penetration through obstacles
    // Adjust current_damage based on material thickness and properties of the obstacle
    float penetration = 0.0f; // Placeholder for penetration calculation
    current_damage *= penetration;
}

