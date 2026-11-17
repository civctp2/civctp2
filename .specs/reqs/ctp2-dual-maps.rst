Dual Maps Requirements: plane coordinate
=======================================

The CTP2 executable requirements below derive from :doc:`../../specs/ctp2-dual-maps.spec`
and :doc:`../../specs/ctp2-dual-maps.decision`. Governing basis: the Dual Maps feature (two persistent map planes);
no earlier requirement exists.

.. req:: Carry a plane in every map position
   :id: DM-ENG-REQ-001
   :status: open
   :tags: functional, map

   The system shall carry a plane index ``z`` (0 or 1) in every ``MapPoint`` and ``MapPointData``, defaulting to 0 whenever a position is built from two coordinates.

.. req:: Distinguish planes in position equality
   :id: DM-ENG-REQ-002
   :status: open
   :tags: functional, map

   When two positions have the same x and y but different planes, the system shall report them as not equal.

.. req:: Keep derived positions on the source plane
   :id: DM-ENG-REQ-003
   :status: open
   :tags: functional, map

   When the system derives a position from another position by neighbour lookup, orthogonal conversion, square or radius iteration, coordinate conversion, or addition or subtraction, the system shall return a position on the source position's plane.

.. req:: Keep the eight map directions
   :id: DM-ENG-REQ-004
   :status: open
   :tags: constraint, map

   The system shall keep ``_SMALL_MAPPOINTS`` defined and shall not add vertical directions to ``WORLD_DIRECTION``.

.. req:: Save and read the plane
   :id: DM-ENG-REQ-005
   :status: open
   :tags: functional, save

   When the system stores a position it shall write x, y and z; when it loads a position from a file whose version is below 68 it shall read x and y and set z to 0; when it loads from an in-memory archive or a file of version 68 or above it shall read x, y and z.

.. req:: Reject new saves in older engines
   :id: DM-ENG-REQ-006
   :status: open
   :tags: constraint, save

   The system shall write save files with the magic value ``CTP0068`` so that an engine without this change refuses them, and shall read every field of a ``CTP0068`` file with the same layout it wrote: no load gate may assume that a version above 66 carries the unused wide sprite-index format or omits the happiness array.

.. req:: Keep raw-copied save structures readable
   :id: DM-ENG-REQ-008
   :status: open
   :tags: constraint, save

   When the system loads a save file of a version below 68, it shall read every structure that embeds a ``MapPoint`` with the layout that version wrote and shall consume exactly the number of bytes the file holds.

.. req:: Leave single-plane play unchanged
   :id: DM-ENG-REQ-007
   :status: open
   :tags: constraint, regression

   While every position is on plane 0, the system shall produce the same logged army orders and positions as the build before this change when both start from the same ``CTP0066`` save.
