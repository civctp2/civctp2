version 0.02

The "Apolyton Edition Mod" is the combined efforts of the posters at the Apolyton CtP2 sourcecode forum (http://www.apolyton.net/forums/forumdisplay.php?s=&forumid=213) to rebalance the game and improve the AI, while keeping as close to the original gameplay of CtP2 as possible.

AE Mod is based on the orignal Super Apolyton Pack 2.0 mod with some other tweaks mainly to wonders, and with all the improvements from the Apolyton source code project.

You can post any comments regarding this mod on the forum thread here: http://www.apolyton.net/forums/showthread.php?s=&threadid=165870

REQUIREMENTS:
=============

Call to Power 2
"Apolyton Edition" (AE) patch

INSTALLATION:
=============

Extract the "AE_Mod" folder to your CtP2 "Scenarios" folder.

GENERAL CHANGES:
================

modified turn lengths to match scientific timeline better.
diplomacy has been made slightly harder for human-to-AI, and slightly easier between AI's.
AI now builds more commerce improvements and farms, and no mines on flat land.
trade is slightly more profitable and less caravans are required, like SAP2.

ADVANCES:
=========

changed:

Fascism - requires Nationalism + Mass Media (was Gunpowder + Theology)
Nationalism - removed Fascism as prerequisite

BUILDINGS:
==========

changed:

aqua filter - maxpop +14 > maxpop +6
hospital - maxpop +0 > maxpop +8
forcefield - now does not require city walls to build

---
the following are the maximum city sizes where there is NO overcrowding effects:

no buildings = size 4
with aqueduct +6
with drug store +4
with hospital (requires drug store) +8
with arcologies (requires aqueduct) +8
with aqua filter (requires aqueduct and arcologies) +6
with incubation center (requires drug store and hospital) +12
with body exchange (requires drug store, hospital and incubation center) +12
*** with all above buildings the maximum size with no overcrowding is 60.

the following are the maximum sizes for cities, this is where growth STOPS completely, with or without overcrowding:

no buildings = size 18
with aqueduct +14
with hospital (requires drug store) +8
with arcologies (requires aqueduct) +14
with aqua filter (requires aqueduct and arcologies) +6
*** with all above buildings maximum city size is 60.
---	

TERRAIN:
========

type  : food/production/commerce - terraform add/remove advance > changed to

for example, to terraform a terrain TO forest it now requires conservation (was agricultural revolution).
and to REMOVE a forest it now requires toolmaking (was agricultural revolution).

forest: 10/5/10 - agri.rev./agr.rev. > 5/15/5 - conservation/toolmaking
plains: toolmaking/toolmaking > toolmaking/agri.rev.
tundra: adv.composites/adv.comp > NONE/adv.composites
glacie: adv.composites/adv.comp > NONE/adv.composites
grassl: 15/5/5 - toolmaking/toolm. > 15/7/5 - agri.rev./agri.rev.
desert: agri.rev./agri.rev. > NONE/agri.rev.
swamp : industrial rev./indus.rev. > NONE/agri.rev.
jungle: agri.rev./agri.rev. > conservation/toolmaking
mounta: fusion/fusion > NONE/Fusion
hill  : explosives/explos. > NONE/explosives

d.moun: fusion/fusion > NONE/fusion
dunes : explosives/explos. > NONE/explosives
p.hill: explosives/explos. > NONE/explosives
p.moun: fusion/fusion > NONE/fusion

UNITS:
======

att/def/ran/damage/armour - moves/vision(cost/support) - advance - catagory

changed:

fascist - 25/35/15 > 35/40/20
mobile SAM - bombards land, sea and air (not just air).
sea engineer - settle size 1 > 3, now creates the buildings in a new city (like urban planner).
Televangelist - can be built by any government (before was just in Theocracy, and never got built).

Upgrade paths:
--------------

Land:

- Knight > Cavalry > Tank > Fusion Tank
- Hoplite > Pikemen > Infantryman > Machine Gunner > Marine > Hover Infantry
- Archer > Mounted Archer > Cavalry
- Catapult > Cannon > Artillery > Mobile SAM > War Walker
- Fascist > Marine > Hover Infantry
- Paratrooper > Hover Infantry

Sea:

- Coracle > Longship > Carrack > Troop Ship
- Fire Trireme > Ship of the Line > Ironclad > Battleship > Plasma Destroyer
- Destroyer > Plasma Destroyer
- Sub > Nuclear Sub > Morey Striker
- Kraken > Dreadnaught

Air:

- Bomber > Stealth Bomber
- Fighter > Interceptor > Stealth Fighter

Special:

- Cleric > Televangelist
- Diplomat > Empathic Diplomat
- Spy > Cyber Ninja

No upgrade:

Abolitionist
Aircraft Carrier
Cargo Helicopter
Corporate Branch
Crawler
Cruise Missile
Eco Ranger
Eco Terrorist
Infector
Lawyer
Leviathan
Nuke
PT Boat
Samurai
Scout Sub
Sea Engineer
Settler
Slaver
Space Plane
Spy Plane
Urban Planner
Warrior

WONDERS:
========

Aristotle's Lyceum - does not obsolete > obsolete at Classical Education.
Central Matter Decompiler - "building everywhere"(Matter Decompiler), costs 20,000 > costs 30,000. (still needs better fix)
Chichen Itza - does not obsolete > obsolete at Theology
Field Dynamics Lab - +35% science > +10% science
Genome Project - +10% prod., +10% health (+1HP) for units. > only +10% health for units.
Globe Sat - costs 12,400, does not obsolete > costs 18,600, obsolete at Nano Machines.
Gutenberg's Bible - +2 happiness, eliminates conversion > only gives "Eliminates conversion"
Internet - "building everywhere" (Computer Center) > +20% science, obsolete at Space Flight(Orbital Laboratories)
Penicillin - does not obsolete > obsolete at Gene Therapy.
Ramayana - +3 happiness > +1 happiness, obsolete Age of Reason
The Agency - does not obsolete > obsolete at Neural Interface (Cyber Ninjas).
The Appian Way - obsolete at Age of Reason > obsolete at Railroad
The Eden Project - destroy 3 most polluting cities > destroy only 1.
World Peace Center - added increase regard (+50%), as it says in GL. ("embassies even in war" may still be overpowered though)
Zero Crime Bill - -60% crime > -30% crime, obsolete at Neural Reprogramming
