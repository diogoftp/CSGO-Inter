#pragma once
//#include "interfaces.hpp"
#include <cstdint>

namespace Offsets2 {
	void Initialize();
	inline ptrdiff_t m_iHealth = NULL;
	inline ptrdiff_t m_iTeamNum = NULL;
	inline ptrdiff_t m_vecOrigin = NULL;

	inline ptrdiff_t m_lifeState = NULL;
	inline ptrdiff_t m_vecViewOffset = NULL;
	inline ptrdiff_t m_iGlowIndex = NULL;

	inline ptrdiff_t m_bDormant = NULL;
	inline ptrdiff_t dwClientState_State = NULL;
	inline ptrdiff_t dwClientState_ViewAngles = NULL;
	//inline ptrdiff_t dwClientState_State = NULL;
	//inline ptrdiff_t dwClientState_State = NULL;

	inline ptrdiff_t m_bSpotted = NULL;
}

std::uint8_t* PatternScan(uintptr_t module, const char* signature);