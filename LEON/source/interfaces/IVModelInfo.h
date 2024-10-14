#pragma once
#include "../util/util.h"


enum EHitboxIndex : int
{
	HITBOX_INVALID = -1,
	HITBOX_HEAD,
	HITBOX_NECK,
	HITBOX_PELVIS,
	HITBOX_STOMACH,
	HITBOX_THORAX,
	HITBOX_CHEST,
	HITBOX_UPPER_CHEST,
	HITBOX_RIGHT_THIGH,
	HITBOX_LEFT_THIGH,
	HITBOX_RIGHT_CALF,
	HITBOX_LEFT_CALF,
	HITBOX_RIGHT_FOOT,
	HITBOX_LEFT_FOOT,
	HITBOX_RIGHT_HAND,
	HITBOX_LEFT_HAND,
	HITBOX_RIGHT_UPPER_ARM,
	HITBOX_RIGHT_FOREARM,
	HITBOX_LEFT_UPPER_ARM,
	HITBOX_LEFT_FOREARM,
	HITBOX_MAX
};

class CStudioBone
{
public:

};

class CStudioBoundingBox
{
public:
	int bone;
	int group;
	Vector3 bbMin;
	Vector3 bbMax;
	int hitboxNameIndex;
	Vector3 offsetRotation;
	float capsuleRadius;
	int unused[4];
};

class CStudioHitboxSet
{
public:
	int nameIndex;
	int numHitboxes;
	int hitboxIndex;


	inline const char* GetName() const noexcept
	{
		return reinterpret_cast<char*>(std::uintptr_t(this) + nameIndex);
	}

	inline CStudioBoundingBox* GetHitbox(int index) noexcept
	{
		if (index < 0 || index >= numHitboxes)
			return nullptr;

		return reinterpret_cast<CStudioBoundingBox*>(reinterpret_cast<std::uint8_t*>(this) + hitboxIndex) + index;
	}
};

class CStudioHdr
{
public:
	inline CStudioBone* GetBone(int index) noexcept
	{
		if (index < 0 || index >= numBones)
			return nullptr;

		return reinterpret_cast<CStudioBone*>(uintptr_t(this) + boneIndex) + index;
	}

	inline CStudioHitboxSet* GetHitboxSet(int index) noexcept
	{
		if (index < 0 || index >= numHitboxSets)
			return nullptr;

		return reinterpret_cast<CStudioHitboxSet*>(uintptr_t(this) + hitboxSetIndex) + index;
	}

	int id;
	int version;
	int checksum;
	char name[64];
	int length;

	Vector3 eyePosition;
	Vector3 illumPosition;
	Vector3 hullMin;
	Vector3 hullMax;
	Vector3 bbMin;
	Vector3 bbMax;

	int flags;
	int numBones;
	int boneIndex;
	int numBoneControllers;
	int boneControllerIndex;
	int numHitboxSets;
	int hitboxSetIndex;
};

class CModel
{
public:
	int handle;
	char name[260];
	int loadFlags;
	int serverCount;
	int type;
	int flags;
	Vector3 mins;
	Vector3 maxs;
	float radius;
};

class IVModelInfo
{
public:
	enum ERenderFlags : int
	{
		RENDER_FLAGS_DISABLE_RENDERING = 0x01,
		RENDER_FLAGS_HASCHANGED = 0x02,
		RENDER_FLAGS_ALTERNATE_SORTING = 0x04,
		RENDER_FLAGS_RENDER_WITH_VIEWMODELS = 0x08,
		RENDER_FLAGS_BLOAT_BOUNDS = 0x10,
		RENDER_FLAGS_BOUNDS_VALID = 0x20,
		RENDER_FLAGS_BOUNDS_ALWAYS_RECOMPUTE = 0x40,
		RENDER_FLAGS_IS_SPRITE = 0x80,
		RENDER_FLAGS_FORCE_OPAQUE_PASS = 0x100,
	};

	const char* GetModelName(const CModel* model) noexcept
	{
		return util::get_vfunc<3, const char*>(this, model);
	}

	CStudioHdr* GetStudioModel(const CModel* model) noexcept
	{
		return util::get_vfunc<32, CStudioHdr*>(this, model);
	}
};