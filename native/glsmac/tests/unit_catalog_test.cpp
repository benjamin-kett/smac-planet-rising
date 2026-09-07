#include "game/backend/unit/UnitCatalog.h"
#include "game/backend/unit/PrototypeCost.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

using game::backend::unit::UnitCatalog;

// Synthetic rules: no commercial game data is needed for this regression test.
const std::string fixture = R"(#TECHNOLOGY
Test Knowledge, Test, 0, 0, 0, 0, None, None, 000
#CHASSIS
Walker,M1,Walkers,M2,Guard,M1,Guards,M2,1,0,0,0,1,1,None,Large,M1,Larger,M2,
Vessel,M1,Vessels,M2,Boat,M1,Boats,M2,4,1,0,0,2,4,Test,Large,M1,Larger,M2,
#REACTORS
First Reactor,First,1,None,
Second Reactor,Second,2,Test,
#WEAPONS
Test Weapon,Gun,2,0,2,-1,None,
Test Colony,Colony,0,8,10,-1,None,
#DEFENSES
Test Armor,Armor,3,0,3,None,
#ABILITIES
Test Ability,1,Test,,000000001011,Description
#MORALE
C0,N0
C1,N1
C2,N2
C3,N3
C4,N4
C5,N5
C6,N6
#UNITS
2
Test Scout,Walker,Gun,Armor,3,0,0,None,-1,0,1
Test Sea Colony,Vessel,Colony,Armor,8,0,0,Disable,-1,1,2
#END
)";

void Check(bool value, const std::string& message) {
    if (!value) throw std::runtime_error(message);
}
UnitCatalog Parse(const std::string& text) {
    std::istringstream in(text);
    return UnitCatalog::Read(in, "fixture.txt");
}
void Reject(const std::string& from, const std::string& to, const std::string& error) {
    auto text = fixture;
    const auto pos = text.find(from);
    Check(pos != std::string::npos, "invalid test replacement");
    text.replace(pos, from.size(), to);
    try { Parse(text); } catch (const std::runtime_error& e) {
        Check(std::string(e.what()).find(error) != std::string::npos, e.what());
        return;
    }
    throw std::runtime_error("accepted invalid input: " + error);
}

int main(int argc, char** argv) {
    try {
        if (argc > 1) {
            for (int i = 1; i < argc; ++i) {
                const auto rules = UnitCatalog::Load(argv[i]);
                std::cout << argv[i] << ": " << rules.units.size() << " units, "
                    << rules.chassis.size() << " chassis, " << rules.weapons.size()
                    << " weapons, " << rules.armor.size() << " armor, "
                    << rules.abilities.size() << " abilities\n";
            }
            return 0;
        }
        auto cost_rules = Parse(fixture);
        auto design = cost_rules.units[0];
        auto& chassis = cost_rules.chassis[0];
        auto& weapon = cost_rules.weapons[0];
        auto& armor = cost_rules.armor[0];
        chassis.cost = 1; weapon.cost = 1; armor.cost = 1;
        Check(PrototypeCost(cost_rules, design) == 1, "basic infantry cost");
        weapon.cost = 10; weapon.mode = 8;
        Check(PrototypeCost(cost_rules, design) == 3, "unarmored colony cost");
        weapon.cost = 6; weapon.mode = 9;
        Check(PrototypeCost(cost_rules, design) == 2, "terraformer cost");
        weapon.cost = 1; weapon.mode = 0; chassis.cost = 2;
        Check(PrototypeCost(cost_rules, design) == 2, "rover cost");
        chassis.triad = 1; chassis.cost = 4; weapon.cost = 10; weapon.mode = 8;
        Check(PrototypeCost(cost_rules, design) == 7, "sea colony cost");
        chassis.triad = 2; chassis.cost = 12; chassis.missile = 1;
        weapon.offense = 99; weapon.cost = 32;
        Check(PrototypeCost(cost_rules, design) == 32, "planet buster cost shortcut");
        bool invalid_reactor = false;
        cost_rules.reactors[0].power = 0;
        try { PrototypeCost(cost_rules, design); } catch (const std::invalid_argument&) { invalid_reactor = true; }
        Check(invalid_reactor, "invalid reactor rejected before shift");
        const auto rules = Parse(fixture);
        Check(rules.units.size() == 2, "unit count");
        const auto& colony = rules.units.at(1);
        Check(colony.chassis == 1 && colony.weapon == 1 && colony.armor == 0 && colony.reactor == 1, "component references");
        Check(rules.chassis[colony.chassis].triad == 1 && rules.chassis[colony.chassis].speed == 4, "sea chassis");
        Check(rules.weapons[colony.weapon].mode == 8, "colony role");
        Check(rules.armor[colony.armor].defense == 3 && rules.reactors[colony.reactor].power == 2, "defense and reactor");
        Check(colony.prerequisite == "Disable" && colony.abilities == 1, "disabled design and bit flags");
        Check(rules.abilities[0].abbreviation.empty(), "empty CSV field");
        auto crlf = std::string("\xef\xbb\xbf");
        for (char c : fixture) crlf += c == '\n' ? "\r\n" : std::string(1,c);
        Check(Parse(crlf).units.size() == 2, "BOM and CRLF");
        Check(Parse(fixture + "ignored text\n").units.size() == 2, "END terminates section");
        Reject("#UNITS\n2", "#UNITS\n3", "unit count");
        Reject("Scout,Walker", "Scout,Missing", "unknown component");
        Reject("3,0,0,None", "3,0,0,Unknown", "unknown prerequisite");
        Reject("1,0,0,0,1,1,None", "1,9,0,0,1,1,None", "out-of-range");
        Reject("1,0,0,0,1,1,None", "1x,0,0,0,1,1,None", "invalid integer");
        Reject("-1,0,1", "-1,2,1", "invalid binary");
        Reject("-1,0,1", "-1,10,1", "undefined unit ability");
        Reject("-1,1,2", "-1,1,0", "out-of-range");
        Reject("Test Scout,", "Test Sea Colony,", "duplicate unit name");
        Reject("#END", "#UNITS", "duplicate section");
        Reject("#MORALE", "#NO_MORALE", "missing or empty #MORALE");
        Reject("C6,N6\n", "", "seven morale");
        std::cout << "All unit catalog tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
