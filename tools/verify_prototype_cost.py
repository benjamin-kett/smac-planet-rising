#!/usr/bin/env python3
"""Compare native cost against pinned Thinker source, without a Windows runtime.

Usage: python3 tools/verify_prototype_cost.py research/upstream/thinker 'Alpha Centauri/alphax.txt'
The temporary reference program is compiled from the supplied MIT-licensed checkout.
This verifies correspondence with recovered source, not the original executable.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('thinker', type=Path)
parser.add_argument('rules', type=Path)
args = parser.parse_args()
pinned = 'b73e88bf1deaa674082d357a23dc08ed542f3038'
revision = subprocess.check_output(['git', '-C', str(args.thinker), 'rev-parse', 'HEAD'], text=True).strip()
if revision != pinned:
    raise SystemExit('Unexpected Thinker revision: ' + revision)
# Read the committed source so local edits cannot silently change the oracle.
source = subprocess.check_output(['git', '-C', str(args.thinker), 'show', pinned + ':src/veh.cpp'], text=True)
begin = source.index('int __cdecl mod_proto_cost(')
end = source.index('\n    return cost;\n}', begin) + len('\n    return cost;\n}')
reference = source[begin:end].replace('__cdecl ', '')
reference = reference.replace('    assert(cost == proto_cost(chassis_id, weapon_id, armor_id, ability, reactor_id));', '')
preamble = r'''
#include "game/backend/unit/PrototypeCost.h"
#include <algorithm>
#include <cstdint>
#include <iostream>
using namespace game::backend::unit;
using std::clamp;
using VehChassis=int; using VehWeapon=int; using VehArmor=int;
using VehAblFlag=uint32_t; using VehReactor=int;
constexpr int TRIAD_LAND=0, TRIAD_SEA=1, TRIAD_AIR=2, REC_FISSION=1;
constexpr int WMODE_PROBE=11, WMODE_MISSILE=2, AFLAG_COST_INC_LAND_UNIT=1<<10;
struct W { int cost, offense_value, mode; };
std::vector<ChassisRule> Chassis;
std::vector<W> Weapon;
std::vector<ArmorRule> Armor;
std::vector<AbilityRule> Ability;
int MaxAbilityNum;
'''
main = r'''
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    auto rules=UnitCatalog::Load(argv[1]);
    Chassis=rules.chassis; Armor=rules.armor; Ability=rules.abilities;
    MaxAbilityNum=Ability.size();
    for (const auto& w:rules.weapons) Weapon.push_back({w.cost,w.offense,w.mode});
    std::vector<uint32_t> masks{0};
    for(int i=0;i<MaxAbilityNum;++i) {
        masks.push_back(uint32_t{1}<<i);
        for(int j=i+1;j<MaxAbilityNum;++j) masks.push_back((uint32_t{1}<<i)|(uint32_t{1}<<j));
    }
    uint64_t checks=0;
    for(size_t c=0;c<Chassis.size();++c)
    for(size_t w=0;w<Weapon.size();++w)
    for(size_t a=0;a<Armor.size();++a)
    for(size_t r=0;r<rules.reactors.size();++r)
    for(auto mask:masks) {
        UnitBlueprint design{};
        design.chassis=c; design.weapon=w; design.armor=a; design.reactor=r; design.abilities=mask;
        const int expected=mod_proto_cost(c,w,a,mask,rules.reactors[r].power);
        const int actual=PrototypeCost(rules,design);
        if(actual!=expected) {
            std::cerr<<"Mismatch "<<c<<','<<w<<','<<a<<','<<r<<','<<mask<<": "<<actual<<" vs "<<expected<<'\n';
            return 1;
        }
        ++checks;
    }
    std::cout<<"Matched recovered Thinker cost for "<<checks<<" component/ability combinations\n";
}
'''
root = Path(__file__).resolve().parents[1]
engine = root / 'native/glsmac/src'
with tempfile.TemporaryDirectory(prefix='smac-cost-') as temp:
    temp = Path(temp)
    program = temp / 'reference.cpp'
    program.write_text(preamble + reference + main)
    binary = temp / 'verify'
    subprocess.run(['clang++', '-std=c++17', '-O2', '-I', str(engine), str(program),
                    str(engine / 'game/backend/unit/UnitCatalog.cpp'),
                    str(engine / 'game/backend/unit/PrototypeCost.cpp'), '-o', str(binary)], check=True)
    subprocess.run([str(binary), str(args.rules.resolve())], check=True)
