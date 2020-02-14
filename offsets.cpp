#include <Windows.h>
#include <Psapi.h>
#include "offsets.hpp"
#include "interfaces.hpp"
#include "xor.hpp"

#define client_panoramadllSTR Decrypt({ 0x2a, 0x25, 0x20, 0x2c, 0x27, 0x3d, 0x16, 0x39, 0x28, 0x27, 0x26, 0x3b, 0x28, 0x24, 0x28, 0x67, 0x2d, 0x25, 0x25 }).c_str()
#define enginedllSTR Decrypt({ 0x2c, 0x27, 0x2e, 0x20, 0x27, 0x2c, 0x67, 0x2d, 0x25, 0x25 }).c_str()
#define DT_BasePlayerSTR Decrypt({ 0xd, 0x1d, 0x16, 0xb, 0x28, 0x3a, 0x2c, 0x19, 0x25, 0x28, 0x30, 0x2c, 0x3b }).c_str()
#define DT_CSPlayerSTR Decrypt({ 0xd, 0x1d, 0x16, 0xa, 0x1a, 0x19, 0x25, 0x28, 0x30, 0x2c, 0x3b }).c_str()
#define DT_BaseEntitySTR Decrypt({ 0xd, 0x1d, 0x16, 0xb, 0x28, 0x3a, 0x2c, 0xc, 0x27, 0x3d, 0x20, 0x3d, 0x30 }).c_str()
#define DT_BaseAnimatingSTR Decrypt({ 0xd, 0x1d, 0x16, 0xb, 0x28, 0x3a, 0x2c, 0x8, 0x27, 0x20, 0x24, 0x28, 0x3d, 0x20, 0x27, 0x2e }).c_str()

#define m_iHealthSTR Decrypt({ 0x24, 0x16, 0x20, 0x1, 0x2c, 0x28, 0x25, 0x3d, 0x21 }).c_str()
#define m_iTeamNumSTR Decrypt({ 0x24, 0x16, 0x20, 0x1d, 0x2c, 0x28, 0x24, 0x7, 0x3c, 0x24 }).c_str()
#define m_vecOriginSTR Decrypt({ 0x24, 0x16, 0x3f, 0x2c, 0x2a, 0x6, 0x3b, 0x20, 0x2e, 0x20, 0x27 }).c_str()
#define m_aimPunchAngleSTR Decrypt({ 0x24, 0x16, 0x28, 0x20, 0x24, 0x19, 0x3c, 0x27, 0x2a, 0x21, 0x8, 0x27, 0x2e, 0x25, 0x2c }).c_str()
#define m_viewPunchAngleSTR Decrypt({ 0x24, 0x16, 0x3f, 0x20, 0x2c, 0x3e, 0x19, 0x3c, 0x27, 0x2a, 0x21, 0x8, 0x27, 0x2e, 0x25, 0x2c }).c_str()
#define m_lifeStateSTR Decrypt({ 0x24, 0x16, 0x25, 0x20, 0x2f, 0x2c, 0x1a, 0x3d, 0x28, 0x3d, 0x2c }).c_str()
#define m_vecViewOffsetSTR Decrypt({ 0x24, 0x16, 0x3f, 0x2c, 0x2a, 0x1f, 0x20, 0x2c, 0x3e, 0x6, 0x2f, 0x2f, 0x3a, 0x2c, 0x3d, 0x12, 0x79, 0x14 }).c_str()
#define m_flFlashDurationSTR Decrypt({ 0x24, 0x16, 0x2f, 0x25, 0xf, 0x25, 0x28, 0x3a, 0x21, 0xd, 0x3c, 0x3b, 0x28, 0x3d, 0x20, 0x26, 0x27 }).c_str()
#define m_iShotsFiredSTR Decrypt({ 0x24, 0x16, 0x20, 0x1a, 0x21, 0x26, 0x3d, 0x3a, 0xf, 0x20, 0x3b, 0x2c, 0x2d }).c_str()
#define m_bSpottedSTR Decrypt({ 0x24, 0x16, 0x2b, 0x1a, 0x39, 0x26, 0x3d, 0x3d, 0x2c, 0x2d }).c_str()
#define m_bSpottedByMaskSTR Decrypt({ 0x24, 0x16, 0x2b, 0x1a, 0x39, 0x26, 0x3d, 0x3d, 0x2c, 0x2d, 0xb, 0x30, 0x4, 0x28, 0x3a, 0x22 }).c_str()
#define m_nForceBoneSTR Decrypt({ 0x24, 0x16, 0x27, 0xf, 0x26, 0x3b, 0x2a, 0x2c, 0xb, 0x26, 0x27, 0x2c }).c_str()

MODULEINFO GetModuleInfo(const char* szModule) {
	MODULEINFO modInfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule != 0) {
		GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
	}
	return modInfo;
}

