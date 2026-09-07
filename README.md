# Alpha Centauri: native engine and AI rebuild

Status: a native ARM64 GLSMAC prototype plays the Alien Crossfire intro, offers
all fourteen factions, and starts a map with human or alien landing animations. This is not yet a complete native port of
the original game. See [native build notes](native/README.md). No new AI is implemented.

The target is an ARM64 macOS application using the existing game assets, with rebuilt production,
combat, terraforming, and diplomacy decisions. Wine and other executable compatibility runtimes
are outside the target. Game balance text edits alone cannot implement this project.

See [the engineering assessment](docs/ENGINEERING.md) for evidence, implementation boundaries,
and milestones. The original installation is in `Alpha Centauri/` and has not been modified.

## Research baseline

`research/upstream/thinker/` and `research/upstream/OpenSMACX/` are public upstream source
checkouts, retained with their existing attribution/license files. They are references, not
an installed mod. Do not distribute this workspace wholesale: it contains the game installation.

Regenerate the read-only executable inventory with:

```sh
python3 tools/inventory_binaries.py 'Alpha Centauri' > research/binary-inventory.json
```

The inventory records SHA-256 identities, PE sections, entry addresses, and imported libraries.
It does not establish compatibility with any mod's expected executable build.
