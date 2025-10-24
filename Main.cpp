#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <algorithm> 
#include "offsets.hpp"
// Credits to @SpeedNextdoor
// Credits @ethantherizzler_1 converted from c# to c++ we can paste all day

struct Vector3 {
    float X, Y, Z;

    constexpr Vector3() : X(0.0f), Y(0.0f), Z(0.0f) {}
    constexpr Vector3(float x, float y, float z) : X(x), Y(y), Z(z) {}

    float Length() const {
        return std::sqrt(X * X + Y * Y + Z * Z);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        return os << "(" << v.X << ", " << v.Y << ", " << v.Z << ")";
    }
};

HANDLE global_handle = NULL;
DWORD global_pid = 0;

namespace memory {
    template <typename T>
    T read(uintptr_t address) {
        T value;
        ReadProcessMemory(global_handle, (LPCVOID)address, &value, sizeof(T), NULL);
        return value;
    }

    bool attach() {
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);

        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

        if (Process32First(snapshot, &entry) == TRUE) {
            while (Process32Next(snapshot, &entry) == TRUE) {
                if (_stricmp(entry.szExeFile, "RobloxPlayerBeta.exe") == 0) {
                    global_pid = entry.th32ProcessID;
                    break;
                }
            }
        }
        CloseHandle(snapshot);

        if (global_pid == 0) {
            return false;
        }

