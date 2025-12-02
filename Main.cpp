#include <iostream>
#include <string>
#include "memory/memory.hpp"
#include "vm/rbx.hpp"
#include "other/notifications.hpp"
#include <thread>

bool esp_running = false; 
std::thread esp_thread;   

bool is_in_game() {
    rbx::update_services();
    if (rbx::localplayer == 0 || rbx::character == 0 || rbx::humanoid == 0) {
        return false;
    }
    try {
        memory::read<uint32_t>(rbx::humanoid);
        return true;
    }
    catch (...) {
        return false;
    }
}

bool wait_for_roblox_and_attach() {
    int attempts = 0;
    const int max_attempts = 30;

    while (attempts < max_attempts) {
        if (memory::find_process("RobloxPlayerBeta.exe")) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
        attempts++;
    }

    if (!memory::find_roblox_window()) {
        return false;
    }

    if (!memory::attach()) {
        notifications::show(L"Roblox", L"Failed to attach to Roblox");
        return false;
    }

    notifications::show(L"Roblox", L"attached to Roblox");
    return true;
}

int main() {
	std::cout << "[+] waiting for Roblox\n";
	wait_for_roblox_and_attach();
    bool lastStateInGame = false;
    auto lastCheckTime = std::chrono::steady_clock::now();

    while (true) {
        auto currentTime = std::chrono::steady_clock::now();
        auto timeSinceLastCheck = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastCheckTime);
        if (timeSinceLastCheck.count() >= 2) {
            bool currentInGame = is_in_game();

            if (currentInGame != lastStateInGame) {
                if (currentInGame) {
                    notifications::show(L"Roblox", L"Modules loaded - In Game");
                }
                else {
                    notifications::show(L"Roblox", L"Modules unloaded - In Menu");
                }
                lastStateInGame = currentInGame;
            }

            lastCheckTime = currentTime;
        }

        rbx::update_services();
        std::string gameState = lastStateInGame ? "In Game" : "In Menu";
        SetConsoleTitle(("LPA - RBX Inspector | " + gameState).c_str());
        // end
        std::cout << "\n[+] Roblox Memory\n";
        std::cout << "[1] Roblox info\n";
        std::cout << "[2] DataModel info\n";
        std::cout << "[3] Player info\n";
        std::cout << "[4] Camera info\n";
        std::cout << "[5] Workspace stats\n";
        std::cout << "[6] player stats\n";
        std::cout << "[7] Start Esp\n";
        std::cout << "[8] Load Module\n";
        std::cout << "<: ";

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
            rbx::print_character_stats();
        }
        else if (input == "") {
            rbx::update_services();
            std::cout << "Module Loaded\n";
        }
        else if (input == "P") {
            memory::detach();
            std::cout << "Detached from Roblox\n";
            break;
        }
        else {
            std::cout << "Invalid option.\n";
        }
    }
    return 0;
}
