#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <string>

namespace mem {
	uintptr_t FindDMAAddy(std::vector<unsigned int> Offsets, DWORD BaseAddress);
	void Patch(BYTE* dst, BYTE* src, unsigned int size);
	void Nop(BYTE* dst, unsigned int size);
}