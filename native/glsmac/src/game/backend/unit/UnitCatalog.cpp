#include "UnitCatalog.h"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace game::backend::unit {
namespace {
struct Row {
    size_t line;
    std::vector<std::string> fields;
};
using Sections = std::map<std::string, std::vector<Row>>;

std::string Trim(const std::string& text) {
    const auto begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return {};
    return text.substr(begin, text.find_last_not_of(" \t\r\n") - begin + 1);
}

[[noreturn]] void Fail(const std::string& file, size_t line, const std::string& message) {
    throw std::runtime_error(file + ":" + std::to_string(line) + ": " + message);
}

Sections ReadSections(std::istream& in, const std::string& file) {
    Sections sections;
    std::string line, section;
    size_t number = 0;
    while (std::getline(in, line)) {
        ++number;
        if (number == 1 && line.compare(0, 3, "\xef\xbb\xbf") == 0) line.erase(0, 3);
        line = Trim(line.substr(0, line.find(';')));
        if (line.empty()) continue;
        if (line.front() == '#') {
            section = Trim(line.substr(1));
            if (section.compare(0, 3, "END") == 0) {
                section.clear();
            } else if (!sections.emplace(section, std::vector<Row>{}).second) {
                Fail(file, number, "duplicate section #" + section);
            }
            continue;
        }
        if (section.empty()) continue;
        Row row{number, {}};
        size_t begin = 0;
        do {
            const auto comma = line.find(',', begin);
            row.fields.push_back(Trim(line.substr(begin, comma - begin)));
            if (comma == std::string::npos) break;
            begin = comma + 1;
        } while (begin < line.size());
        sections.at(section).push_back(std::move(row));
    }
    if (in.bad()) Fail(file, number, "failed reading rules file");
    return sections;
}

const std::vector<Row>& Require(const Sections& sections, const std::string& name, const std::string& file) {
    const auto it = sections.find(name);
    if (it == sections.end() || it->second.empty()) Fail(file, 0, "missing or empty #" + name);
    return it->second;
}

void Columns(const Row& row, size_t count, const std::string& file) {
    if (row.fields.size() != count) {
        Fail(file, row.line, "expected " + std::to_string(count) + " columns, got " + std::to_string(row.fields.size()));
    }
}

int Integer(const Row& row, size_t column, const std::string& file) {
    const auto& value = row.fields.at(column);
    int result = 0;
    const auto parsed = std::from_chars(value.data(), value.data() + value.size(), result);
    if (parsed.ec != std::errc{} || parsed.ptr != value.data() + value.size()) {
        Fail(file, row.line, "invalid integer in column " + std::to_string(column + 1) + ": " + value);
    }
    return result;
}

int Bounded(const Row& row, size_t column, int min, int max, const std::string& file) {
    const int value = Integer(row, column, file);
    if (value < min || value > max) Fail(file, row.line, "out-of-range value in column " + std::to_string(column + 1));
    return value;
}

uint32_t Bits(const Row& row, size_t column, const std::string& file) {
    const auto& value = row.fields.at(column);
    if (value.empty() || value.size() > 32 || value.find_first_not_of("01") != std::string::npos) {
        Fail(file, row.line, "invalid binary flags in column " + std::to_string(column + 1));
    }
    uint32_t result = 0;
    for (char digit : value) result = (result << 1) | (digit - '0');
    return result;
}

size_t Reference(const std::string& name, const std::map<std::string, size_t>& names, const Row& row, const std::string& file) {
    const auto it = names.find(name);
    if (it == names.end()) Fail(file, row.line, "unknown component: " + name);
    return it->second;
}

void Name(std::map<std::string, size_t>& names, const std::string& name, size_t index, const Row& row, const std::string& file) {
    if (name.empty() || !names.emplace(name, index).second) Fail(file, row.line, "empty or duplicate component name: " + name);
}
}

UnitCatalog UnitCatalog::Load(const std::string& filename) {
    std::ifstream input(filename, std::ios::binary);
    if (!input) Fail(filename, 0, "cannot open rules file");
    return Read(input, filename);
}

