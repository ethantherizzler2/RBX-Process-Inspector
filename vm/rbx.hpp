#pragma once
#include <cstdint>
#include "Roblox/instance.hpp"

namespace rbx {
    extern uintptr_t datamodel;
    extern uintptr_t workspace;
    extern uintptr_t localplayer;
    extern uintptr_t character;
    extern uintptr_t humanoid;

    uintptr_t get_datamodel();
    void update_services();
    void print_system_info();
    void print_datamodel_children();
    void print_player_details();
    void print_camera_info();
    void print_workspace_stats();
	void print_character_stats();
}