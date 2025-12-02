#include "rbx.hpp"
#include <iostream>
#include <iomanip>

namespace rbx {
    uintptr_t datamodel = 0;
    uintptr_t workspace = 0;
    uintptr_t players = 0;
    uintptr_t localplayer = 0;
    uintptr_t character = 0;
    uintptr_t humanoid = 0;
    uintptr_t camera = 0;
    uintptr_t lighting = 0;
    uintptr_t run_service = 0;

    uintptr_t get_datamodel() {
        uintptr_t base = memory::get_roblox_base_address();
        uintptr_t fake_dm = memory::read<uintptr_t>(base + offsets::FakeDataModelPointer);
        if (!fake_dm) return 0;
        return memory::read<uintptr_t>(fake_dm + offsets::FakeDataModelToDataModel);
    }

    void update_services() {
    datamodel = get_datamodel();
    if (!datamodel) return;

    rbx_instance::Instance dm(datamodel);

    // Core services
    workspace = dm.find_first_child("Workspace").address;
    players = dm.find_first_child("Players").address;
    camera = dm.find_first_child("Camera").address;
    lighting = dm.find_first_child("Lighting").address;
    run_service = dm.find_first_child("Run Service").address;

    if (!players) return;

    localplayer = memory::read<uintptr_t>(players + offsets::LocalPlayer);
    if (!localplayer) return;

    rbx_instance::Instance plr(localplayer);
    if (!plr.address) return;

    std::string plr_name = plr.name();
    if (workspace && !plr_name.empty()) {
        character = rbx_instance::Instance(workspace).find_first_child(plr_name).address;
        if (character)
            humanoid = rbx_instance::Instance(character).find_first_child("Humanoid").address;
    }
}

bool valid() {
    return datamodel && workspace && players && localplayer && character && humanoid;
}
    void print_system_info() {
        std::cout << "\n[+] Current attached memory status\n";
        std::cout << "Roblox PID: " << memory::pid << "\n";
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
            std::cout << "\n[+] Camera option\n";
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

    void print_character_stats() {
        if (!character || !humanoid) {
            std::cout << "[-] Character not found!\n";
            return;
        }

        rbx_instance::Instance charInst(character);
        rbx_instance::Instance humanoidInst(humanoid);

        std::cout << "\n[+] Character Stats:\n";
        std::cout << "Health: " << humanoidInst.getHealth() << "/" << humanoidInst.getMaxHealth() << "\n";
        std::cout << "Walk Speed: " << humanoidInst.getWalkSpeed() << "\n";
        std::cout << "Jump Power: " << humanoidInst.getJumpPower() << "\n";
        std::cout << "Position: " << charInst.getPosition() << "\n";
        std::cout << "Velocity: " << charInst.getVelocity() << "\n";
    }

}