UnitCatalog UnitCatalog::Read(std::istream& source, const std::string& filename) {
    const auto sections = ReadSections(source, filename);
    UnitCatalog result;
    std::set<std::string> technologies{"None", "Disable"};
    for (const auto& row : Require(sections, "TECHNOLOGY", filename)) {
        Columns(row, 9, filename);
        const auto& code = row.fields[1];
        if (code.empty() || !technologies.insert(code).second) Fail(filename, row.line, "duplicate or empty technology code: " + code);
        result.technology_codes.push_back(code);
    }
    const auto prereq = [&](const Row& row, size_t column) -> std::string {
        const auto& code = row.fields.at(column);
        if (technologies.find(code) == technologies.end()) Fail(filename, row.line, "unknown prerequisite: " + code);
        return code;
    };
    for (const auto& row : Require(sections, "TECHNOLOGY", filename)) {
        prereq(row, 6);
        prereq(row, 7);
    }
    std::map<std::string, size_t> chassis_names, weapon_names, armor_names;
    for (const auto& row : Require(sections, "CHASSIS", filename)) {
        Columns(row, 19, filename);
        Name(chassis_names, row.fields[0], result.chassis.size(), row, filename);
        result.chassis.push_back({row.fields[0], prereq(row, 14),
            Bounded(row, 8, 0, 255, filename), Bounded(row, 9, 0, 2, filename),
            Bounded(row, 10, 0, 255, filename), Bounded(row, 11, 0, 1, filename),
            Bounded(row, 12, 0, 255, filename), Bounded(row, 13, 0, 255, filename)});
    }
    for (const auto& row : Require(sections, "WEAPONS", filename)) {
        Columns(row, 7, filename);
        Name(weapon_names, row.fields[1], result.weapons.size(), row, filename);
        result.weapons.push_back({row.fields[0], row.fields[1], prereq(row, 6),
            Bounded(row, 2, -1, 255, filename), Bounded(row, 3, 0, 14, filename),
            Bounded(row, 4, 0, 255, filename), Integer(row, 5, filename)});
    }
    for (const auto& row : Require(sections, "DEFENSES", filename)) {
        Columns(row, 6, filename);
        Name(armor_names, row.fields[1], result.armor.size(), row, filename);
        result.armor.push_back({row.fields[0], row.fields[1], prereq(row, 5),
            Bounded(row, 2, -1, 255, filename), Bounded(row, 3, 0, 2, filename),
            Bounded(row, 4, 0, 255, filename)});
    }
    for (const auto& row : Require(sections, "REACTORS", filename)) {
        Columns(row, 4, filename);
        result.reactors.push_back({row.fields[0], row.fields[1], prereq(row, 3),
            Bounded(row, 2, 1, 4, filename)});
    }
    for (const auto& row : Require(sections, "ABILITIES", filename)) {
        // The description after flags is presentation text and may include commas.
        if (row.fields.size() < 6) Fail(filename, row.line, "ability needs six columns");
        result.abilities.push_back({row.fields[0], prereq(row, 2), row.fields[3],
            Bounded(row, 1, -7, 255, filename), Bits(row, 4, filename)});
    }
    if (result.abilities.size() > 32) Fail(filename, 0, "more than 32 unit abilities");
    for (const auto& row : Require(sections, "MORALE", filename)) {
        Columns(row, 2, filename);
        result.conventional_morale.push_back(row.fields[0]);
        result.native_morale.push_back(row.fields[1]);
    }
    if (result.conventional_morale.size() != 7) Fail(filename, 0, "expected seven morale levels");
    const auto& units = Require(sections, "UNITS", filename);
    Columns(units.front(), 1, filename);
    const auto count = Bounded(units.front(), 0, 1, 512, filename);
    if (units.size() != static_cast<size_t>(count) + 1) Fail(filename, units.front().line, "unit count does not match #UNITS rows");
    std::set<std::string> unit_names;
    for (size_t i = 1; i < units.size(); ++i) {
        const auto& row = units[i];
        Columns(row, 11, filename);
        if (row.fields[0].empty() || !unit_names.insert(row.fields[0]).second) Fail(filename, row.line, "empty or duplicate unit name");
        const auto abilities = Bits(row, 9, filename);
        if (result.abilities.size() < 32 && (abilities >> result.abilities.size()) != 0) Fail(filename, row.line, "undefined unit ability bit");
        result.units.push_back({row.fields[0], prereq(row, 7),
            Reference(row.fields[1], chassis_names, row, filename),
            Reference(row.fields[2], weapon_names, row, filename),
            Reference(row.fields[3], armor_names, row, filename),
            static_cast<size_t>(Bounded(row, 10, 1, result.reactors.size(), filename) - 1),
            Bounded(row, 4, -1, 14, filename), Bounded(row, 5, 0, 255, filename),
            Bounded(row, 6, 0, 255, filename), Integer(row, 8, filename), abilities});
    }
    return result;
}

}
