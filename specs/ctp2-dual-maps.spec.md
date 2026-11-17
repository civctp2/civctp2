---
description: 'Plane coordinate for CTP2 map locations: first slice of two persistent map planes'

***definitions***

- :Plane: is one persistent strategic map layer. Plane 0 (`GROUND_Z`) is the first plane and plane 1 (`SPACE_Z`) is the second. This slice supports only the index; it adds no second map.
- :Plane-Coordinate: is the `z` member of `MapPoint`. It names a plane. It is not a spatial axis and it never changes when a unit steps to a neighbouring tile.
- :Plane-Save-Version: is save-format version 68 (`CTP0068`), the first format whose `MapPoint` records carry the plane coordinate.

***implementation reqs***

- Governing basis: the Dual Maps feature (two persistent map planes, such as the Arcanus and Myrror planes of the Masters of Magic scenario). This slice covers only the plane index and the save format; the engine assumptions it addresses are listed in ADR-003. No earlier requirement exists.
- `MapPoint` and `MapPointData` must carry a plane coordinate `z`, default 0, in every build configuration.
- `_SMALL_MAPPOINTS` must stay defined. It also removes `UP` and `DOWN` from `WORLD_DIRECTION`; `NOWHERE` is used by 83 direction loops that must keep visiting the eight map directions only.
- Every two-coordinate construction (`MapPoint(x, y)`, `MapPointData(x, y)`) must still compile and must yield plane 0. `Set(x, y)` must still compile and must leave the plane of the point it is called on unchanged.
- Position equality and inequality (`MapPointData::operator==`, `MapPoint::operator==`, `MapPoint::operator!=`) must compare the plane coordinate as well as x and y.
- Every `MapPoint` operation that derives a position from another position (neighbour lookup, `OrthogonalPoint`, `SquareIterator`, `RadiusIterator`, `xy2rc`, `rc2xy`, `+=`, `-=`) must return a position on the same plane as its source.
- `MapPoint::Serialize` must write x, y and z when storing. When loading, it must read z only if the file version is below zero (an in-memory archive) or at least the Plane-Save-Version; otherwise it must read x and y and set z to 0.
- The save-format table must gain `CTP0068` as its newest entry, and `k_GAME_MAGIC_VALUE` must name it, so an engine without this change rejects a new file instead of misreading it. Version 67 stays reserved for `USE_FORMAT_67`, which also changes the sprite-group format and is not coupled here.
- Map files (`GameMapFile`) follow the same rule: files written by this build carry the header `CTPMAP68`; files with the earlier header `CTPMAP__` must still load and are read as version 66. An engine without this change must not list or load a `CTPMAP68` file.
- Any save-format code that copies a structure or a range of members containing a `MapPoint` as raw memory must be split or rewritten so that a file written before the plane format loads byte for byte as before. After a `CTP0066` load, the archive position must equal the archive length.
- Network and multiplayer behaviour are out of scope. The in-memory archive layout changes with the new member; peers must run the same build.
- Out of scope for this slice: a second cell grid, per-plane vision, per-plane unit trees, rendering or minimap selection of a plane, plane-aware pathfinding, Towers and Planar Travel. Later slices own them.

***test reqs***

- Build both executables from the same source: Release `ctp2.exe` (Final-SDL) and Debug `ctp2-dbg.exe` (Debug-SDL). Both must compile and link with zero error lines.
- Equality: a unit test (or a logged assertion run in the Debug build) must show `MapPoint(3, 4, 0) == MapPoint(3, 4)`, `MapPoint(3, 4, 1) != MapPoint(3, 4, 0)`, and `MapPointData` behaves the same.
- Plane preservation: for centres on plane 0 and plane 1, `GetNeighborPosition` in all eight directions, a `RadiusIterator` of radius 2, and `OrthogonalPoint` round trips must return only positions on the centre's plane, on at least three centres including one that wraps the map edge.
- Round trip: `MapPoint::Serialize` stores then loads x, y and z unchanged for three varied points (plane 0, plane 1, negative `MAP_POINT_UNKNOWN`).
- Stream consumption: after a `CTP0066` load, the number of archive bytes read must equal the number stored (temporary probe at the end of `GameFile::Restore`; a mismatch means a raw-layout site was missed).
- Legacy load: load a real `CTP0066` save (for example an autosave of a Masters of Magic game) from the main-menu Load Game screen in the new Release build; require no load error, and require the same army and city positions in the engine log before and after as the baseline build shows for the same save.
- New-format round trip: save the loaded game (the header must read `CTP0068`), load it again, require that the archive position equals the archive length, and require the same per-player unit counts as the legacy load.
- Rejection: an engine built before this change must refuse the new file with its "not a savegame file" error rather than load it. Record the exact message.
- Neutrality: from the same `CTP0066` save, end three turns in the baseline pair and in the new pair at 800x600; compare the logged army orders and positions. Any difference must be explained before the slice is accepted.

***functional specs***

- A map position should know which plane it is on without changing any single-plane game.
  - Given code that builds a position from two coordinates, When it runs, Then the position is on plane 0 and compares equal to the same x and y built earlier.
  - Given two positions with the same x and y and different planes, When they are compared, Then they are not equal.
  - Given a position on plane 1, When the engine asks for a neighbour or iterates a radius, Then every position returned is on plane 1.
- A save should record the plane and an old save should still load.
  - Given a game saved by this build, When the file header is read, Then it is `CTP0068` and every stored position carries its plane.
  - Given a `CTP0066` save, When this build loads it, Then every position is on plane 0 and the game loads without a prompt beyond the existing older-version SLIC reload.
  - Given a `CTP0068` save, When an older build opens it, Then the older build refuses the file.
