# ADR-003: Carry the map plane in `MapPoint.z` and keep `_SMALL_MAPPOINTS` defined

**Status:** Proposed
**Date:** 2026-11-17
**Decision owner:** AI, from the operator's instruction of 2026-11-16 to start the Dual Maps feature
**Governs:** `ctp2-dual-maps.spec.md`, `.specs/reqs/ctp2-dual-maps.rst`

## Context

Two persistent map planes need a plane identity on every map location. `MapPoint.h:46` defines `_SMALL_MAPPOINTS`, so `MapPoint` has no `z` member and `MapPoint::Serialize` writes x and y only. `World::GetCell(const MapPoint&)` indexes one grid and 119 files read the one global `g_theWorld`.

The dormant 3-D branches (`#if !defined(_SMALL_MAPPOINTS)`, 25 blocks) cannot simply be switched on. The same macro adds `DOWN` and `UP` to `WORLD_DIRECTION` (`gs/utility/directions.h:38`), and 83 uses of `NOWHERE` (for example `for (i = 0; i < NOWHERE; i++)` over neighbours in `Goal.cpp`, `ctpai.cpp`, `agent.cpp`) would then visit vertical neighbours and size arrays one larger. `IsValid` would also test `z < g_mp_size.z` while `g_mp_size.z` is never set.

## Decision

1. Keep `_SMALL_MAPPOINTS` defined. Add `sint16 z` to `MapPointData` unconditionally, default 0, meaning a plane index (`GROUND_Z` 0, `SPACE_Z` 1). It is not a spatial axis and no direction changes it.
2. Compare z in position equality. Preserve z in every derived position (neighbour, `OrthogonalPoint`, iterators, `xy2rc`, `rc2xy`, `+=`, `-=`).
3. Serialize z, gated on the save version: write always; read when the file version is below zero or at least 68. Add `CTP0068` as the newest magic value. Do not take 67, which `USE_FORMAT_67` reserves together with a sprite-group format change.
4. Land this as its own change, behaviour-neutral for single-plane games, before any change that gives a plane its own cells.

## Consequences

- Every file that includes `MapPoint.h` recompiles with a 2-byte-larger struct (258 files, 3,594 hits). No code in the tree uses `sizeof(MapPoint)` or raw struct reads and writes, so the size change reaches only the archive format.
- Saves written by this build are rejected by older engines, and a loaded `CTP0066` save triggers the existing older-version SLIC reload (`civapp.cpp:1740`, `:2082`) because the version is below the current one.
- Later slices (per-plane cells, vision, unit trees, view, movement, Towers) can select a plane from `MapPoint.z`.
- The dormant 3-D blocks stay dormant; deleting them is a separate cleanup.

## Rejected

- Remove `#define _SMALL_MAPPOINTS`. Adds `UP` and `DOWN` neighbours to 83 direction loops. Wins only if the game ever needs a true third axis.
- Keep `MapPoint` 2-D and swap the contents of `g_theWorld` when the viewed plane changes. A unit on the other plane keeps an x, y that would index the wrong grid, and any loop over all units that calls `GetCell` reads the wrong plane. Wins only if no code iterates units while a different plane is viewed; that count was not taken.
- Store the plane on `Unit` and `Army` only. Positions passed to `GetCell`, vision and pathfinding would still lack a plane.

## AI-Context

- **Constraints:** single-plane games unchanged; no `UP` or `DOWN` direction; two-coordinate constructors keep compiling with z = 0; saves versioned, old saves readable.
- **Patterns:** version gate by `g_saveFileVersion` as in `GameFile.cpp` and 20 other files; magic table `s_magicValue[]`.
- **Anti-patterns:** do not undefine `_SMALL_MAPPOINTS`; do not add a second world object yet; do not reuse version 67.
- **Verification commands:** build the Release and Debug pair; load a `CTP0066` save from the main menu and compare logged positions with the baseline build; save, confirm the header reads `CTP0068`, reload; open the new file in an older build and record the refusal.