        global_handle = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, global_pid);
        return global_handle != NULL;
    }

    void detach() {
        if (global_handle != NULL) {
            CloseHandle(global_handle);
        }
        global_pid = 0;
        global_handle = NULL;
    }

    std::string read_string(uintptr_t address, int max_len = 200) {
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

        if (EnumProcessModules(global_handle, hMods, sizeof(hMods), &cbNeeded)) {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                TCHAR szModName[MAX_PATH];
                if (GetModuleFileNameEx(global_handle, hMods[i], szModName, sizeof(szModName) / sizeof(TCHAR))) {
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

namespace rbx_instance {
    class Instance {
    public:
        uintptr_t address;

        Instance(uintptr_t addr) : address(addr) {}

        std::string name() const {
            uintptr_t ptr = memory::read<uintptr_t>(address + offsets::Name);
            if (ptr != 0) {
                return memory::read_string(ptr);
            }
            return "";
        }

        std::string class_name() const {
            uintptr_t class_descriptor = memory::read<uintptr_t>(address + offsets::ClassDescriptor);
            if (class_descriptor != 0) {
                uintptr_t class_name_ptr = memory::read<uintptr_t>(class_descriptor + offsets::ClassDescriptorToClassName);
                if (class_name_ptr != 0) {
                    return memory::read_string(class_name_ptr);
                }
            }
            return "";
        }

        Instance parent() const {
            return Instance(memory::read<uintptr_t>(address + offsets::Parent));
        }

        std::vector<Instance> get_children() const {
            std::vector<Instance> children;
            uintptr_t start = memory::read<uintptr_t>(address + offsets::Children);
            uintptr_t end = memory::read<uintptr_t>(start + offsets::ChildrenEnd);

            for (uintptr_t ptr = memory::read<uintptr_t>(start); ptr != end; ptr += 0x10) {
                uintptr_t child_addr = memory::read<uintptr_t>(ptr);
                if (child_addr != 0) {
                    children.push_back(Instance(child_addr));
                }
            }
            return children;
        }

        Instance find_first_child(const std::string& name_to_find) const {
            for (const auto& child : get_children()) {
                if (child.name() == name_to_find) {
                    return child;
                }
            }
            return Instance(0);
        }

        Vector3 getPosition() const {
            return memory::read<Vector3>(address + offsets::Position);
        }
    };
}

namespace rbx {
    uintptr_t datamodel = 0;
    uintptr_t workspace = 0;
    uintptr_t localplayer = 0;
    uintptr_t character = 0;
    uintptr_t humanoid = 0;

    uintptr_t get_datamodel() {
        uintptr_t fake_dm = memory::read<uintptr_t>(memory::get_roblox_base_address() + offsets::FakeDataModelPointer);
        return memory::read<uintptr_t>(fake_dm + offsets::FakeDataModelToDataModel);
    }

    void update_services() {
        datamodel = get_datamodel();
        rbx_instance::Instance dm(datamodel);
        workspace = dm.find_first_child("Workspace").address;
        rbx_instance::Instance players = dm.find_first_child("Players");
        localplayer = memory::read<uintptr_t>(players.address + offsets::LocalPlayer);
        rbx_instance::Instance plr_instance(localplayer);
        character = rbx_instance::Instance(workspace).find_first_child(plr_instance.name()).address;
        if (character != 0) {
            humanoid = rbx_instance::Instance(character).find_first_child("Humanoid").address;
        }
    }

    void print_system_info() {
        std::cout << "\n[+] Current attached memory status\n";
        std::cout << "Roblox PID: " << global_pid << "\n";
        std::cout << "Base Address: 0x" << std::hex << memory::get_roblox_base_address() << std::dec << "\n";
        std::cout << "DataModel: 0x" << std::hex << datamodel << std::dec << "\n";
        std::cout << "Workspace: 0x" << std::hex << workspace << std::dec << "\n";
        std::cout << "LocalPlayer: 0x" << std::hex << localplayer << std::dec << "\n";
        std::cout << "Character: 0x" << std::hex << character << std::dec << "\n";
        std::cout << "Humanoid: 0x" << std::hex << humanoid << std::dec << "\n";
    }

    void print_datamodel_children() {
        rbx_instance::Instance dm(datamodel);
        std::cout << "\n[+] DataModel Children\n";
        for (auto child : dm.get_children()) {
            std::cout << "[" << child.class_name() << "] " << child.name()
                << " (0x" << std::hex << child.address << std::dec << ")\n";
        }
    }

    void print_player_details() {
        rbx_instance::Instance players = rbx_instance::Instance(datamodel).find_first_child("Players");
        rbx_instance::Instance localPlayerInst(localplayer);

        std::cout << "\n{+] Player Details\n";
        for (auto player : players.get_children()) {
            std::cout << "Player: " << player.name();
            if (player.name() == localPlayerInst.name()) {
                std::cout << " (Local Player)";
            }
            std::cout << "\n  Class: " << player.class_name();
            std::cout << "\n  Address: 0x" << std::hex << player.address << std::dec << "\n";

            rbx_instance::Instance charInst = rbx_instance::Instance(workspace).find_first_child(player.name());
            if (charInst.address != 0) {
                std::cout << "  Character: 0x" << std::hex << charInst.address << std::dec;
                Vector3 pos = charInst.getPosition();
                std::cout << " Position: " << pos << "\n";
            }
            std::cout << "---\n";
        }
    }

    void print_camera_info() {
        uintptr_t camera_ptr = memory::read<uintptr_t>(workspace + offsets::Camera);
        if (camera_ptr) {
            std::cout << "\n=== Camera Information ===\n";
            std::cout << "Camera Address: 0x" << std::hex << camera_ptr << std::dec << "\n";
            float fov = memory::read<float>(camera_ptr + offsets::FOV);
            Vector3 pos = memory::read<Vector3>(camera_ptr + offsets::CameraPos);
            std::cout << "FOV: " << fov << "\n";
            std::cout << "Position: " << pos << "\n";
        }
    }

    void print_workspace_stats() {
        rbx_instance::Instance ws(workspace);
        std::cout << "\n[+] Workspace status\n";
        std::cout << "Workspace Address: 0x" << std::hex << workspace << std::dec << "\n";

        int childCount = 0;
        for (auto child : ws.get_children()) {
            childCount++;
        }
        std::cout << "Total Children: " << childCount << "\n";

        std::cout << "Sample Children:\n";
        int count = 0;
        for (auto child : ws.get_children()) {
            if (count++ >= 10) break;
            std::cout << "  [" << child.class_name() << "] " << child.name() << "\n";
        }
    }
}

bool is_in_game() {
    rbx::update_services();
    if (rbx::localplayer == 0 || rbx::character == 0 || rbx::humanoid == 0)
        return false;
    float health = memory::read<float>(rbx::humanoid + offsets::Health);
    return health > 0.f;
}

int main() {
    if (!memory::attach()) {
        std::cout << "Failed to attach to Roblox\n";
        return 1;
    }

    std::cout << "Roblox Memory Analyzer - Attached\n";

    while (true) {
        rbx::update_services();
        std::string gameState = is_in_game() ? "In Game" : "In Menu";
        SetConsoleTitle(("Roblox Memory Analyzer - " + gameState).c_str());

        std::cout << "\n[+] Roblox Memory\n";
        std::cout << "[1] Roblox info\n";
        std::cout << "[2] DataModel info\n";
        std::cout << "[3] Player info\n";
        std::cout << "[4] Camera info\n";
        std::cout << "[5] Workspace stats\n";
        std::cout << "[6] Refresh Data\n";
        std::cout << "[7] Exit\n";
        std::cout << "Option<: ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "1") {
            rbx::print_system_info();
        }
        else if (input == "2") {
            rbx::print_datamodel_children();
        }
        else if (input == "3") {
            rbx::print_player_details();
        }
        else if (input == "4") {
            rbx::print_camera_info();
        }
        else if (input == "5") {
            rbx::print_workspace_stats();
        }
        else if (input == "6") {
            rbx::update_services();
            std::cout << "Data refreshed\n";
        }
        else if (input == "7") {
            memory::detach();
            std::cout << "Detached from Roblox process.\n";
            break;
        }
        else {
            std::cout << "Invalid option.\n";
        }
    }

    return 0;

}
