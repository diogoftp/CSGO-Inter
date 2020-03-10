#include "ESP.hpp"
#include "offsets.hpp"

void ESP(Entity* localPlayer, Entity* target, uintptr_t dwClient, uintptr_t GlowObjectManager) {
    int my_team = 0;
    int ent_team = 0;

    if (localPlayer != NULL && GlowObjectManager != NULL) {
        //my_team = localPlayer->team;
        my_team = *(int*)((uintptr_t)localPlayer + Offsets::m_iTeamNum);
        int glowIndex = *(int*)((uintptr_t)target + Offsets::m_iGlowIndex);
        //ent_team = target->team;
        ent_team = *(int*)((uintptr_t)target + Offsets::m_iTeamNum);
        if (ent_team == my_team) {
            *(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x4)) = 0.f;
            *(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x8)) = 0.f;
            *(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0xc)) = 1.f;
            *(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x10)) = 1.7f;
        }
        else {
            *(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x4)) = 1.f;
            *(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x8)) = 0.f;
            *(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0xc)) = 0.f;
            *(float*)((GlowObjectManager)+((glowIndex * 0x38) + 0x10)) = 1.7f;
        }
        *(bool*)((GlowObjectManager)+((glowIndex * 0x38) + 0x24)) = true;
        *(bool*)((GlowObjectManager)+((glowIndex * 0x38) + 0x25)) = false;
    }
}