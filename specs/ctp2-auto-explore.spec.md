---
description: 'Executable-level auto-explore orders for CTP2 units'

***definitions***

- :Auto-Explore: is a persistent unit order that selects safe movement to reveal useful unknown map tiles and ends when no safe useful route remains.
- :Exploration-Value: is the amount of currently unknown map area a unit could reveal from a reachable destination using its normal vision range.
- :Threat-Posture: is movement to a reachable safe tile with the strongest available terrain defense when a hostile unit is visible nearby.

***implementation reqs***

- The behavior must execute in the compiled game executable and use native unit orders, visibility, pathfinding, movement costs, diplomacy, and terrain defense data.
- SLIC scripts and scenario data must not implement or be required to enable this behavior.
- The implementation must use only map and unit information available to the acting player; it must not inspect hidden enemies or unrevealed terrain.
- The order must be registered as a first-class remappable keyboard action in the executable key-function table, with lowercase `i` as its initial default in `ctp2_data/default/uidata/keymap.txt`; uppercase `I` remains `INVESTIGATE_CITY`.
- The player must also be able to start Auto-Explore from the selected army's Army Manager context menu; both entry points must enqueue the same native unit order.
- The default binding must not reuse `A` or `a`: the installed keymap assigns uppercase `A` to `ENDTURN` and lowercase `a` to `CENTER_MAP`; bindings are case-sensitive.
- The order must be remappable through the existing keyboard remapping screen (`K`) and its binding must persist in `ctp2_program/ctp/userkeymap.txt`.
- When an existing `userkeymap.txt` lacks the new action and lowercase `i` is unassigned, the executable must add the lowercase `i` default without changing the player's other bindings. If the player already assigned lowercase `i`, the executable must preserve that binding and expose Auto-Explore as unbound in the `K` remapping screen.
- When the executable reaches the end of an existing `userkeymap.txt`, it must complete keymap migration before returning from keymap initialization.
- Reference: `H:\\freeciv\\server\\advisors\\autoexplorer.c` (`player_may_explore`, `explorer_desirable`, `manage_auto_explorer`) and `H:\\freeciv\\common\\movement.c` (path movement-cost adjustments), inspected 2026-11-14.
- CTP2 keymap references: `ctp2_code/ui/aui_ctp2/keymap.cpp` (`func2key`, `load`, `save`); installed defaults in `ctp2_data/default/uidata/keymap.txt`; player bindings in `ctp2_program/ctp/userkeymap.txt`.
- The Freeciv source is a behavioral reference only; CTP2 path, diplomacy, and defense APIs must be verified against its C++ source before implementation.

***test reqs***

- Exercise at least three varied maps per behavior: open low-cost terrain, road-connected routes, and terrain with a visible hostile contact; include a case where high ground is impassable.
- Exercise legacy keymaps with lowercase `i` unassigned and occupied; verify the default is installed only when free, existing bindings remain intact, and Auto-Explore remains remappable.
- Record chosen destinations, traversed route, revealed tiles, and stop reason so target selection can be compared against the contract.

***functional specs***

- A player should be able to start auto-explore for an eligible selected unit through a native executable order.
  - Given an eligible unit, When the player starts auto-explore, Then the unit receives an auto-explore order without loading or invoking SLIC.
  - Given the installed default keymap, When the player invokes lowercase `i`, Then the executable starts auto-explore and uppercase `I` continues to investigate a city.
  - Given an eligible selected army, When the player chooses Auto-Explore from the Army Manager context menu, Then the executable starts the same native order as lowercase `i`.
  - Given a player with an older `userkeymap.txt` where lowercase `i` is unassigned, When the executable loads the new feature, Then Auto-Explore receives the lowercase `i` default and all existing bindings remain unchanged.
  - Given a player with an older `userkeymap.txt` where lowercase `i` is already assigned, When the executable loads the new feature, Then the saved assignment remains unchanged and Auto-Explore is available to remap through `K`.
  - Given the player remaps auto-explore through `K`, When the executable saves and reloads key bindings, Then the selected binding remains assigned to auto-explore.
- The auto-explorer must reject routes through foreign cities, foreign territory, and tiles occupied by units the owner may not safely enter.
  - Given a route whose only path crosses a rejected tile, When the unit evaluates destinations, Then it must not select or traverse that route.
- While no hostile unit is visible within one normal movement allowance, the auto-explorer should select a reachable destination with high :Exploration-Value: relative to total native movement cost, calculated as unknown tiles revealed divided by one plus path cost.
  - Given two safe destinations where one reveals more unknown tiles for its path cost, When the unit chooses a destination, Then it should choose the destination with the higher information value per movement cost.
  - Given equal information value per movement cost, When the unit chooses a destination, Then it should prefer lower path cost, then higher native terrain defense, then a stable coordinate order.
- The auto-explorer must calculate route cost with the unit's normal movement rules, including road discounts where the engine grants them.
  - Given equivalent safe routes and one route has usable roads, When the unit selects its route, Then it should use the lower-cost road route.
- While a hostile unit is visible within one normal movement allowance, the auto-explorer must pause exploration and choose a reachable safe :Threat-Posture: destination with the highest applicable terrain defense value.
  - Given a reachable defensible ridge or mountain and an unsafe low-ground alternative, When a hostile unit is visible, Then the unit must prefer the defensible tile and must not enter a tile occupied by that hostile unit.
  - If the highest-defense terrain is impassable, the unit must choose the best reachable defensive tile under its native movement rules.
- When the player issues another order, the unit becomes unable to move, or no safe unexplored destination remains, the auto-explore order must stop without replacing the player's order or issuing an unsafe move.
- The auto-explorer must recompute safety, target value, and route after each movement segment and after newly visible units or terrain change the known map.
- If an unknown tile blocks a planned route or native movement rejects the next step, the auto-explorer must recompute from the unit's actual location and stop if no safe route remains.
- Given unknown terrain beyond the unit's current visibility, When the auto-explorer ranks targets, Then it must base its decision only on information the owner currently knows.
