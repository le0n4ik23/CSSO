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
#include <Windows.h>
#include <unordered_map>
#include <iostream>
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
inline bool IsKP(int key) {
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

inline bool IsKT(int keyCode) {
	// Check if the key is currently toggled (e.g., Caps Lock, Num Lock, etc.)
	// The high-order bit (0x8000) indicates the key's toggle state.
	return (GetKeyState(keyCode) & 0x0001) != 0;
}
namespace hooks
{
	inline UserCmd* currentcmd;
	void SetupHooks();
	void RemoveHooks();
	void SetupFont(const char* fontName);
	inline bool bSendPacket = true;
	inline int hits = 0;
	inline int predflags = 0;
	inline int screenheight = 0;
	inline int screenwidth = 0;
	inline Vector3 predvelo;
	inline Vector3 realangles;
	inline Vector3 oldangles;
	inline std::vector<CEntity*> playerList;
	inline bool setup;
}

