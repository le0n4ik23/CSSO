#include "CBaseCombatWeapon.h"

const char* CBaseCombatWeapon::GetWeaponName()
{
    CEntity* weapon = (CEntity*)this;
    if (!weapon)
        return "N/A";

    int weaponID = weapon->GetClientClass()->classID;

    const char* weapName;

    switch (weaponID)
    {
    case ClassID::CWeaponAK47:
        weapName = "AK-47";
        break;

    case ClassID::CWeaponAUG:
        weapName = "AUG";
        break;

    case ClassID::CWeaponAWP:
        weapName = "AWP";
        break;

    case ClassID::CWeaponBizon:
        weapName = "BIZON";
        break;

    case ClassID::CWeaponCZ75A:
        weapName = "CZ-75";
        break;

    case ClassID::CWeaponDeagle:
        weapName = "DEAGLE";
        break;

    case ClassID::CWeaponElite:
        weapName = "DUALIES";
        break;

    case ClassID::CWeaponFAMAS:
        weapName = "FAMAS";
        break;

    case ClassID::CWeaponFiveSeven:
        weapName = "FIVE-SEVEN";
        break;

    case ClassID::CWeaponG3SG1:
        weapName = "G3SG1";
        break;

    case ClassID::CWeaponGalilAR:
        weapName = "GALIL";
        break;

    case ClassID::CWeaponGlock:
        weapName = "GLOCK";
        break;

    case ClassID::CWeaponHKP2000:
        weapName = "P2000";
        break;

    case ClassID::CWeaponM249:
        weapName = "M249";
        break;

    case ClassID::CWeaponM4A1:
        weapName = "M4A1-S";
        break;

    case ClassID::CWeaponM4A4:
        weapName = "M4A4";
        break;

    case ClassID::CWeaponMAC10:
        weapName = "MAC-10";
        break;

    case ClassID::CWeaponMAG7:
        weapName = "MAG-7";
        break;

    case ClassID::CWeaponMP5SD:
        weapName = "MP5";
        break;

    case ClassID::CWeaponMP7:
        weapName = "MP7";
        break;

    case ClassID::CWeaponMP9:
        weapName = "MP9";
        break;

    case ClassID::CWeaponNegev:
        weapName = "NEGEV";
        break;

    case ClassID::CWeaponNova:
        weapName = "NOVA";
        break;

    case ClassID::CWeaponP250:
        weapName = "P250";
        break;

    case ClassID::CWeaponP90:
        weapName = "P90";
        break;

    case ClassID::CWeaponRevolver:
        weapName = "REVOLVER R8";
        break;

    case ClassID::CWeaponSawedOff:
        weapName = "SAWED-OFF";
        break;

    case ClassID::CWeaponSCAR20:
        weapName = "SCAR-20";
        break;

    case ClassID::CWeaponSG556:
        weapName = "SG-556";
        break;

    case ClassID::CWeaponSSG08:
        weapName = "SCOUT";
        break;

    case ClassID::CWeaponTaser:
        weapName = "TASER";
        break;

    case ClassID::CWeaponTec9:
        weapName = "TEC-9";
        break;

    case ClassID::CWeaponUMP45:
        weapName = "UMP-45";
        break;

    case ClassID::CWeaponUSP:
        weapName = "USP-S";
        break;

    case ClassID::CWeaponXM1014:
        weapName = "XM1014";
        break;

    case ClassID::CMolotovGrenade:
        weapName = "MOLOTOV";
        break;

    case ClassID::CIncendiaryGrenade:
        weapName = "INCENDIARY GRENADE";
        break;

    case ClassID::CFlashbang:
        weapName = "FLASHBANG";
        break;

    case ClassID::CDecoyGrenade:
        weapName = "DECOY";
        break;

    case ClassID::CSmokeGrenade:
        weapName = "SMOKE";
        break;

    case ClassID::CKnife:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifeBayonet:
        weapName = "BAYONET KNIFE";
        break;

    case ClassID::CKnifeButterfly:
        weapName = "BUTTERFLY KNIFE";
        break;

    case ClassID::CKnifeCanis:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifeCord:
        weapName = "PARACORD KNIFE";
        break;

    case ClassID::CKnifeCSS:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifeFalchion:
        weapName = "FALCHION KNIFE";
        break;

    case ClassID::CKnifeFlip:
        weapName = "FLIP KNIFE";
        break;

    case ClassID::CKnifeGG:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifeGypsy:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifeKarambit:
        weapName = "KARAMBIT";
        break;

    case ClassID::CKnifeM9Bayonet:
        weapName = "M9 BAYONET";
        break;

    case ClassID::CKnifeOutdoor:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifePush:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifeSkeleton:
        weapName = "SKELETON KNIFE";
        break;

    case ClassID::CKnifeStiletto:
        weapName = "STILETTO";
        break;

    case ClassID::CKnifeSurvivalBowie:
        weapName = "BOWIE KNIFE";
        break;

    case ClassID::CKnifeT:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifeTactical:
        weapName = "KNIFE";
        break;

    case ClassID::CKnifeUrsus:
        weapName = "URSUS KNIFE";
        break;

    case ClassID::CKnifeWidowmaker:
        weapName = "KNIFE";
        break;

    default:
        weapName = "N/A";
    }

    return weapName;
}

