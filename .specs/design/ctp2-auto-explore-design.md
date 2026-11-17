# CTP2 Auto-Explore Design

**Status:** Draft
**Decision:** [ADR-001](../../specs/ctp2-auto-explore.decision.md)
**Requirements:** [AE-REQ-001–AE-REQ-006](../reqs/ctp2-auto-explore.rst)
**Source specification:** [ctp2-auto-explore.spec.md](../../specs/ctp2-auto-explore.spec.md)

## Overview

Auto-Explore is a native unit-order behavior. The order controller asks the planner for one legal movement segment, executes it through CTP2's existing movement rules, then reevaluates with the player's current map knowledge. The order ends when the player replaces it, the unit cannot safely move, or no useful safe destination remains.

The component names below describe proposed responsibilities. They are not claims about existing C++ class names or file ownership.

## Components and Responsibilities

| Component | Responsibility | Inputs | Output |
|---|---|---|---|
| Command adapter | Expose the order under lowercase `i`, the existing remapping screen, and the Army Manager context menu; preserve custom bindings and route both commands to one order request | Key event or selected-army menu action, current keymap | Auto-Explore order request |
| Order controller | Start, continue, and stop the unit order; yield to a replacement player order | Unit state, order state, owner turn | One movement-segment request or stop reason |
| Known-map view | Supply only the acting player's currently known tiles, visible units, ownership, and improvements | Player visibility state | Owner-scoped map facts |
| Route-cost provider | Find reachable paths using unit movement type, native terrain costs, road discounts, and entry restrictions | Unit, origin, candidate tile | Legal path and native movement cost |
| Destination planner | Compare exploration value or defensive posture and select a stable best candidate | Known-map view, legal paths | Destination and route |
| Movement executor | Apply the selected legal segment using native unit movement | Unit, selected route | Actual unit position and newly revealed information |

## Planning Contract

For a safe exploration candidate:

`exploration_score = newly_revealed_unknown_tiles / (1 + native_path_cost)`

Choose the candidate with the highest score. Break equal scores by lower path cost, higher native terrain defense, then stable map-coordinate order. If a hostile unit is visible within one normal movement allowance, suspend information-seeking selection and choose the reachable safe tile with the highest applicable defense. Break equal defense by lower path cost and stable coordinate order.

Foreign cities, foreign territory, hostile-occupied tiles, and any tile the unit cannot safely enter are excluded before destination ranking. A road is preferred only when CTP2's native movement rules make its route cheaper. After each movement segment, new visibility, a blocked route, a changed unit order, or a movement rejection causes replanning from the actual unit position. A player-issued replacement order always wins.

## Interfaces to Confirm in C++ Source

- Key-function registration, default keymap loading, remapping, and old `userkeymap.txt` migration.
- Existing unit-order representation, per-owner-turn scheduling, cancellation, and save/load behavior.
- Owner-visible map and hostile-unit queries; no hidden-map reads are allowed.
- Native pathfinding, road cost, foreign entry restrictions, terrain defense, and movement-segment execution.

The installed keymap uses case-sensitive bindings. Lowercase `i` is unassigned; uppercase `I` remains `INVESTIGATE_CITY`. The `KEY_FUNCTION`/`func2key` tables and `km_screen` Unit-tab list provide remapping; the Army Manager menu and keyboard action enqueue the same native unit order. Keymap initialization must run its legacy-binding migration after the saved file reaches EOF. `ArmyData::ExecuteOrders` advances the persistent order, `UnitAstar::FindPath` supplies owner-scoped native routes, and `GEV_CheckOrders` re-enters after each movement event. The planner examines known candidate tiles within `vision range + maximum movement + 1`, counts unknown tiles in the unit vision radius, and chooses one candidate path each time the order is advanced. Under threat it scans currently visible enemies within the stack movement allowance and considers only defensive destinations reachable within one maximum movement allowance. If the current tile is the strongest reachable defensive posture, the order stays active and holds position until the next order evaluation. A legacy keymap gets lowercase `i` only if that exact case-sensitive key is free; a conflicting saved binding is retained and the new action remains remappable.

`g_orderInfo` is positional metadata used by engine diagnostics. Keep one row per enum slot through `UNIT_ORDER_AUTO_EXPLORE`; the pre-existing `UNIT_ORDER_UPGRADE` entry must remain at slot 63 so Auto-Explore occupies slot 64.

When an Auto-Explore path reaches its local endpoint, `ArmyData::IncrementOrderPath` must preserve the head `UNIT_ORDER_AUTO_EXPLORE`; the already queued `GEV_CheckOrders` then re-enters `ExecuteOrders`, which chooses the next target. Ordinary GOTO orders still complete and are removed at their path endpoint. If movement is exhausted first, the same Auto-Explore order remains queued. `Player::ResetAllMovement` refreshes movement before `Player::BeginTurnUnits` queues each `GEV_BeginTurnArmy`; after the army begin-turn events, `GEV_BeginTurnExecute` re-enters the retained human Auto-Explore order. Do not replace the order with `UNIT_ORDER_MOVE`, `UNIT_ORDER_MOVE_TO`, or a second persistence mechanism. A player-issued replacement order continues to clear/replace it through the existing order API.

## Not in This Design

- Scenario-specific SLIC scripts or data-driven triggers.
- A new movement-cost table or a duplicate pathfinding implementation.
- A commitment to new save-file fields; order persistence should use the existing unit-order save path if source inspection confirms it supports the new order.

## Validation Plan

Use three bounded map fixtures: open terrain with unequal reveal value, a safe road route that is cheaper than an unroaded alternative, and a visible-hostile case with reachable and impassable high-ground candidates. For each, record selected target, route cost, movement segment, reveal result, and stop/replan reason. Confirm the player's order replaces Auto-Explore without a later planner step overwriting it.
