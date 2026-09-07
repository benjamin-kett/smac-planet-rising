# Native SMAC / Alien Crossfire coverage

Updated 2026-09-07. Target: complete native ARM64 gameplay, including Alien
Crossfire, before changing AI policy. Current status: **incomplete prototype**.

The executable is a locally modified GLSMAC reimplementation. It does not run
the original Windows engine, and loading expansion assets does not recover the
expansion's compiled rules. Thinker and OpenSMACX remain research references.

| Area | Current evidence | Required for a complete port |
| --- | --- | --- |
| Native platform | ARM64 Mach-O; SDL, Apple OpenGL 4.1, AVKit movie playback | Broader host testing and standalone library packaging |
| Expansion intro | Original `openingx.wve` transcoded locally; watched through completion | Validate sound output and remaining in-game movies |
| Expansion menu | `xopeninga.pcx`, all seven expansion factions selectable; original seven retained | Original setup options, faction-specific presentation and rules |
| Landing | Original human/alien FLC frames followed by smoke, positioned on the starting tile | Correct starting roster, Progenitor arrival timing, sea starts and fidelity comparison |
| Factions | Selected ID reaches backend; names, colors, base art and naval/alien flags | Complete faction bonuses, restrictions, initial techs and special mechanics |
| World | Random map renders, units move, prototype bases and tile yields exist | Original generation, visibility and resource-rule parity |
| Units | Four native-life definitions; new games use a placeholder mindworm | Conventional designs, colony pods, formers, rovers, aircraft, transports and expansion abilities |
| Bases/economy | Prototype growth, population and worked tiles | Production queues, facilities, projects, support, ecology and all economic rules |
| Technology/social engineering | No complete gameplay implementation established | Research tree, prerequisites, unlocks, social choices and effects |
| Diplomacy/AI | No complete campaign behavior | Diplomacy state/dialogue, AI turns and original behavior baseline before rebuilding policy |
| Persistence/victory | No complete campaign validated | Save/reload continuity, victory conditions, game completion and compatibility policy |
| Stability | One successful in-game exit after fixing worker teardown; focused startup tests pass | Resolve full GSE suite crash, GC cleanup warnings and broader gameplay failures |

## Next implementation gates

1. Recover or implement the conventional unit model and original starting state.
   A human faction must begin with its correct colony and military units rather
   than a mindworm and prebuilt HQ. Establish expansion sea/alien start cases.
2. Close one playable economic loop: found a base, work tiles, produce a unit,
   terraform, research an unlock and advance turns. Validate rules against the
   hash-identified original installation and recovered source, not artwork.
3. Extend that loop to combat, diplomacy, AI opponents, save/reload and victory.
   Cover the expansion's additional mechanics explicitly.
4. Only after a reproducible complete campaign, replace the AI decisions.

These are unfinished engineering requirements, not delivered features. A faithful
port requires recovering or rebuilding missing game systems; there is no complete
portable original-engine source tree in the current workspace.
