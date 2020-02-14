#include <Windows.h>
#include "memHelper.hpp"

uintptr_t mem::FindDMAAddy(std::vector<unsigned int> Offsets, DWORD BaseAddress) {
    uintptr_t pointer = BaseAddress;

    for (unsigned int i = 0; i < Offsets.size(); ++i) {
        pointer = *(uintptr_t*)pointer;
        pointer += Offsets[i];
    }
    return pointer;
}
void mem::Patch(BYTE* dst, BYTE* src, unsigned int size) {
    DWORD oldprotect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldprotect, &oldprotect);
}
void mem::Nop(BYTE* dst, unsigned int size) {
    DWORD oldprotect;
    VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    memset(dst, 0x90, size);
    VirtualProtect(dst, size, oldprotect, &oldprotect);
}