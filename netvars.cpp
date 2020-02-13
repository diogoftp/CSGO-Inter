#include "netvars.hpp"
#include "interfaces.hpp"
#include <vector>

namespace Offsets2 {
	void Initialize() {
		m_iHealth = GetNetVarOffset("DT_BasePlayer", "m_iHealth", g_BaseClientDLL->GetAllClasses());
		m_iTeamNum = GetNetVarOffset("DT_BasePlayer", "m_iTeamNum", g_BaseClientDLL->GetAllClasses());
		m_vecOrigin = GetNetVarOffset("DT_BasePlayer", "m_vecOrigin", g_BaseClientDLL->GetAllClasses());
		

		m_lifeState = GetNetVarOffset("DT_CSPlayer", "m_lifeState", g_BaseClientDLL->GetAllClasses());
		m_vecViewOffset = GetNetVarOffset("DT_CSPlayer", "m_vecViewOffset[0]", g_BaseClientDLL->GetAllClasses());
		m_iGlowIndex = GetNetVarOffset("DT_CSPlayer", "m_flFlashDuration", g_BaseClientDLL->GetAllClasses()) + 24;

        m_bSpotted = GetNetVarOffset("DT_BaseEntity", "m_bSpotted", g_BaseClientDLL->GetAllClasses());

		m_bDormant = (ptrdiff_t)(PatternScan((uintptr_t)GetModuleHandleW(L"client_panorama.dll"), "8D 34 85 ? ? ? ? 89 15 ? ? ? ? 8B 41 08 8B 48 04 83 F9 FF") + 3);
	}
}

std::uint8_t* PatternScan(uintptr_t module, const char* signature) {
    static auto pattern_to_byte = [](const char* pattern) {
        auto bytes = std::vector<int>{};
        auto start = const_cast<char*>(pattern);
        auto end = const_cast<char*>(pattern) + strlen(pattern);

        for (auto current = start; current < end; ++current) {
            if (*current == '?') {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else {
                bytes.push_back(strtoul(current, &current, 16));
            }
        }
        return bytes;
    };

    auto dosHeader = (PIMAGE_DOS_HEADER)module;
    auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

    auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
    auto patternBytes = pattern_to_byte(signature);
    auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

    auto s = patternBytes.size();
    auto d = patternBytes.data();

    for (auto i = 0ul; i < sizeOfImage - s; ++i) {
        bool found = true;
        for (auto j = 0ul; j < s; ++j) {
            if (scanBytes[i + j] != d[j] && d[j] != -1) {
                found = false;
                break;
            }
        }
        if (found) {
            return &scanBytes[i];
        }
    }
    return nullptr;
}