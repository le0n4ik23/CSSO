#pragma once
#include "../pch.h"
#include "../util/vector.h"
#include "../util/netvar.h"
#include "../util/util.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a, b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__)[size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}

class CBaseCombatWeapon;
class CEntity;
class IClientEntityList;
class IGlobalVars;

class IClientUnknown
{
public:
	CEntity* GetBaseEntity() noexcept
	{
		return util::get_vfunc<7, CEntity*>(this);
	}
};
class IClientRenderable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;

	// Data accessors
	virtual Vector3 const& GetRenderOrigin(void) = 0;
	virtual Vector3 const& GetRenderAngles(void) = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual bool					IsTransparent(void) = 0;
	virtual bool					UsesPowerOfTwoFrameBufferTexture() = 0;
	virtual bool					UsesFullFrameBufferTexture() = 0;

	virtual void*	GetShadowHandle() const = 0;

	// Used by the leaf system to store its render handle.
	virtual void* RenderHandle() = 0;

	// Render baby!
	virtual const void* GetModel() const = 0;
	virtual int						DrawModel(int flags) = 0;

	// Get the body parameter
	virtual int		GetBody() = 0;

	// Determine alpha and blend amount for transparent objects based on render state info
	virtual void	ComputeFxBlend() = 0;
	virtual int		GetFxBlend(void) = 0;

	// Determine the color modulation amount
	virtual void	GetColorModulation(float* color) = 0;

	// Returns false if the entity shouldn't be drawn due to LOD. 
	// (NOTE: This is no longer used/supported, but kept in the vtable for backwards compat)
	virtual bool	LODTest() = 0;

	// Call this to get the current bone transforms for the model.
	// currentTime parameter will affect interpolation
	// nMaxBones specifies how many matrices pBoneToWorldOut can hold. (Should be greater than or
	// equal to studiohdr_t::numbones. Use MAXSTUDIOBONES to be safe.)
	virtual bool	SetupBones(CMatrix3x4* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;

	virtual void	SetupWeights(const CMatrix3x4* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void	DoAnimationEvents(void) = 0;

	// Return this if you want PVS notifications. See IPVSNotify for more info.	
	// Note: you must always return the same value from this function. If you don't,
	// undefined things will occur, and they won't be good.
	virtual void* GetPVSNotifyInterface() = 0;

	// Returns the bounds relative to the origin (render bounds)
	virtual void	GetRenderBounds(Vector3& mins, Vector3& maxs) = 0;

};


class CEntity
{
public:
	union

	{
		//              Type     Name    Offset
		DEFINE_MEMBER_N(int, m_iHealth, 0x9c);
		DEFINE_MEMBER_N(int, m_fFlags, 0x430);
		DEFINE_MEMBER_N(Vector3, m_vecOrigin, 0x418);
		DEFINE_MEMBER_N(float, m_flStamina, 0x9CA0);
		DEFINE_MEMBER_N(int, m_iObserverMode, 0x2f58);
		DEFINE_MEMBER_N(int, m_hObserverTarget, 0x2f5c);
		DEFINE_MEMBER_N(bool, m_lifeState, 0x9b);
		DEFINE_MEMBER_N(int, m_nTickBase, 0x3010);
		DEFINE_MEMBER_N(int, m_iTeamNum, 0xa4);
		DEFINE_MEMBER_N(float, m_vecViewOffset2, 0xfc);
		DEFINE_MEMBER_N(Vector3, m_aimPunchAngle, 0x2d40);
		DEFINE_MEMBER_N(float, m_flFlashDuration, 0x9ce4);
		DEFINE_MEMBER_N(float, m_flFlashMaxAlpha, 0x9ce0);
		DEFINE_MEMBER_N(Vector3, m_vecVelocity, 0x100);
		DEFINE_MEMBER_N(bool, m_bDucked, 0x44);
		DEFINE_MEMBER_N(DWORD, m_boneMatrix, 0x664);
		DEFINE_MEMBER_N(bool, m_bIsDefusing, 0x33a0);
		DEFINE_MEMBER_N(bool, m_bIsScoped, 0x3394);
		DEFINE_MEMBER_N(bool, m_bImmunity, 0x33ac);
		DEFINE_MEMBER_N(CMatrix3x4, m_rgflCoordinateFrame, 0x404);
		DEFINE_MEMBER_N(bool, m_bHasDefuser, 0x9d58);
		DEFINE_MEMBER_N(bool, m_bHasHelmet, 0x9d40);
		DEFINE_MEMBER_N(int, m_ArmorValue, 0x9d48);
		DEFINE_MEMBER_N(float, m_flNextAttack, 0x2950);
		DEFINE_MEMBER_N(uintptr_t, m_hActiveWeapon, 0x2BDC);
		DEFINE_MEMBER_N(uintptr_t, m_nRenderMode, 0x88);

	};
	enum move_type {
		movetype_none = 0,
		movetype_isometric,
		movetype_walk,
		movetype_step,
		movetype_fly,
		movetype_flygravity,
		movetype_vphysics,
		movetype_push,
		movetype_noclip,
		movetype_ladder,
		movetype_observer,
		movetype_custom,
		movetype_last = movetype_custom,
		movetype_max_bits = 4,
		max_movetype
	};
	CBaseCombatWeapon* GetActiveWeapon();

	bool IsAlive()
	{
		return !this->m_lifeState;
	}

	Vector3 GetEyePos()
	{
		Vector3 pos = this->m_vecOrigin;
		pos.z += this->m_vecViewOffset2;
		return pos;
	}
	 Vector3 abs_origin()
	{
	}

	Vector3 GetBonePos(int index)
	{
		Vector3 pos{0.f,0.f,0.f};

		DWORD boneBase = (DWORD)this->m_boneMatrix;

		pos.x = *(float*)(boneBase + 0x30 * index + 0x0C);
		pos.y = *(float*)(boneBase + 0x30 * index + 0x1C);
		pos.z = *(float*)(boneBase + 0x30 * index + 0x2C);

		return pos;
	}


	IClientRenderable* GetRenderable()
	{
		return (IClientRenderable*)((DWORD)this + 0x4);
	}

	void* Renderable()
	{
		return (void*)((DWORD)this + 0x4);
	}

	void* Networkable()
	{
		return (void*)((DWORD)this + 0x8);
	}

public:	//renderable

	int DrawModel(int flags)
	{
		auto renderable = Renderable();
		if (!renderable) return false;
		return util::get_vfunc<10, int>(renderable, flags);
	}

	bool SetupBones(CMatrix3x4* out, std::int32_t max, std::int32_t mask, float currTime)
	{
		auto renderable = Renderable();
		if (!renderable) return false;
		return util::get_vfunc<16, bool>(renderable, out, max, mask, currTime);
	}


	//void GetRenderBounds(Vector3& mins, Vector3& maxs)
	//{
	//	auto renderable = Renderable();
	//	if (!renderable) return;
	//	return util::get_vfunc<22, void>(renderable, mins, maxs);
	//}

public:	//networkable
	bool IsDormant()
	{
		auto networkable = Networkable();
		if (!networkable) return false;
		return util::get_vfunc<8, bool>(networkable);
	}

	int GetIndex()
	{
		auto networkable = Networkable();
		if (!networkable) return 0;
		return util::get_vfunc<9, int>(networkable);
	}

	ClientClass* GetClientClass()
	{
		auto networkable = Networkable();
		if (!networkable) return nullptr;
		return util::get_vfunc<2, ClientClass*>(networkable);
	}

	bool IsPlayer()
	{
		return GetClientClass()->classID == 31;
	}

	bool IsInCooldown() const {
		// Add your implementation here
		// For example, check if the entity is currently in cooldown
		return false; // Placeholder return value, replace it with your actual implementation
	}
public: //entity vfuncs
	

	bool IsVisible(int bone);



}; //Size: 0x3070
//class C_BaseEntity
//{
//public:
//
//	static __forceinline C_BaseEntity* GetEntityByIndex(int index) {
//		return static_cast<C_BaseEntity*>(EntityList->GetClientEntity(index));
//	}
//	static __forceinline C_BaseEntity* get_entity_from_handle(CBaseHandle h) {
//		return static_cast<C_BaseEntity*>(EntityList->GetClientEntityFromHandle(h));
//	}
//}

namespace mem {
	inline int tick_count;
}