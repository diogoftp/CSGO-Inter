#include "radar.hpp"

void Radar(Entity* target) {
    *(int*)((uintptr_t)target + offsets::netvars::m_bSpotted) = 1;
}