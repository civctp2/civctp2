Auto-Explore Requirements
========================

The CTP2 executable requirements below derive from :doc:`../../specs/ctp2-auto-explore.spec`
and :doc:`../../specs/ctp2-auto-explore.decision`.

.. req:: Start a native Auto-Explore order
   :id: AE-REQ-001
   :status: open
   :tags: functional, unit-order

   When a player invokes Auto-Explore for an eligible selected unit, the system shall start a first-class executable unit order without invoking SLIC.

.. req:: Preserve remapped bindings
   :id: AE-REQ-002
   :status: open
   :tags: functional, keymap

   When the executable loads an existing player keymap that lacks Auto-Explore, the system shall bind Auto-Explore to lowercase ``i`` if it is unassigned, or preserve the existing ``i`` assignment and expose Auto-Explore for remapping through ``K`` if it is assigned.

.. req:: Choose safe exploration targets by value per cost
   :id: AE-REQ-003
   :status: open
   :tags: functional, pathfinding

   While no hostile unit is visible within one normal movement allowance, the system shall choose a reachable safe target maximizing newly revealed unknown tiles divided by one plus native path movement cost.

.. req:: Prefer roads under native movement rules
   :id: AE-REQ-004
   :status: open
   :tags: functional, pathfinding

   When a safe road route has lower native movement cost than an equivalent route, the system shall use the lower-cost route.

.. req:: Take a safe defensive posture near visible hostiles
   :id: AE-REQ-005
   :status: open
   :tags: functional, safety

   While a hostile unit is visible within one normal movement allowance, the system shall choose the reachable safe tile with the highest applicable terrain defense from candidates excluding unsafe occupied or foreign tiles.

.. req:: Replan or stop without overriding the player
   :id: AE-REQ-006
   :status: open
   :tags: functional, lifecycle

   When a route becomes unsafe, blocked, or no longer useful, the system shall replan from the unit's actual location or stop the order without replacing a newer player order.

.. req:: Continue Auto-Explore across turns
   :id: AE-REQ-007
   :status: open
   :tags: functional, lifecycle, turn-processing

   While safe useful destinations remain, when an Auto-Explore path ends or the unit exhausts its movement for the turn, the system shall retain that same native order and re-evaluate it at the path endpoint or automatically after movement is restored on the unit's next turn.

.. req:: Start Auto-Explore from the Army Manager
   :id: AE-REQ-007
   :status: open
   :tags: functional, unit-order, user-interface

   When a player selects Auto-Explore from the Army Manager context menu for an eligible selected army, the system shall enqueue the same native unit order used by the Auto-Explore keyboard action.
