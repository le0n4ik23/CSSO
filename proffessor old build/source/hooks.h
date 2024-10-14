#pragma once
#include "../source/util/usercmd.h"
#include "../source/interfaces/interfaces.h"
#include "../ext/minhook/MinHook.h"
#include "../source/features/misc.h"
#include <string_view>
#include "stringapiset.h"
#include <codecvt>
#include "../source/features/aimbot.h"
#include "../source/features/esp.h"
#include "../source/util/vector.h"
#include "../source/gui.h"
#include "vhook.hpp"


using CreateMove = bool(__thiscall*)(void*, float, UserCmd*);
inline CreateMove CreateMoveOriginal = nullptr;

using PaintTraverse = void(__thiscall*)(IPanel*, uintptr_t, bool, bool);
inline PaintTraverse PaintTraverseOriginal = nullptr;

using DrawModel = void(__thiscall*)(void*, void*, const CDrawModelInfo&, CMatrix3x4*, float*, float*, const Vector3&, const std::int32_t);
inline DrawModel DrawModelOriginal = nullptr;

using SceneEnd = void(__thiscall*)(void*);
inline SceneEnd SceneEndOriginal = nullptr;

using LevelInitPostEntity = void(__thiscall*)(void*);
inline LevelInitPostEntity LevelInitPostEntityOriginal = nullptr;

using EndSceneFn = long(__thiscall*)(void*, IDirect3DDevice9*) noexcept;
inline EndSceneFn EndSceneOriginal = nullptr;

using ResetFn = HRESULT(__thiscall*)(void*, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
inline ResetFn ResetOriginal = nullptr;

using LockCursorFn = void(__thiscall*)(void*);
inline LockCursorFn LockCursorOriginal = nullptr;

namespace hooks
{
	void SetupHooks();
	void RemoveHooks();
	//inline vfunc_hook gameevents_hook;
	//using FireEvent = bool(__thiscall*)(IGameEventManager2*, IGameEvent* pEvent);
}
namespace index
{
	constexpr auto EmitSound1 = 5;
	constexpr auto EmitSound2 = 6;
	constexpr auto EndScene = 42;
	constexpr auto Reset = 16;
	constexpr auto PaintTraverse = 41;
	constexpr auto CreateMove = 24;
	constexpr auto DrawModelExecute2 = 29;
	constexpr auto PlaySound = 82;
	constexpr auto FrameStageNotify = 37;
	constexpr auto DrawModelExecute = 21;
	constexpr auto DoPostScreenSpaceEffects = 44;
	constexpr auto SvCheatsGetBool = 13;
	constexpr auto OverrideView = 18;
	constexpr auto LockCursor = 67;
	constexpr auto RenderSmokeOverlay = 41;
	constexpr auto FireEvent = 9;

}
