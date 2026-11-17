# Current interpretations

## Auto-Explore persists across route endpoints and turns — 2026-11-16

- **Decision:** Keep the same `UNIT_ORDER_AUTO_EXPLORE` order when its selected local path ends, and let the already queued `GEV_CheckOrders` re-evaluate it. When movement is exhausted first, keep the order queued until `GEV_BeginTurnExecute` resumes it after the movement refresh at the start of the owner's next turn. Ordinary GOTO orders still complete and leave the queue at their path endpoint.
- **Basis:** `MoveUnitsEvent` queues `GEV_CheckOrders`, moves the army, then calls `ArmyData::IncrementOrderPath`, which used to delete every head order whose path reached its end. An Auto-Explore target is always a local path endpoint, so the order vanished before it could pick the next target or resume on the next turn. An earlier resume-only hypothesis was wrong; path-end deletion was the first broken link.
- **Decider:** AI, following the instruction to keep Auto-Explore as the persistent native order.

## Preserve an existing lowercase-i assignment

- **Decision:** Add Auto-Explore to an older saved keymap on lowercase `i` only when `i` is unassigned; if the player already bound `i`, preserve that action and leave Auto-Explore available to remap through `K`.
- **Basis:** The keymap maps one action per case-sensitive key. The installed keymap maps uppercase `I` to `INVESTIGATE_CITY` and leaves lowercase `i` free.
- **Decider:** AI, following the explicit no-binding-loss requirement and existing keymap data structure.

## Complete legacy migration after reading the saved keymap

- **Decision:** Reach the fallback binding migration after normal end-of-file, so old `userkeymap.txt` files receive Auto-Explore when lowercase `i` is unassigned.
- **Basis:** The original constructor returned on `fgets` EOF before reaching the new-action migration block; the user's saved keymap lacks `AUTO_EXPLORE` and lowercase `i`.
- **Decider:** AI, based on source inspection and the reported keypress failure.
