# Recovered native routines

## Prototype cost

`native/glsmac/src/game/backend/unit/PrototypeCost.cpp` is a native C++ adaptation
of Thinker `mod_proto_cost`, pinned at commit
`b73e88bf1deaa674082d357a23dc08ed542f3038`, `src/veh.cpp`.
The reference maps the original routine to address `0x005A5A60`.

Inputs are owned unit-component records, replacing fixed `Chassis`, `Weapon`,
`Armor` and `Ability` arrays in the Windows process. The calculation preserves
ability ordering, integer rounding, land/sea/air adjustments, reactors, the
planet-buster exception and sea-probe handling. It returns mineral rows, not
final base production cost. Invalid reactor shifts and unsupported cost overflow
are rejected.

`tools/verify_prototype_cost.py` compiles the reference function directly from
that pinned Git object in a temporary directory and compares all component
combinations with zero, one and two abilities. The local expansion catalog
produced 5,713,344 matching cases. Correspondence to the recovered implementation
is verified; original-executable behavior still needs independent verification.

## Executable analysis

`tools/ghidra/ExportPortSlice.java` locates candidate unit-construction routines
using the Thinker address map and exports local decompiler output. These are
candidate labels until byte identities, calls and types are checked. The Ghidra
project and output stay under ignored `research/ghidra` and `research/decompiled`.
They are not compilable native engine implementations merely because a
pseudocode export exists.
