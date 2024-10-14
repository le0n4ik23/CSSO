#pragma once
#include "../util/util.h"

class CStudioHdr;
class IClientRenderable;
class IMaterial;
class IMaterialVar;


enum OverrideType_t {
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SELECTIVE,
	OVERRIDE_SSAO_DEPTH_WRITE,
};


class CDrawModelInfo
{
public:
	CStudioHdr* studioHdr;
	void* hardwareData;
	std::int32_t decals;
	std::int32_t skin;
	std::int32_t body;
	std::int32_t hitboxSet;
	IClientRenderable* renderable;
};


class IStudioRender
{
public:
	enum
	{
		STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
		STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
		STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
		STUDIORENDER_DRAW_GROUP_MASK = 0x03,
		STUDIORENDER_DRAW_NO_FLEXES = 0x04,
		STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,
		STUDIORENDER_DRAW_ACCURATETIME = 0x10,		// Use accurate timing when drawing the model.
		STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
		STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,
		STUDIORENDER_DRAW_WIREFRAME = 0x80,
		STUDIORENDER_DRAW_ITEM_BLINK = 0x100,
		STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,
		STUDIORENDER_UNUSED = 0x400,
		STUDIORENDER_SKIP_DECALS = 0x800,
		STUDIORENDER_MODEL_IS_CACHEABLE = 0x1000,
		STUDIORENDER_SHADOWDEPTHTEXTURE_INCLUDE_TRANSLUCENT_MATERIALS = 0x2000,
		STUDIORENDER_NO_PRIMARY_DRAW = 0x4000,
		STUDIORENDER_SSAODEPTHTEXTURE = 0x8000,
	};

	void SetColorModulation(float const* pColor)
	{
		util::get_vfunc<27, void>(this, pColor);
	}
	void SetAlphaModulation(float flAlpha)
	{
		util::get_vfunc<28, void>(this, flAlpha);
	}

	void ForcedMaterialOverride(IMaterial* newMaterial, std::int32_t nOverrideType = OVERRIDE_NORMAL, std::int32_t nMaterialIndex = -1)
	{
		util::get_vfunc<33, void>(this, newMaterial, nOverrideType);
	}

	bool IsForcedMaterialOverride()
	{
		return util::get_vfunc<34, bool>(this);
	}
};