// Derived from Thinker mod_proto_cost, commit b73e88bf1deaa674082d357a23dc08ed542f3038.
// Copyright (c) Thinker Mod authors. MIT license: /licenses/Thinker-MIT.txt.
// Replaced fixed executable addresses with owned UnitCatalog records.
#include "PrototypeCost.h"
#include <algorithm>
#include <stdexcept>
#include <limits>

namespace game::backend::unit {
int PrototypeCost(const UnitCatalog& rules, const UnitBlueprint& design) {
    const auto& chassis = rules.chassis.at(design.chassis);
    const auto& weapon = rules.weapons.at(design.weapon);
    const auto& armor = rules.armor.at(design.armor);
    const int reactor = rules.reactors.at(design.reactor).power;
    if (reactor < 1 || reactor > 4) throw std::invalid_argument("reactor power must be 1..4");
    if (rules.abilities.size() > 32 || (rules.abilities.size() < 32 && (design.abilities >> rules.abilities.size()))) {
        throw std::invalid_argument("undefined unit ability bit");
    }
    const int weapon_cost = weapon.cost;
    if (chassis.missile && weapon.offense >= 99) return weapon_cost;
    int armor_cost = armor.cost;
    int speed_cost = chassis.cost;
    int ability_modifier = 0, flag_modifier = 0;
    for (size_t i = 0; i < rules.abilities.size(); ++i) {
        if (!(design.abilities & (uint32_t{1} << i))) continue;
        if (ability_modifier) ++ability_modifier;
        const auto& ability = rules.abilities[i];
        if (ability.cost > 0) {
            ability_modifier += ability.cost;
        } else {
            switch (ability.cost) {
                case -1: if (armor_cost) ability_modifier += std::clamp(weapon_cost / armor_cost, 0, 2); break;
                case -2: ability_modifier += weapon_cost - 1; break;
                case -3: ability_modifier += armor_cost - 1; break;
                case -4: ability_modifier += speed_cost - 1; break;
                case -5: ability_modifier += weapon_cost + armor_cost - 2; break;
                case -6: ability_modifier += weapon_cost + speed_cost - 2; break;
                case -7: ability_modifier += armor_cost + speed_cost - 2; break;
                default: break;
            }
        }
        if ((ability.flags & (1u << 10)) && chassis.triad == 0) ++flag_modifier;
    }
    ability_modifier += flag_modifier;
    if (chassis.triad == 1) {
        armor_cost /= 2;
        speed_cost += reactor;
    } else if (chassis.triad == 2) {
        if (armor_cost > 1) armor_cost *= reactor * 2;
        speed_cost += reactor * 2;
    }
    const int combat = std::max(armor_cost / 2 + 1, weapon_cost);
    int prototype;
    if (combat == 1 && armor_cost == 1 && speed_cost == 1 && reactor == 1) {
        prototype = 1;
    } else {
        prototype = ((speed_cost + armor_cost) * combat + (1 << reactor)) / (2 << reactor);
        if (speed_cost == 1) prototype = prototype / 2 + 1;
        if (weapon_cost > 1 && armor.cost > 1) {
            ++prototype;
            if (chassis.triad == 0 && speed_cost > 1) ++prototype;
        }
        if (chassis.triad == 1 && weapon.mode != 11) {
            prototype = (prototype + 1) / 2;
        } else if (chassis.triad == 2) {
            prototype /= weapon.mode > 2 ? 2 : 4;
        }
        prototype = std::max(prototype, (reactor * 3 + 1) / 2);
    }
    const int64_t cost = (int64_t{prototype} * (ability_modifier + 4) + 2) / 4;
    if (cost < 0 || cost > std::numeric_limits<int>::max()) {
        throw std::overflow_error("prototype cost is outside the supported range");
    }
    return static_cast<int>(cost);
}
}
