#pragma once
#include <cstdint>
#include "../interfaces/interfaces.h"
#include "vector.h"

namespace util
{
	template < size_t index, typename ret, class ... args_ >
	constexpr __forceinline ret get_vfunc(void* thisptr, args_... args) {
		using fn = ret(__thiscall*)(void*, args_...);

		auto fn_ptr = (fn)(*(uintptr_t**)thisptr)[index];
		return fn_ptr(thisptr, args...);
	}
    template<typename t>
    __forceinline static t call_virtual(void* name, int offset) {
        int** inttable = reinterpret_cast<int**>(name); // Corrected the declaration
        int* table = *inttable; // Corrected the variable name
        int address = table[offset];
        return reinterpret_cast<t>(address);
    }

	static std::wstring ansi2unicode(const std::string& str)
	{
		int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}

	inline std::uint8_t* PatternScan(const char* moduleName, const char* pattern)
	{
        static auto patternToByte = [](const char* pattern) noexcept -> const std::vector<std::int32_t>
            {
                std::vector<std::int32_t> bytes = std::vector<std::int32_t>{ };
                char* start = const_cast<char*>(pattern);
                const char* end = const_cast<char*>(pattern) + std::strlen(pattern);

                for (auto current = start; current < end; ++current)
                {
                    if (*current == '?')
                    {
                        ++current;

                        if (*current == '?')
                            ++current;

                        bytes.push_back(-1);
                    }
                    else
                        bytes.push_back(std::strtoul(current, &current, 16));
                }

                return bytes;
            };

        const HINSTANCE handle = GetModuleHandle(moduleName);

        if (!handle)
            return nullptr;

        const PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(handle);
        const PIMAGE_NT_HEADERS ntHeaders =
            reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uint8_t*>(handle) + dosHeader->e_lfanew);

        const std::uintptr_t size = ntHeaders->OptionalHeader.SizeOfImage;
        const std::vector<std::int32_t> bytes = patternToByte(pattern);
        std::uint8_t* scanBytes = reinterpret_cast<std::uint8_t*>(handle);

        const std::size_t s = bytes.size();
        const std::int32_t* d = bytes.data();

        for (std::size_t i = 0ul; i < size - s; ++i)
        {
            bool found = true;

            for (std::size_t j = 0ul; j < s; ++j)
            {
                if (scanBytes[i + j] != d[j] && d[j] != -1)
                {
                    found = false;
                    break;
                }
            }

            if (found)
                return &scanBytes[i];
        }

        return nullptr;
	}

}


