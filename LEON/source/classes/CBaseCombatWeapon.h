#pragma once
#include "../interfaces/interfaces.h"
#include "../util/util.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

class CBaseHandle;
class CCSWeaponData;

class CBaseCombatWeapon
{
public:
	union
	{
		DEFINE_MEMBER_N(int, m_iClip1, 0x25d8);
		DEFINE_MEMBER_N(float, m_flNextPrimaryAttack, 0x2588);
		DEFINE_MEMBER_N(float, m_iState, 0x25c8);
		DEFINE_MEMBER_N(uintptr_t, m_hOwner, 0x2580);
		DEFINE_MEMBER_N(uintptr_t, m_fLastShotTime, 0x26a8);
	};

	CCSWeaponData& GetWeaponData()
	{
		typedef CCSWeaponData& (__thiscall* weaponData_t)(void*);
		static weaponData_t get_weapon_data;
		
		if (!get_weapon_data)
		{
			get_weapon_data = (weaponData_t)util::PatternScan("client.dll", "0F B7 81 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 C3");
		}
		
		return get_weapon_data(this);
	}

	const char* GetWeaponName();
	bool IsKnife();
	bool IsCanShoot();
	bool IsSniper();
	bool IsPistol();
	bool IsRifle();
	int GetWeaponMaxSpeed();
	float GetIdealAccurateSpeed();
	float GetWeaponPenetration();

	

};

class CCSWeaponData
{
public:
	char pad0[0x14];			// 0x0000
	int iMaxClip1;					// 0x0014
	int iMaxClip2;					// 0x0018
	int iDefaultClip1;				// 0x001C
	int iDefaultClip2;				// 0x0020
	int iPrimaryMaxReserveAmmo;		// 0x0024
	int iSecondaryMaxReserveAmmo;	// 0x0028
	const char* szWorldModel;		// 0x002C
	const char* szViewModel;		// 0x0030
	const char* szDroppedModel;		// 0x0034
	BYTE pad1[0x50];			// 0x0038
	const char* szHudName;			// 0x0088
	const char* szWeaponName;		// 0x008C
	char pad2[0x2];			// 0x0090
	bool bIsMeleeWeapon;			// 0x0092
	char pad3[0x9];			// 0x0093
	float flWeaponWeight;			// 0x009C
	char pad4[0x4];			// 0x00A0
	int iSlot;						// 0x00A4
	int iPosition;					// 0x00A8
	char pad5[0x1C];			// 0x00AC
	int nWeaponType;				// 0x00C8
	char pad6[0x4];			// 0x00CC
	int iWeaponPrice;				// 0x00D0
	int iKillAward;					// 0x00D4
	const char* szAnimationPrefix;	// 0x00D8
	float flCycleTime;				// 0x00DC
	float flCycleTimeAlt;			// 0x00E0
	char pad8[0x8];			// 0x00E4
	bool bFullAuto;					// 0x00EC
	char pad9[0x3];			// 0x00ED
	int iDamage;					// 0x00F0
	float flHeadShotMultiplier;		// 0x00F4
	float flArmorRatio;				// 0x00F8
	int iBullets;					// 0x00FC
	float flPenetration;			// 0x0100
	char pad10[0x8];			// 0x0104
	float flRange;					// 0x010C
	float flRangeModifier;			// 0x0110
	float flThrowVelocity;			// 0x0114
	char pad11[0xC];			// 0x0118
	bool bHasSilencer;				// 0x0124
	char pad12[0xF];			// 0x0125
	float flMaxSpeed[2];			// 0x0134
	char pad13[0x4];			// 0x013C
	float flSpread[2];				// 0x0140
	float flInaccuracyCrouch[2];	// 0x0148
	float flInaccuracyStand[2];		// 0x0150
	char pad14[0x8];			// 0x0158
	float flInaccuracyJump[2];		// 0x0160
	float flInaccuracyLand[2];		// 0x0168
	float flInaccuracyLadder[2];	// 0x0170
	float flInaccuracyFire[2];		// 0x0178
	float flInaccuracyMove[2];		// 0x0180
	float flInaccuracyReload;		// 0x0188
	int iRecoilSeed;				// 0x018C
	float flRecoilAngle[2];			// 0x0190
	float flRecoilAngleVariance[2];	// 0x0198
	float flRecoilMagnitude[2];		// 0x01A0
	float flRecoilMagnitudeVariance[2]; // 0x01A8
	int iSpreadSeed;				// 0x01B0
};