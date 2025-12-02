#include <string>
#include <iostream>
#include <thread>
#include <chrono>
#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include "memory.hpp"
#include "other/vector.hpp"
#include "visual/matrix.h"

namespace memory {
    HANDLE handle = NULL;
    DWORD pid = 0;

    template <typename T>
    T read(uintptr_t address) {
        T value;
        ReadProcessMemory(handle, (LPCVOID)address, &value, sizeof(T), NULL);
        return value;
    }

    template int read<int>(uintptr_t);
    template float read<float>(uintptr_t);
    template uintptr_t read<uintptr_t>(uintptr_t);
    template Vector3 read<Vector3>(uintptr_t);
    template uint32_t read<uint32_t>(uintptr_t);
    template uint8_t read<uint8_t>(uintptr_t);
    template char read<char>(uintptr_t);
	template bool read<bool>(uintptr_t);  // read <bool>
    template Matrix4 read<Matrix4>(uintptr_t);   
    template Vector2 read<Vector2>(uintptr_t);      
    bool find_process(const std::string& process_name) {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        bool found = false;

        if (snapshot == INVALID_HANDLE_VALUE) {
            return false;
        }

        if (Process32First(snapshot, &entry)) {
            while (Process32Next(snapshot, &entry)) {
                if (_stricmp(entry.szExeFile, process_name.c_str()) == 0) {
                    pid = entry.th32ProcessID;
                    found = true;
                    break;
                }
            }
        }

        CloseHandle(snapshot);
        return found;
    }

    bool attach() {
        if (pid == 0) {
            return false;
        }

        handle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);
        return handle != NULL && handle != INVALID_HANDLE_VALUE;
    }

    void detach() {
        if (handle != NULL && handle != INVALID_HANDLE_VALUE) {
            CloseHandle(handle);
        }
        pid = 0;
        handle = NULL;
    }

    bool find_roblox_window() {
        std::cout << "[+] Waiting for Roblox to load fully" << std::endl;

        int attempts = 0;
        const int max_attempts = 10;

        while (attempts < max_attempts) {
            HWND hwnd = FindWindowW(NULL, L"Roblox");
            if (hwnd != NULL) {
                std::cout << "[+] Roblox loaded!" << std::endl; 
                return true;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            attempts++;
        }

        return false;
    }

    std::string read_string(uintptr_t address, int max_len) {
        std::string str;
        str.reserve(max_len);
        for (int i = 0; i < max_len; ++i) {
            char c = read<char>(address + i);
            if (c == 0) {
                break;
            }
            str += c;
        }
        return str;
    }

    uintptr_t get_roblox_base_address() {
        HMODULE hMods[1024];
        DWORD cbNeeded;
        uintptr_t base_address = 0;

        if (EnumProcessModules(handle, hMods, sizeof(hMods), &cbNeeded)) {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                TCHAR szModName[MAX_PATH];
                if (GetModuleFileNameEx(handle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
                    std::string moduleName = szModName;
                    if (moduleName.find("RobloxPlayerBeta.exe") != std::string::npos) {
                        base_address = (uintptr_t)hMods[i];
                        break;
                    }
                }
            }
        }
        return base_address;
    }
}
