#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <string>

namespace memory {
    extern HANDLE handle;
    extern DWORD pid;

    template <typename T>
    T read(uintptr_t address);


    bool attach();
    void detach();
    bool find_process(const std::string& process_name);
    bool find_roblox_window();
    std::string read_string(uintptr_t address, int max_len = 200);
    uintptr_t get_roblox_base_address();
}