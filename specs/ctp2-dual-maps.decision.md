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

- Every file that includes `MapPoint.h` recompiles with a larger struct (258 files, 3,594 hits; 4 bytes becomes 6). Several save formats copy structs that embed a `MapPoint` as raw memory, so the size change alters their on-disk layout. A first Debug build loaded a `CTP0066` autosave into a crash in `GameFile::Restore` (`CivArchive::SetSize` with a garbage length) because the 64-entry starting-position table in the save header grew from 8 to 12 bytes per entry. The raw sites found and fixed in this slice: the save header and `World` starting positions (`StartingPosition` now has explicit disk layouts), the SLIC location list (`SimpleDynamicArray<MapPoint>`), and chunk copies that span a `MapPoint` member in `InstallationData`, `TerrainImprovementData`, `Player` (`m_setupCenter`) and `Wormhole`. Each is split around the `MapPoint`, which is written by `MapPoint::Serialize`, so the bytes of an old file are unchanged. The check that no site was missed is that a legacy load consumes exactly the stored stream.
- Two load gates written for the never-shipped format 67 mis-read any file newer than 66, so a first `CTP0068` file loaded into garbage (an assertion in `SpriteGroupList::GetSprite` from `UnitActor`). `UnitActor` read 32-bit sprite and unit-database indices for `g_saveFileVersion >= 67` while the writer only emits them under `USE_FORMAT_67`, which is never defined; it now asks `gamefile_HasWideSpriteIndices()`, true for version 67 or, only when this build writes format 67, for later versions. `HappyTracker` skipped loading its array for versions above 66 (`else if (g_saveFileVersion <= 66)`); it now loads the version-66 array for every version above 64. A trace of every raw archive read against the write sequence (offset, length, caller) matched exactly up to the failure, which is what localised the fault to plain field reads. The remaining version gates in the tree compare with `>=` against numbers at or below 66 and are unaffected.
- Map files have no version number of their own (header `CTPMAP__`, world version 0), so a map written by an older build cannot be told from one with the plane. New maps carry the header `CTPMAP68`; `GameMapFile::Restore` and `ValidateGameMapFile` accept both headers and read the old one as version 66. This path is covered by the build only; no map file was loaded in the tests of this slice.
- After a load, `GameFile::Restore` and `GameMapFile::Restore` set `g_saveFileVersion` to the current version so later in-memory archive reads and saves do not follow the layout of the file that was loaded.
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
