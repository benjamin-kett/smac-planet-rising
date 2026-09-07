#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>

namespace game::backend::unit {

// Owned, portable rules data. No original executable addresses or 32-bit pointers.
// Index order is significant: the original predefined designs use these indexes.
struct ChassisRule {
    std::string name, prerequisite;
    int speed, triad, range, missile, cargo, cost;
};
struct WeaponRule {
    std::string name, short_name, prerequisite;
    int offense, mode, cost, icon;
};
struct ArmorRule {
    std::string name, short_name, prerequisite;
    int defense, mode, cost;
};
struct ReactorRule {
    std::string name, short_name, prerequisite;
    int power;
};
struct AbilityRule {
    std::string name, prerequisite, abbreviation;
    int cost;
    uint32_t flags;
};
struct UnitBlueprint {
    std::string name, prerequisite;
    size_t chassis, weapon, armor, reactor;
    int plan, cost, cargo, icon;
    uint32_t abilities;
};

class UnitCatalog {
public:
    static UnitCatalog Read(std::istream& source, const std::string& filename);
    static UnitCatalog Load(const std::string& filename);

    std::vector<ChassisRule> chassis;
    std::vector<WeaponRule> weapons;
    std::vector<ArmorRule> armor;
    std::vector<ReactorRule> reactors;
    std::vector<AbilityRule> abilities;
    std::vector<UnitBlueprint> units;
    std::vector<std::string> technology_codes;
    std::vector<std::string> conventional_morale, native_morale;
};

}
