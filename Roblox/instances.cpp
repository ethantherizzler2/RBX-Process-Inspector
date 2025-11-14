#include "Roblox/instance.hpp"
#include "vm/rbx.hpp"

namespace rbx_instance {

    std::string Instance::name() const {
        if (!address) return "";
        uintptr_t ptr = memory::read<uintptr_t>(address + offsets::Name);
        return (ptr != 0) ? memory::read_string(ptr) : "";
    }

    std::string Instance::class_name() const {
        if (!address) return "";

        uintptr_t class_descriptor = memory::read<uintptr_t>(address + offsets::ClassDescriptor);
        if (class_descriptor == 0) return "";

        uintptr_t class_name_ptr = memory::read<uintptr_t>(class_descriptor + offsets::ClassDescriptorToClassName);
        return (class_name_ptr != 0) ? memory::read_string(class_name_ptr) : "";
    }

    Instance Instance::parent() const {
        return address ? Instance(memory::read<uintptr_t>(address + offsets::Parent)) : Instance(0);
    }

    std::vector<Instance> Instance::get_children() const {
        std::vector<Instance> children;
        if (!address) return children;

        uintptr_t start = memory::read<uintptr_t>(address + offsets::Children);
        uintptr_t end = memory::read<uintptr_t>(start + offsets::ChildrenEnd);

        for (uintptr_t ptr = memory::read<uintptr_t>(start); ptr != end; ptr += 0x10) {
            uintptr_t child_addr = memory::read<uintptr_t>(ptr);
            if (child_addr != 0) {
                children.emplace_back(child_addr);
            }
        }
        return children;
    }

    Instance Instance::find_first_child(const std::string& name_to_find) const {
        if (!address) return Instance(0);

        for (const auto& child : get_children()) {
            if (child.name() == name_to_find) {
                return child;
            }
        }
        return Instance(0);
    }

    Instance Instance::find_first_child_of_class(const std::string& class_name) const {
        if (!address) return Instance(0);

        for (const auto& child : get_children()) {
            if (child.class_name() == class_name) {
                return child;
            }
        }
        return Instance(0);
    }

    std::vector<Instance> Instance::get_children_of_class(const std::string& class_name) const {
        std::vector<Instance> result;
        if (!address) return result;

        for (const auto& child : get_children()) {
            if (child.class_name() == class_name) {
                result.push_back(child);
            }
        }
        return result;
    }

    Instance Instance::find_descendant(const std::string& name_to_find) const {
        if (!address) return Instance(0);

        for (const auto& child : get_children()) {
            if (child.name() == name_to_find) {
                return child;
            }

            Instance found = child.find_descendant(name_to_find);
            if (found) return found;
        }
        return Instance(0);
    }


    Vector3 Instance::getPosition() const {
        return address ? memory::read<Vector3>(address + offsets::Position) : Vector3();
    }

    Vector3 Instance::getVelocity() const {
        return address ? memory::read<Vector3>(address + offsets::Velocity) : Vector3();
    }

    float Instance::getWalkSpeed() const {
        return address ? memory::read<float>(address + offsets::WalkSpeed) : 0.0f;
    }

    float Instance::getJumpPower() const {
        return address ? memory::read<float>(address + offsets::JumpPower) : 0.0f;
    }

    bool Instance::getAnchored() const {
        return address ? memory::read<bool>(address + offsets::Anchored) : false;
    }

    bool Instance::getCanCollide() const {
        return address ? memory::read<bool>(address + offsets::CanCollide) : false;
    }

    float Instance::getHealth() const {
        return address ? memory::read<float>(address + offsets::Health) : 0.0f;
    }

    float Instance::getMaxHealth() const {
        return address ? memory::read<float>(address + offsets::MaxHealth) : 0.0f;
    }

    float Instance::getTransparency() const {
        return address ? memory::read<float>(address + offsets::Transparency) : 0.0f;
    }

    std::string Instance::getDisplayName() const {
        if (!address) return "";
        uintptr_t ptr = memory::read<uintptr_t>(address + offsets::DisplayName);
        return (ptr != 0) ? memory::read_string(ptr) : "";
    }

    Instance Instance::getTeam() const {
        return address ? Instance(memory::read<uintptr_t>(address + offsets::Team)) : Instance(0);
    }

    std::string Instance::getValue() const {
        if (!address) return "";
        uintptr_t ptr = memory::read<uintptr_t>(address + offsets::Value);
        return (ptr != 0) ? memory::read_string(ptr) : "";
    }

    bool Instance::is_a(const std::string& class_name) const {
        return this->class_name() == class_name;
    }

    std::string Instance::get_full_path() const {
        if (!address) return "";

        std::vector<std::string> path_parts;
        Instance current = *this;

        while (current && current.address != rbx::datamodel) {
            path_parts.push_back(current.name() + "[" + current.class_name() + "]");
            current = current.parent();
        }

        std::string path;
        for (auto it = path_parts.rbegin(); it != path_parts.rend(); ++it) {
            path += *it;
            if (it + 1 != path_parts.rend()) {
                path += " → ";
            }
        }
        return path;
    }

}
