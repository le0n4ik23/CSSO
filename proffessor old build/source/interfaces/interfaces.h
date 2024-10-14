#pragma once
#include "../classes/CEntity.h"
#include "../datatable.h"
#include "../util/util.h"
#include "IVEngineClient.h"
#include "IBaseClientDll.h"
#include "IClientEntityList.h"
#include "ISurface.h"
#include "IVDebugOverlay.h"
#include "IVModelRender.h"
#include "IStudioRender.h"
#include "IMaterialSystem.h"
#include "IVModelInfo.h"
#include "IGlobalVars.h"
#include "IEngineClientReplay.h"
#include "IVRenderView.h"
#include "../classes/CBaseCombatWeapon.h"
#include "../classes/CBaseHandle.h"
#include "../util/enums.h"
#include "../classes/CC4.h"
#include "../classes/CPlantedC4.h"
#include "../classes/CCSPlayerResource.h"
#include "../globals.h"
#include "gameevent.h"


//class IPanel
//{
//public:
//    const char* GetName(int vguiPanel)
//    {
//        return util::get_vfunc<36, const char*>(this, vguiPanel);
//    }
//};

enum TraceType_t
{
    TRACE_EVERYTHING = 0,
    TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
    TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
    TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};


class ITraceFilter
{
public:
    virtual bool ShouldHitEntity(CEntity* pEntity, int contentsMask) = 0;
    virtual TraceType_t	GetTraceType() const = 0;
};


#define  Assert( _exp ) ((void)0)
class __declspec(align(16))VectorAligned : public Vector3
{
public:
    VectorAligned& operator=(const Vector3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }
    float w;
};

struct Ray_t
{
    VectorAligned  m_Start;	// starting point, centered within the extents
    VectorAligned  m_Delta;	// direction + length of the ray
    VectorAligned  m_StartOffset;	// Add this to m_Start to get the actual ray start
    VectorAligned  m_Extents;	// Describes an axis aligned box extruded along a ray

    bool	m_IsRay;	// are the extents zero?
    bool	m_IsSwept;	// is delta != 0?

    void Init(Vector3 const& start, Vector3 const& end)
    {
        m_Delta = end - start;

        m_IsSwept = (m_Delta.LengthSqr() != 0);

        m_Extents.x = m_Extents.y = m_Extents.z = 0.f;

        m_IsRay = true;

        // Offset m_Start to be in the center of the box...
        m_StartOffset.x = m_StartOffset.y = m_StartOffset.z = 0.0f;
        m_Start = start;
    }

private:
};


class CRay
{
public:
    constexpr CRay(const Vector3& src, const Vector3& dst) noexcept
    {
        start = src;
        delta = dst - src;
        isSwept = delta.x || delta.y || delta.z;
    }

private:
    Vector3 start;
    char pad0[4]{ };
    Vector3 delta;
    char pad1[4]{};
    Vector3 startOffset;
    char pad2[4]{};
    Vector3 Extents;
    char pad3[12]{ };
    bool isRay{ true };
    bool isSwept;
};

class CTraceFilter
{
public:
    constexpr CTraceFilter(const CEntity* entity) noexcept
    {
        skip = entity;
    }

    virtual bool ShouldHit(CEntity* entity, std::int32_t mask)
    {
        return entity != skip;
    }

    virtual std::int32_t GetTraceType() const
    {
        return 0;
    }

    const CEntity* skip;
};

struct trace_t
{
    Vector3 start;
    Vector3 end;
    char pad[20];
    float fraction;
    int contents;
    unsigned short dispFlags;
    bool allSolid;
    bool startSolid;
    char pad1[4];
    struct Surface_t {
        const char* name;
        short surfaceProps;
        unsigned short flags;
    } surface;
    int hitgroup;
    char pad2[4];
    CEntity* entity;
    int hitbox;
};



class ITexture;

class IEngineTrace
{
public:
    virtual int   GetPointContents(const Vector3& vecAbsPosition, int contentsMask = 0, void** ppEntity = nullptr) = 0;
    virtual int   GetPointContents_WorldOnly(const Vector3& vecAbsPosition, int contentsMask = 0) = 0;
    virtual int   GetPointContents_Collideable(void* pCollide, const Vector3& vecAbsPosition) = 0;
    virtual void  ClipRayToEntity(const Ray_t& ray, unsigned int fMask, CEntity* pEnt, trace_t* pTrace) = 0;
    virtual void  ClipRayToCollideable(const Ray_t& ray, unsigned int fMask, void* pCollide, trace_t* pTrace) = 0;
    virtual void  TraceRay(const Ray_t& ray, unsigned int fMask, CTraceFilter *pTraceFilter, trace_t* pTrace) = 0;
};

void SetupInterfaces();


inline IClientEntityList* entityList = nullptr;
inline IBaseClientDll* client = nullptr;
inline IVDebugOverlay* debugOverlay = nullptr;
inline IPanel* vguiPanel = nullptr;
inline vgui::ISurface* surface = nullptr;
inline IVEngineClient* engine = nullptr;
inline IEngineTrace* engineTrace = nullptr;
inline IVModelRender* modelRender = nullptr;
inline IGameEventManager2* g_GameEvents = nullptr;
inline IStudioRender* studioRender = nullptr;
inline IMaterialSystem* matSystem = nullptr;
inline IVModelInfo* modelInfo = nullptr;
inline IGlobalVars* globalVars = nullptr;
inline IEngineClientReplay* engineClientReplay = nullptr;
inline IVRenderView* renderView = nullptr;
inline void* g_ClientMode = nullptr;
inline IVDebugOverlay* g_DebugOverlay = nullptr;
inline IGameEvent* gameevent = nullptr;



