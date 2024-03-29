#pragma once
#include <vector>

MODULEINFO GetModuleInfo(const char* szModule);
const char* ScanIn(const char* pattern, const char* mask, const char* begin, unsigned int size);
ptrdiff_t PatternScan(const char* module, const char* pattern, const char* mask, std::vector<int> offsets, unsigned int extra, bool relative);

namespace Offsets {
	void Setup();
	inline uintptr_t dwClient = NULL;
	inline uintptr_t dwEngine = NULL;

	//NetVars
	//DT_BasePlayer
	inline ptrdiff_t m_iHealth = NULL;
	inline ptrdiff_t m_iTeamNum = NULL;
	inline ptrdiff_t m_vecOrigin = NULL;
	inline ptrdiff_t m_aimPunchAngle = NULL;
	inline ptrdiff_t m_viewPunchAngle = NULL;

	//DT_CSPlayer
	inline ptrdiff_t m_lifeState = NULL;
	inline ptrdiff_t m_vecViewOffset = NULL;
	inline ptrdiff_t m_iGlowIndex = NULL;
	inline ptrdiff_t m_iShotsFired = NULL;
	inline ptrdiff_t m_vecVelocity = NULL;
	inline ptrdiff_t m_fFlags = NULL;

	//DT_BaseEntity
	inline ptrdiff_t m_bSpotted = NULL;
	inline ptrdiff_t m_bSpottedByMask = NULL;

	//DT_BaseAnimating
	inline ptrdiff_t m_dwBoneMatrix = NULL;

	//Signatures
	//client_panorama.dll
	inline ptrdiff_t m_bDormant = NULL;
	inline ptrdiff_t dwLocalPlayer = NULL;
	inline ptrdiff_t dwGlowObjectManager = NULL;
	inline ptrdiff_t dwEntityList = NULL;
	inline ptrdiff_t dwGetAllClasses = NULL;
	inline ptrdiff_t LineGoesThroughSmoke = NULL; //Func
	inline ptrdiff_t dwViewMatrix = NULL;

	//engine.dll
	inline ptrdiff_t dwClientState = NULL;
	inline ptrdiff_t dwClientState_State = NULL;
	inline ptrdiff_t dwClientState_ViewAngles = NULL;
	inline ptrdiff_t dwGlobalVars = NULL;
	//inline ptrdiff_t dwGameDir = NULL;
	//inline ptrdiff_t dwClientState_MapDirectory = NULL;
}