#include "radar.hpp"
#include "offsets.hpp"

void Radar(Entity* target) {
    *(int*)((uintptr_t)target + offsets::m_bSpotted) = 1;
}