#include "ESP.hpp"
#include "offsets.hpp"

void ESP(Entity* localPlayer, Entity* target, uintptr_t GlowObjectManager) {
    int glowIndex = target->glowIndex();
    if (localPlayer->team() == target->team()) {
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