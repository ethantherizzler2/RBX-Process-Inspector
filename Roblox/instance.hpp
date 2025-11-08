#pragma once
#include <vector>
#include <string>
#include "other/vector.hpp"
#include "other/offsets.hpp"
#include "memory/memory.hpp"

namespace rbx_instance {
    class Instance {
    public:
        uintptr_t address;

        Instance(uintptr_t addr = 0) : address(addr) {}

        std::string name() const;
        std::string class_name() const;
        Instance parent() const;
        std::vector<Instance> get_children() const;
        Instance find_first_child(const std::string& name_to_find) const;
        Vector3 getPosition() const;

		// extra methods
        Instance find_first_child_of_class(const std::string& class_name) const;
        std::vector<Instance> get_children_of_class(const std::string& class_name) const;
        Instance find_descendant(const std::string& name_to_find) const;
        Vector3 getVelocity() const;
        float getHealth() const;
        float getMaxHealth() const;
        float getWalkSpeed() const;
        float getJumpPower() const;
        bool getAnchored() const;
        bool getCanCollide() const;
        float getTransparency() const;
        Instance getTeam() const;
        std::string getDisplayName() const;
        std::string getValue() const;
        bool is_a(const std::string& class_name) const;
        std::string get_full_path() const; // end
        // replicated storage acces
        static Instance get_replicated_storage();
        static Instance get_players();
        static Instance get_lighting();
        static Instance get_sound_service();
        static Instance get_workspace();
        static Instance get_core_gui();
        static Instance get_service(const std::string& service_name);

        operator bool() const { return address != 0; }
    };
}