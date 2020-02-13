#include "radar.hpp"
#include "netvars.hpp"

void Radar(Entity* target) {
    *(int*)((uintptr_t)target + Offsets2::m_bSpotted) = 1;
}