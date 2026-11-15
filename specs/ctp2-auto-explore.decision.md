# ADR-001: Implement Auto-Explore as a Native CTP2 Unit Order

**Status:** Proposed
**Date:** 2026-11-14
**Decision owner:** AI, based on the user's stated feature intent

## Context

The requested behavior must be available to the player as an ongoing unit order. It depends on CTP2's movement costs, road rules, ownership and visibility boundaries, terrain defense, and unit-order lifecycle. The scenario SLIC layer can react to events, but it is not the appropriate owner of native movement planning or persistent unit-order execution.

The installed default keymap is case-sensitive: uppercase `A` is `ENDTURN`, lowercase `a` is `CENTER_MAP`, and uppercase `I` is `INVESTIGATE_CITY`. Lowercase `i` is currently unused.

## Decision

Implement Auto-Explore in the compiled game executable as a first-class, remappable unit order. Use lowercase `i` as the initial default binding, preserving uppercase `I` and all existing player bindings. Use only information known to the acting player. Use native movement and road costs, reject unsafe foreign or occupied tiles, rank exploration by newly visible unknown area per path cost, and switch to the strongest reachable safe defensive terrain when a hostile is visible nearby.

SLIC and scenario data are not required to enable or execute this behavior.

## Consequences

- The C++ implementation must integrate with the executable's key-function registration, keymap migration, unit-order lifecycle, save/load behavior, pathfinding, visibility, diplomacy, and terrain-defense interfaces.
- The default binding must remain distinct by case; keymap tests must prove lowercase `i` and uppercase `I` retain separate actions.
- The exact source classes and APIs remain an implementation discovery task; the design documents specify the behavior and conceptual data, not verified C++ class names.

## AI-Context

- **Constraints:** CTP2 executable feature; no SLIC dependency; preserve existing bindings; avoid hidden-information reads; use native movement and safety rules.
- **Patterns:** Extend existing key-function and unit-order extension points; re-evaluate after each movement segment or material visibility change.
- **Anti-patterns:** Do not implement the planner as scenario script; do not hard-code terrain costs or reveal hidden threats; do not overwrite a replacement player order.
- **Verification commands:** Validate the use-case documents; validate EARS requirements; advance the typed `sw-design` workflow only with persisted, hashed artifacts; test the eventual executable on open terrain, road routes, visible-hostile terrain, and an impassable high-ground case.
