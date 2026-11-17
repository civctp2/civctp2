# Use Case: Advance an Auto-Explore Order

## Overview

**Use Case ID:** UC-006
**Use Case Name:** Advance an Auto-Explore Order
**Primary Actor:** Game Turn Processor
**Goal:** Move one active Auto-Explore unit by a legal segment toward a valuable safe destination, or stop it cleanly.
**Trigger:** The unit's owner turn begins while its Auto-Explore order is active.
**Status:** Draft
**Type:** System

**Requirements:** [AE-REQ-003, AE-REQ-004, AE-REQ-005, AE-REQ-006](../reqs/ctp2-auto-explore.rst)

## Preconditions

- The unit carries an active Auto-Explore order and belongs to the player whose turn is being processed.
- Current visibility, unit movement rules, terrain, roads, and entry restrictions can be queried through native game state.

## Main Success Scenario

1. The Game Turn Processor asks the active order to advance the unit.
2. The game reads only the owner's currently known terrain, visible units, territory, and improvements.
3. The game checks for a visible hostile within one normal movement allowance.
4. The game computes reachable safe destinations and their native movement costs.
5. The game scores each exploration destination as newly revealed unknown tiles divided by one plus native path cost.
6. The game selects the highest-scoring destination, breaking ties by lower path cost, higher terrain defense, then stable map-coordinate order.
7. The game moves the unit along the selected route by one legal movement segment, applying native road discounts where available.
8. The game reevaluates after movement or new visibility before issuing another segment.

## Alternative Flows

### A1: A visible hostile requires a defensive posture

**Trigger:** A hostile is visible within one normal movement allowance (step 3)
**Flow:**

1. The game excludes unsafe tiles, hostile-occupied tiles, and tiles that violate foreign-entry rules.
2. The game chooses the reachable safe destination with the highest applicable defense; equal defense prefers lower path cost, then stable coordinate order.
3. The use case continues at step 7.

### A2: The highest-defense terrain is unreachable

**Trigger:** The highest-defense candidate is impassable to this unit (step 4)
**Flow:**

1. The game selects the next highest-defense safe reachable candidate.
2. The use case continues at step 7.

### A3: No safe candidate remains

**Trigger:** No reachable destination passes safety and usefulness checks (step 4 or 5)
**Flow:**

1. The game ends the order and records a stop reason.
2. The use case ends without moving the unit.

### A4: The route changes after planning

**Trigger:** A new visible obstacle or a native movement rejection invalidates the next route step (step 7)
**Flow:**

1. The game discards the stale route and recalculates from the unit's actual location.
2. If a safe route exists, the use case continues at step 3; otherwise, the use case ends as in A3.

### A5: The player issues a replacement order

**Trigger:** The active unit order is replaced before the next segment (step 1 or 8)
**Flow:**

1. The game ends Auto-Explore without changing the replacement order.
2. The use case ends.

## Postconditions

### Success Postconditions

- The unit has moved by one legal segment, adopted a safe defensive posture, or stopped with a reason.
- Any route retained for a later segment is recalculated after material visibility or path changes.

### Failure Postconditions

- No movement occurs through a rejected foreign tile, an unsafe occupied tile, or a route that native movement rules reject.
- No player-issued replacement order is overwritten.

## Business Rules

### BR-001: Exploration value

Rank safe reachable targets by newly revealed unknown tiles divided by one plus native path movement cost.

### BR-002: Threat posture

When a hostile is visible within one normal movement allowance, choose the reachable safe tile with the highest applicable terrain defense instead of an exploration target.

### BR-003: Native route costs

The route cost comes from the unit's ordinary movement rules, including roads only where the engine grants their discount.
