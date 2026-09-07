# Engineering assessment — 2026-09-07

## Verified local baseline

Host: ARM64, macOS 15.7.3; Apple Clang available. CMake was initially absent and has since been installed.
The workspace initially contained a game installation, without a source project or Git repository.
Both installed game executables are Windows PE32/i386. The installed and `_backup_v2.1`
executables have different hashes; the backup directory name is not proof of an unmodified release.
See `research/binary-inventory.json` for exact identities. Imported dependencies include DirectDraw,
DirectSound, DirectPlay, WinMM, GDI, USER32 and video APIs. Native execution requires replacing these
platform services as well as compiling recovered/reimplemented engine logic for ARM64.

## Sources and suitability

* [Thinker](https://github.com/induktio/thinker), local commit
  `b73e88bf1deaa674082d357a23dc08ed542f3038`: substantial reconstructed/replacement game logic.
  Its CMake file targets i686 MinGW. `src/engine.cpp` binds globals and functions to addresses
  in the original executable. This is a strong AI reference, not a standalone portable engine.
  Its `-native` launch option means native desktop resolution, not native OS execution.
* [OpenSMACX](https://github.com/Dionysus1/OpenSMACX), local commit
  `54929749e757c37a165255a47f3f9fe37e53ccf9`: explicitly a decompilation project. This checkout's
  source is limited to debugging, file mapping, heap, strings, and text indexing; it is not
  a complete compilable game replacement.
* [GLSMAC](https://github.com/afwbkbc/glsmac): an independent engine reimplementation.
  The README inspected today identifies v0.3+, puts AI/diplomacy on its later roadmap, and
  reports Mac OpenGL crashes. It also warns that original binary mods are incompatible and
  text-mod/save compatibility is limited or not a priority. It is not presently established
  as a route to the complete original game with a replaced AI. A local ARM64 build now runs; see the updated coverage report.

Recommendation: use Thinker's recovered logic as the primary research baseline, and establish
standalone engine coverage before committing to a full port architecture. Neither changing a
compiler target nor putting a native launcher in front of `terranx.exe` achieves the requirement.
Static recompilation is a research alternative, not a demonstrated shortcut: indirect calls,
32-bit layouts, platform APIs, and executable modifications still need resolving.

## Decision paths already located

Paths below are relative to `research/upstream/thinker/` and the pinned commit above.
They describe upstream source, not verified equivalence to the local patched binaries.

| Concern | Source entry points | Required investigation |
| --- | --- | --- |
| Excess defenders | `src/build.cpp:810` `select_build`; defender accounting near 876; `src/move.cpp:1080` defense goals | Trace threat estimates into production, existing forces, support burden, and redeployment before adding defenders. |
| Mobile forces | `src/veh.cpp:3039` garrison classification; `src/faction.cpp:1562` design consideration; `src/move.cpp` combat movement | Trace design selection and production through actual attack reach, terrain, aircraft range and return obligations. |
| Terraforming | `src/move.cpp:1568` `can_borehole`; yield comparison near 1883; `src/move.cpp:2047` `former_move` | Already includes advanced improvements; evaluate investment horizon, worker assignment, prerequisites and ecological costs. |
| Diplomacy | `src/faction.cpp:862` `great_satan`; `evaluate_attack` near 1570; `mod_wants_to_attack` at 1751 | Enumerate all war, trade, pact and dialogue gates; changing attack willingness alone is insufficient. |

The rank complaint has a concrete counterpart: `evaluate_attack` near line 1636 exempts the
top-ranked target from one pacifist no-attack rule. `great_satan` uses ranking strength,
difficulty, turn timing and victory conditions. Other attack branches involve treaties,
atrocities, revenge, strength and personality. This supports investigating anti-leader behavior;
it does not prove that rank alone determines every original diplomatic refusal. Savegame and
binary-level comparisons are still needed to establish exact behavior for this installation.

## Proposed AI behavior contract

* Production: cover credible threats with nearby available forces. Account for travel time,
  support costs and competing economic investment. Avoid fixed defender quotas at safe bases.
* Warfare: value units by reachable objectives and expected campaign contribution. Use mobile
  reserves, concentrated attacks and supported air operations. Preserve faction preferences.
* Terraforming: compare legal improvement sequences over a planning horizon, including former
  turns, travel, worked tiles, nutrient/mineral/energy needs, technology and ecology. Advanced
  improvements should win when their net benefit warrants them, not be built everywhere.
* Diplomacy: separate personality, ideology, trust, grievances, shared interests and concrete
  strategic threat. Rank alone must not force aggression or forbid trade. A credible imminent
  victory can motivate a response without erasing relationships or making it universal.
* Fairness and diagnosis: decisions use an explicit faction knowledge view. Log candidate
  scores, rejected alternatives and reasons. Keep deterministic randomness for replayability.

These are proposed acceptance criteria, not claims about completed behavior.

## Implementation sequence and gates

1. **Executable correspondence and dependency map.** Import the hash-identified executable into
   a disassembler/decompiler; map Thinker hooks and their expected bytes to the local build.
   Trace reachable original functions for one complete AI decision and enumerate uncovered
   callees. Keep original, recovered and newly designed behavior distinguishable.
2. **Native feasibility slice.** Compile one recovered rules subsystem as an ARM64 Mach-O
   executable using owned state rather than fixed addresses. Validate outputs against known
   fixtures, then load original assets and render a map through a native platform backend.
   This is a milestone, not a playable port. Explicitly resolve packed 32-bit save layouts,
   pointer widths, calling conventions and deterministic arithmetic.
3. **Engine closure.** Implement or recover turn processing, movement/combat, economy,
   technology, diplomacy, UI, saves, audio and remaining platform services. Track original
   executable dependencies to zero. Acceptance: a native game can start, play, save, reload
   and complete a campaign without Wine or an original executable process.
4. **AI replacement.** Put policy behind an engine-independent state/action interface.
   Replace each decision family with reproducible scenarios and explanations, then integrate
   into complete turns. A Windows hook adapter could assist reference testing, but is not
   the native deliverable and has not been built or installed.
5. **Campaign evaluation.** Compare fixed seeds, factions and difficulties. Measure idle
   defenders/support costs, useful mobile deployments, terraform returns per former-turn,
   and diplomacy stability. Test a peaceful ideologically aligned leader, a weaker aggressor,
   betrayal and an imminent rival victory separately. Win rate alone is insufficient.

Full native gameplay remains a substantial engine reconstruction project. The current deliverable
includes the source map and native GLSMAC startup prototype. No decompiler run, new AI
implementation, or full campaign validation has occurred. See `PORT_COVERAGE.md`.