bool CBaseCombatWeapon::IsKnife()
{
    CEntity* weapon = (CEntity*)this;
    if (!weapon)
        return false;

    int weaponID = weapon->GetClientClass()->classID;


    switch (weaponID)
    {
    case ClassID::CKnife:
    case ClassID::CKnifeBayonet:
    case ClassID::CKnifeButterfly:
    case ClassID::CKnifeCanis:
    case ClassID::CKnifeCord:
    case ClassID::CKnifeCSS:
    case ClassID::CKnifeFalchion:
    case ClassID::CKnifeFlip:
    case ClassID::CKnifeGG:
    case ClassID::CKnifeGypsy:
    case ClassID::CKnifeKarambit:
    case ClassID::CKnifeM9Bayonet:
    case ClassID::CKnifeOutdoor:
    case ClassID::CKnifePush:
    case ClassID::CKnifeSkeleton:
    case ClassID::CKnifeStiletto:
    case ClassID::CKnifeSurvivalBowie:
    case ClassID::CKnifeT:
    case ClassID::CKnifeTactical:
    case ClassID::CKnifeUrsus:
    case ClassID::CKnifeWidowmaker:
        return true;
        break;

    default:
        return false;
    }
}

bool CBaseCombatWeapon::IsSniper()
{
    CEntity* weapon = (CEntity*)this;
    if (!weapon)
        return false;

    int weaponID = weapon->GetClientClass()->classID;

    switch (weaponID)
    {
    case ClassID::CWeaponAWP:
    case ClassID::CWeaponG3SG1:
    case ClassID::CWeaponSSG08:
    case ClassID::CWeaponSCAR20:
        return true;
        break;

    default:
        return false;
    }

}

bool CBaseCombatWeapon::IsPistol()
{
    CEntity* weapon = (CEntity*)this;
    if (!weapon)
        return false;

    int weaponID = weapon->GetClientClass()->classID;

    switch (weaponID)
    {
    case ClassID::CWeaponUSP:
    case ClassID::CWeaponHKP2000:
    case ClassID::CWeaponGlock:
    case ClassID::CWeaponFiveSeven:
    case ClassID::CWeaponCZ75A:
    case ClassID::CWeaponElite:
    case ClassID::CWeaponP250:
    case ClassID::CWeaponTec9:

        return true;
        break;

    default:
        return false;
    }
}

bool CBaseCombatWeapon::IsDeagle()
{
    CEntity* weapon = (CEntity*)this;
    if (!weapon)
        return false;

    int weaponID = weapon->GetClientClass()->classID;

    switch (weaponID)
    {
    case ClassID::CWeaponDeagle:

        return true;
        break;

    default:
        return false;
    }
}

int CBaseCombatWeapon::GetWeaponMaxSpeed()
{
    CEntity* weapon = (CEntity*)this;
    if (!weapon)
        return false;

    int weaponID = weapon->GetClientClass()->classID;


    switch (weaponID)
    {
    case ClassID::CWeaponHKP2000:
    case ClassID::CWeaponGlock:
    case ClassID::CWeaponFiveSeven:
    case ClassID::CWeaponCZ75A:
    case ClassID::CWeaponElite:
    case ClassID::CWeaponP250:
    case ClassID::CWeaponUSP:
    case ClassID::CWeaponTec9:
    case ClassID::CWeaponUMP45:
    case ClassID::CWeaponBizon:
    case ClassID::CWeaponMAC10:
    case ClassID::CWeaponMP5SD:
    case ClassID::CWeaponMP7:
    case ClassID::CWeaponMP9:
    case ClassID::CWeaponP90:
        return 240;
        break;

    case ClassID::CWeaponSSG08:
        return 230;
        break;

    case ClassID::CWeaponM249:
        return 195;
        break;

    case ClassID::CWeaponNegev:
        return 150;
        break;

    case ClassID::CWeaponSG556:
    case ClassID::CWeaponM4A4:
    case ClassID::CWeaponM4A1:
    case ClassID::CWeaponGalilAR:
    case ClassID::CWeaponFAMAS:
    case ClassID::CWeaponAUG:
    case ClassID::CWeaponAK47:
        return 215;
        break;


    case ClassID::CWeaponSCAR20:
    case ClassID::CWeaponG3SG1:
        return 215;
        break;

    case ClassID::CWeaponAWP:
        return 100;
        break;

    case ClassID::CWeaponRevolver:
        return 180;
        break;

    case ClassID::CWeaponNova:
    case ClassID::CWeaponSawedOff:
    case ClassID::CWeaponXM1014:
    case ClassID::CWeaponMAG7:
        return 210;
        break;

    default:
        return 100;
    }
}

float CBaseCombatWeapon::GetIdealAccurateSpeed()
{
    return GetWeaponMaxSpeed() / 3.f;;
}