const char* ScanIn(const char* pattern, const char* mask, const char* begin, unsigned int size) {
	unsigned int patternLength = strlen(mask);

	for (unsigned int i = 0; i < size - patternLength; i++) {
		bool found = true;
		for (unsigned int j = 0; j < patternLength; j++) {
			if (mask[j] != '?' && pattern[j] != *(begin + i + j)) {
				found = false;
				break;
			}
		}
		if (found) {
			return (begin + i);
		}
	}
	return nullptr;
}

ptrdiff_t PatternScan(const char* module, const char* pattern, const char* mask, std::vector<int> offsets, unsigned int extra, bool relative) {
	MODULEINFO mInfo = GetModuleInfo(module);
	DWORD base = (DWORD)mInfo.lpBaseOfDll;
	DWORD size = (DWORD)mInfo.SizeOfImage;
	intptr_t result = (intptr_t)ScanIn(pattern, mask, (char*)base, size);
	if (offsets.size() != 0) {
		for (auto o : offsets) {
			result = result + o;
			result = *(intptr_t*)result;
		}
	}
	if (extra > 0) {
		result += extra;
	}
	if (relative) {
		result = result - (uintptr_t)base;
	}
	return result;
}

namespace offsets {
	void Initialize() {
		//Netvars
		//DT_BasePlayer
		m_iHealth = GetNetVarOffset(DT_BasePlayerSTR, m_iHealthSTR, g_BaseClientDLL->GetAllClasses());
		m_iTeamNum = GetNetVarOffset(DT_BasePlayerSTR, m_iTeamNumSTR, g_BaseClientDLL->GetAllClasses());
		m_vecOrigin = GetNetVarOffset(DT_BasePlayerSTR, m_vecOriginSTR, g_BaseClientDLL->GetAllClasses());
		m_aimPunchAngle = GetNetVarOffset(DT_BasePlayerSTR, m_aimPunchAngleSTR, g_BaseClientDLL->GetAllClasses());
		m_viewPunchAngle = GetNetVarOffset(DT_BasePlayerSTR, m_viewPunchAngleSTR, g_BaseClientDLL->GetAllClasses());

		//DT_CSPlayer
		m_lifeState = GetNetVarOffset(DT_CSPlayerSTR, m_lifeStateSTR, g_BaseClientDLL->GetAllClasses());
		m_vecViewOffset = GetNetVarOffset(DT_CSPlayerSTR, m_vecViewOffsetSTR, g_BaseClientDLL->GetAllClasses());
		m_iGlowIndex = GetNetVarOffset(DT_CSPlayerSTR, m_flFlashDurationSTR, g_BaseClientDLL->GetAllClasses()) + 24;
		m_iShotsFired = GetNetVarOffset(DT_CSPlayerSTR, m_iShotsFiredSTR, g_BaseClientDLL->GetAllClasses());

		//DT_BaseEntity
		m_bSpotted = GetNetVarOffset(DT_BaseEntitySTR, m_bSpottedSTR, g_BaseClientDLL->GetAllClasses());
		m_bSpottedByMask = GetNetVarOffset(DT_BaseEntitySTR, m_bSpottedByMaskSTR, g_BaseClientDLL->GetAllClasses());

		//DT_BaseAnimating
		m_dwBoneMatrix = GetNetVarOffset(DT_BaseAnimatingSTR, m_nForceBoneSTR, g_BaseClientDLL->GetAllClasses()) + 28;

		//Signatures
		//client_panorama.dll
		m_bDormant = PatternScan(client_panoramadllSTR, "\x8A\x81\x00\00\x00\x00\xC3\x32\xC0", "xx????xxx", { 2 }, 8, false);
		dwLocalPlayer = PatternScan(client_panoramadllSTR, "\x8D\x34\x85\x00\x00\x00\x00\x89\x15\x00\x00\x00\x00\x8B\x41\x08\x8B\x48\x04\x83\xF9\xFF", "xxx????xx????xxxxxxxxx", { 3 }, 4, true);
		dwGlowObjectManager = PatternScan(client_panoramadllSTR, "\xA1\x00\x00\x00\x00\xA8\x01\x75\x4B", "x????xxxx", { 1 }, 4, true);
		dwEntityList = PatternScan(client_panoramadllSTR, "\xBB\x00\x00\x00\x00\x83\xFF\x01\x0F\x8C\x00\x00\x00\x00\x3B\xF8", "x????xxxxx????xx", { 1 }, 0, true);

		//engine.dll
		dwClientState = PatternScan(enginedllSTR, "\xA1\x00\x00\x00\x00\x33\xD2\x6A\x00\x6A\x00\x33\xC9\x89\xB0", "x????xxxxxxxxxx", { 1 }, 0, true);
		dwClientState_State = PatternScan(enginedllSTR, "\x83\xB8\x00\x00\x00\x00\x00\x0F\x94\xC0\xC3", "xx?????xxxx", { 2 }, 0, false);
		dwClientState_ViewAngles = PatternScan(enginedllSTR, "\xF3\x0F\x11\x80\x00\x00\x00\x00\xD9\x46\x04\xD9\x05", "xxxx????xxxxx", { 4 }, 0, false);
	}
}