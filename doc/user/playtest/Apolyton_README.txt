THIS MATERIAL IS NOT MADE OR SUPPORTED BY ACTIVISION.
This version of the Call To Power 2 executable developed at apolyton.net

Contact the authors on the Apolyton forums at
http://apolyton.net/forums/forumdisplay.php?s=&forumid=213
or at ctp2source@apolyton.net


This is the current state of the source code project released for playtesting.

Playtesters should be aware that these files are intended for a plain vanilla 
install of CTP2. It should also be no problem to install over a patched
version of CTP2, or over a previous playtest version.

In theory this version is now compatible with most of the mods, but special
actions may be required to make them work properly.  Please visit the
thread mentioned below and post queries if you desire further information.

Installation instructions:
unzip into your CTP2 dir and play.  If you get warnings about overwriting
existing files then you have unzipped to the correct location.
It might be wise, if you have the space available, to make a copy of
your CTP dir and unzip this into the copy, so that you can preserve an
unchanged copy for playing mods, etc.

Please report any problems, bugs, crashes, etc. in the following threads:
http://apolyton.net/forums/showthread.php?s=&threadid=147969

Playtesting would be most useful if it tests the most recent changes, which
are described briefly below, and in more detail in ChangeLog.txt



Changelog:
2006-03-22
Added: production bonus to AI, they dont run production deficits
Added: GoodyHutExcluded flag (just in case) for advances
Added: messages for verifyattack and move into enemy. If you move into an enemy or attack WIHOUT declaring war you will get a pop, but the pop doesnt let you declare war (yet) you have to do that through the diplomacy screen.
Added: newupkeep flags,although not necessary (may remove)
Added: gold bonus to AI so they dont have to be hurt by negative values. If a cities gold is lss than zero for an AI it is automatically set to zero so they dont get hurt by new gold modifiers

2006-03-11
Added: GoodyHutExcluded flag for Units. Units with this flag shouldn't appear from a goody hut.
Added: Enablesgood in terraineffect - allows for different goods for same imp on different terrains

2006-03-02
Added: EnablesVeterans flag for buildings. Now if a city has a flag with this building your units start as veterans (like civ3)
Added: PopCostsToBuild flag. Add an integer here in unitstxt and units can cost 1 or more population points. I havent instituted a disband city warning yet though

2006-03-01
Added:  GoldPerUnit - like my previous GoldPerCity flag but its just the flag value times the number of units (like Civ3). I stuck it to the palace to represent unit costs. Value seen in City Management (not national)
Added: GoldPerUnitReadiness - same as above but added the readiness impact. Value seen in City Management (not national)
Added:  Need Number of buildings to Build - like the feat it can now be a flag for building a building (especially with OnePerCiv) or a wonder.
Added: Citystyle Bonuses to Gold, Food, and production by either a bonus (just a solid integer) or by a percentage. 
Added: Increase Borders to AddImprovement. Buildings can now do city radius!
Added: multipleattack flag but not fully implemented (you can move after attacking or bombarding but not attack again)
Added: TotalBuildingUpkeep: building upkeep also has a negative impact by being affected by your readiness and is deducted from the city gold (I can remove this if too much hate for it but it makes gold and readiness more significant)


2006-02-15
Updated: BUGS list to reflect fixes and new discoveries
Updated: Improved error messages in tileset.cpp
Updated: zlib (1.2.3) and tiff (3.7.3) libraries
Fixed:Improved dbgen error message
Fixed:Corrected turn box computation for ship paths through cities.
Fixed:1.1 patch savegame compatibility fix
Fixed: player color above player 16 and allow loading of the original color set again. Now there are only two valid number of colors for a ColorsXX.txt 58 and 74 - original and source code version respectively. For the 58 color version the missing player colors are copied from the terrain colors so that the result is like in the original game
Fixed: mistake in tileset.cpp which prevented loading of tile file
Fixed: mis-cast in UnitControlPanel.cpp
Fixed: Prevent useless Assert pop-up (Governor.cpp) with the debug version
Fixed: Rreport start of wonder when deleting the first item and the second item is a wonder
Fixed: Removed texts that have been replaced with icons
Fixed: Removed after redistribution of contents
Fixed: Corrected building maintenance deficit spending handling.
Fixed: Load strings from default files if a mod misses them
Fixed: Added token so that the default strings aren't scanned for missing strings
Fixed, added some strings and removed some duplicated strings
Fixed: corrected endless path computation in an AOM game
Added: NeedsCityGood, NeedsCityGoodAll, and NeedsCityGoodAny flags
Added:Redesigned ColorSet to prevent a crash during start-up.
Added: to Civilisation and CityStyle databases happiness increasement flags
Added: Message boxes are now non-modal, left click on a message in the message list, the message is opened if it is not open, otherwise it is closed, right click removes the message from list, if it is open it is closed in addition. Left clicking on another message opens that and closes the first message. Alert boxes are left as they are modal.
Added: option to select a city first instead of an active army.
Added: DebugSlic and GoodAnim to the advanced options.
Added:Used option to select a city first instead of an active army. Unfortunately this doesn't quite work. But for now this should be enough.
Added:Made the autoDebug object more conform to the standart, maybe I move its struct to seperate header and source files
Added: Improved handling of multiple build queue actions in a turn.
Added:Display very slow or non-growth as ---
Added: NeedsCityGood and NeedsCityGoodAll added to CanBuildUnit, CanBuildBuilding, and CanBuild Wonder
Added:Implemented EnablesGood for buildings, wonders and tile improvements now they give goods to a city 
Added:To ProcessFood,ProcessProduction, EfficiencyOrCrime added a check that if a city has or is buying a good than you can get a bonus.   
Added: NeedsCityGoodAnyCity allows for a unit to be built in any city if one city has the good
Added: Sneakattack - can attack with out declaring war or be attacked, like Hidden Nationality
Added: Sneakbombard so you can bombard without war declared
Added: new combat modifiers based on mounted and woodship bonus (see unit.cdb for flags) 
Added: new DifficultyDB to the project however, the DifficultyDB must still be replaced
Added: two new options to the profile database:
- One for allowing end turns while the game is running in the background
- One that is supposed to stop end turns if there is a city with empty build queue, doesn't work yet.
Added: Reneralised production start reporting
Added: Settlers can now be added to a city! 
Added: Terrain Improvement flag CantPillage now works
Added: NonLethalBombard - now the NLB flag doesnt destroy units (but weakens them)
Added: CanBombardTiles - Tile Improvements can now be bombarded and destroyed
Added: CollateralTileDamage - if unit has flag it will destroy the tile imps the unit they attack is on (lethal and nonlethal bombard) 
Added: GoldPerCity building flag. add subtract gold based on the number of cities your empire has
Added: OnePerCiv -  buildings can now be Small Wonders/National Projects 


2005-10-23
Fixed: Misc crashes
Fixed: Broken/incorrect links in the great library
Fixed: Topic display bug in great library
Fixed: Hosting of MP games on XP machines
Fixed: SLIC error related to missing city
Fixed: Prediction of movement times for ships moving through cities
Changed: Message boxes to be non-modal
Added: Option to select a city first instead of an army
Added: DebugSlic and GoodAnim options
Added: Two more colour sets

2005-09-12
Fixed: Bug with gold income
Added: City limit to domestic control panel
Changed: Information window remains open across turns
Changed: Opening score tab of information window no longer closes other windows
Added: Gold loss due to conversion and production loss due to franchising to national manager resource tab
Added: Special attack window to display the cost of a special attack
Added: Construction time to tileimp tracker window
Fixed: Crashes when deleting tileimps
Added: Option to end game on db error
Fixed: MP database sync check
Added: Female leader pictures
Fixed: Age display in MP summary
Fixed: Colors used on single player selection screen

2005-08-02
Fixed: Bugs with /importmap
Fixed: Misc crashes
Fixed: Updating of values caused by alteration of sliders on national management dialog
Fixed: Bug preventing rush buying an item inserted before Capitalization or Infrastructure
Added: Buttons linking the National, City and Build Managers

2005-06-28
Fixed: Crash caused by loading savegames with the wrong number of goods
Fixed: Misc crashes
Added: More AOM support
Changed: Method of generating advances and units from goody huts to speed it up
Fixed: Problem preventing play with fewer than 3 civs
Removed: Automatic tutorial advice on low difficulty levels, since it breaks mods and generally causes problems (we have a specific tutorial button now on the main menu for those that want one)

2005-05-23
Added: Brief changelog to playtest readme
Fixed: Incorrect bonuses from goods
Fixed: Misc crashes
Added: Some more credits

2005-05-17
Added: /debugai and /debugcells commands in chat window
Changed: Some diplomatic agreements to have limited duration
Added: New restriction fields to buildings and tile improvements
Added: Bitwise operators in SLIC
Changed: Piracy AI
Added: New Anet library dlls (to fix XP hosting problems)
Added: Leader name support
Fixed: Misc. crashes and memory leaks
Added: Support for multiple ctp2_data directories
Added: Flag to prevent units being used as barbarians
Fixed: Incorrect city style name display in cheat editor
Fixed/Changed: AI worker allocation
Fixed: Server listing in bootserv.txt
Added: (Currently not useful) optimize slider button in domestic manager
Added: Optimize specialist button to city manager
Changed: National manager and city manager to show net rather than gross food and production
Added: DisplayLevel field for tile improvements
Fixed: Bug with government name display in message
Fixed: Misc GL errors
Added: "Turns to growth" display on main map
Added: Support for immobile units
Added: Ranged bombarding
Changed: The method by which televangelists are recognized as special
Changed: Method of caravan cost fix - now mods should have reasonable caravan costs
Changed: Default colour scheme to get player 1 blue by default
Added: Detailed changelog to playtest version

2005-03-05
Disabled: AI unit text, so that you don't see the AI goals.
Fixed: Crashes and memory leaks.
Fixed: Minor translation problems with the French and German string files.
Updated: strategies.txt, advancelists.txt, buildlistsequences.txt and unitbuildlist.txt, 
  to make the AI more agressive and seperate the AI personalities more.
Fixed: Vision of borders of civs you have no contact to in fog off and god mode.
Fixed: A Vision problem and a crash in combination with city conquest.
Changed: Disabled autosave for MP client.
Added: Finished production handling in the MP turn start.
Updated: Anet game number.
Fixed: Basic AI population assignment.
       - If cities has slaves the number of pops to assign is calculated correctly.
       - The first pop in the database can now be used, meaning AI can use entertainers.
       - The minimum amount of calculated entertainers can now be assigned.
       - However this are only basic fixes, population optimisation has still to be done. 
Added: A new slic power operator: **
Fixed: Slic database access, so that the feat units in CTC work.
Added: Slic function AddSlaves to add slaves to a city.
Added: Slic function IsOnSameContinent to check whether two locations are on the 
  same continent.
Fixed: Wonder start and almost complete messages are also send for the first wonder 
  in the database correctly. Precisely you get now such messages for Aristotle's Lyceum.
Fixed: Some minor bugs in the AI goal computation.
Restored: Old operator of squd strengh. That caused the huge delays in the last version
  on maps with more than one continent.
Changed: Pathfinding doesn't consider anymore danger on the way. Another solution for 
  that has to be found. This removes the huge zone of control.
Changed: Tile improvements for small cities of AIs get full utilisation bonus for placement.
Changed: AI places tile improvements every fifth turn and roads every second turn.
Added: CanSettleOn flag to unit.txt so that modders have the ability to limit settling 
  to certain terrains.
Changed: New games can now be started with only one civ (without any opponents).
Changed: The version date is now derived from the exe's creation date (not last
  modification date), to prevent of showing a wrong date if ldl_str.txt has not 
  been updated.
Fixed: Turn counter synchronisation when creating a new civilisation (by revolt) to 
  prevent one sided wars.
Fixed: Science popup in hotseat and PBEM is delayed until the appropiate player's turn.
Fixed: The risk levels in the drop down menu in the scenario editor are now in the 
  right order.
Fixed: Turn count in PBEM don't say anymore always "Turn 0"
Changed: The same algorithm for SP and MP is used to generate Barbarians
Changed: New terrain values

2004-12-28
Added: An attemp to fix range bombardment, but it does not seems to work.
Fixed: Vision
        - The staus bar only shows what it should show. (Terrain status of the last visit)
        - Help tile window shows terrain statisics and improvements from the last visit.
        - The map only shows terrain and improvements and owners and city radii from the
          last visit.
        - If fog of war is toggled of status bar, help tile window and map show everything.
Changed: The algorithm used in SP games is now the same as that used in MP games.
Fixed: Feat accomplishment propagation from server to client.
Fixed: Propagate city defensive bonus.
Fixed: Mouse and keyboard events are also handled after buffer overflow.
Fixed: Some crashes concerning the Alexander scenario.
Added: Localized strings for the new civs to the other languages.
Fixed: Arena and Aqueduct prerequisites
Restored: Compatibility for Diplomod.
Added: A Quit Button at the multiplayer screen for hotseat games.
Fixed: Memory leaks concerning vision and the resourcemap.
Improved: The music screen unconfirmed user changes are not reseted when returning to 
  the music screen after visiting the music track selection screen and removed memory 
  leaks.
Fix: Unblock the client user interface when the server has handled the city production.
Restored: Database compatibility of constDB for MP testing.
Untested fix: Make cities grow and make science handled well in PBEM.
Updated: Some player slic built-ins so that they can be not only used in string 
  replacement but also in optaining integer values.
Repaired: Some invalid links in the English Great Libary of the Alexander scneraio.
Updated: GAMEID to match the value of the new anet.inf
Fixed: Some crashes in concerning diplomat data and the sp new tribe screen.
Fixed: Movement point propagation for settlers from disbanded cities in network games 
  with 3+ players
Corrected: Return type of IsVeteran function.

2004-10-20
Fixed: Bug fix and user interface improvement for the tactical info tab of the Unit manager
Fixed: Select and center on the transport when double-clicking on a transported unit
Fixed: Spelling error ("Devision")
Fixed: Altered instant message display code to prevent messages being displayed to the 
  wrong player in hotseat mode
Fixed: Corrected handling of invalid strategies
Fixed: Prevented crashes with invalid units
Changed: Removed some duplicated code and allow to test cell whether its productivity 
  concerning food, shields and gold would be better then with the current terrain type
Added: If a terrain has no food tile improvement the AI checks whether there is a 
  terraform option avalable for a terrain with better food stats
Added: Changes to favorize Respect of non-treaspassing Treaty and retreat units
Added: Checks to avoid Army blocked in grouping phasis
Changed: The Group size check
Changed: Rollback goals in double search
Added: Methods to check if there is enough room for army along path, check danger along path
Changed: Several values in goals.txt
Fixed: typo corrected
Changed: useless compiler warning disabled
Added: CanTransport and IsCivilian, and a few other consistency changes to the Army class
Fixed: Prevented a crash when a space plane gets assigned multiple orders
Changed: Updated startegy database so that when two strategies are merged values aren't 
  overwritten by the default values of an entry if this value has not been defined
Changed:: Updated database generator so that bit pairs can now have default values as well.
  That allow to have default values in databases with entries that are merged, without
  merging default values into the merged record
Fixed: The typo that caused the number of merchants to be displayed in the scientists 
  column of the new Specialists tab of the National Manager window
Changed: Updated the database generator so that the default values of bits can be accessed
  if they have any even if no value was set
Fixed: Prevent crash when loading a saved game
Fixed: Terraform logic
Changed: Updated road path generation
Changed: Lots (and lots) of things related to AI movement in an effort to improve that
Added: Some French translations
Fixed: Improved cleanup of the data of a dead player, to not keep old strategies hanging around
Fixed: Prevented a crash when a city with walls or a forcefield gets destroyed
Fixed: Allow more than 44 playable civs
Added: 18 new civilizations
Fixed: Crossed Sword Bug
Changed: Great library enhanced
Fixed: Visual tileimp progression
Fixed: Extra checks to disable science victory in network games
Fixed: Check for clear queue actions from clients received after they lost the city to another player
Fixed: Network object bookkeeping gets cleared when exiting a game
Fixed: Propagating PW in network games

2004-09-14
Added: Option (In advanced options) to display all orders any unit in an army can perform
Added: Recalculation of support costs upon change of government
Added: New messages when investigating a city depending on whether spying unit captured
Fixed: Possible wrong sorting by "turns till production finished" in National Manager
Changed: Sorting logic to speed sorting of lists
Fixed: Sorting by govener type
Fixed: Possible crash-causing behaviour on missing files
Fixed: Text file errors in Alexander scenario
Added: Prevention against a city revolting twice in a row to allow the new owner to
  repair problems with it and stand a chance of keeping it
Fixed: Various memory leaks
Fixed: Gaia controller messages
Fixed: Various hotseat bugs
Changed: Behaviour of text in Starting and Ending Age buttons in multiplayer setup to
  best cope with variations introduced by different mods.
Fixed: Some problems with the original fix for the Miles sound library
Changed: Many things to improve MSVC.NET compatibility
Changed: Behaviour of messages tab so that newest message always displayed
Added: New option to put newest messages at the top
Changed: String hashing to prevent crash in Cradle
Added: Target coordinates to 'armytext' playtest feature
Changed: AI personalities system to improve flexibility
Fixed: Possible double assignment of same civ at the start of the game
Added: New city styles
Added: New civs
Changed: Buffer size when reading GL texts to prevent truncation of the longest entries
Changed: Lots (and lots) of things related to AI movement in an effort to improve that
Fixed: Crashes due to invalid units
Fixed: Crash caused by starting a new game from the menu
Fixed: Crashes due to malformed data files
Added: Backwards compatibility with original data files
Changed: Freight values to make caravans practical
Changed: Sprite limit to 255 (from 200)
Changed: Terrain scores to improve AI settling strategy
Changed: AI tile improvement placement strategy to allow it to place undersea tunnels
  and be more intelligent in other placements

2004-07-25
Fixed: Various memory leaks
Added: Added corrections for the French version
Added: Redermination of the city style when setting the civilization in HotSeat
Fixed: Strange tile visibility patterns in HotSeat mode
Fixed: Enabled the end turn button when unblanking the screen in HotSeat mode
Fixed: Blank the interface when loadeding a saved hot seat game 
Fixed: Recipients of messages
Fixed: Increased animation queue to 12, making unit-in-wrong-place bug less likely
Fixed: Cities do not revolt twice in a row
Fixed: Slic files for the Alexander the Great scenario
Added: Crash prevention for missing strings and images
Fixed: Sort order for governors in in status tab for blank entries
Improved: Sort logic for listboxes
Fixed: Sort order in NationalManager for turns until production is finished
Added: New message for the distinction if a unit was cought or not in city investigation
Fixed: Recalculation of military support after government change
Added: Option to display the intersection of unit orders or the union 
  of unit orders for an army

2004-06-28
Fixed: Various memory leaks
Added: Specialists tab in national manager dialog
Fixed: Bug related to changing civ with the scenario editor
Fixed: Bug with scenario savegames
Added: Translational functionality on right click on the radar map
Changed: Version of the freetype library used
Fixed: Bombard information to AIs

2004-06-15
Added: Implementation of chance of gaining an advance through conquest
Fixed: Various SLIC bugs
Fixed: Bug which would inappropriately disable the embargo and declare war buttons
Fixed: Miscellaneous sprite-related bugs
Fixed: Possible crash (may be related to Solaris project bug)
Fixed: Memory leak in the wonder movie window
Changed: Score from city sizes to reasonable values
Changed: Things to improve .NET compatibility
Changed: Naming convention for autosaves
Fixed: Blank age boxes in MP setup
Added: More automatic focus changes for cities to standardize behaviour
Fixed: Bug causing incorrect production when pressing F3 after end of turn
Fixed: Possible rounding errors on science calculations
Fixed: Incorrect display in keymapping dialog
Added: New commands in keymapping dialog
Added: New credits

2004-05-19
Fixed: Memory leak in GL
Added: New ColorSet option to userprofile.txt to aid with switching colors##.txt files
Fixed: Treatment of zero freight costs which gave strange trade route paths
Fixed: Memory leaks with government modifications to DB entries
Changed: Select current research on change research screen
Fixed: Check which prevented sueing of franchises
Fixed: When starting a scenario, use difficulty settings selected by user
Changed: French translation of "Network Game"
Changed: Implementation of new SLIC global variables

2004-04-30
Added: New SLIC global variables
Fixed: Bug with scenario editor and players who have been removed
Added: New SLIC functions
Changed: Some French and German strings
Improved: Govener AI choice of squares on which to place tile imps
Changed: Error reporting for modulo 0 errors
Fixed: Active defense movement type check
Fixed: Neutral tile imp pillage bug
Fixed: Problem with loading games using SLIC database access

2004-04-18
Added: Rush buy button status updated when gold gained from goody huts
Fixed: Empty(ing) build queue crashes
Fixed: Invalid memory access when loading from a file with a different table size
Fixed: SLIC bugs
Changed: Menu structure and graphics

2004-04-12
Added: Mousewheel support
Fixed: Problem with previous bugfix preventing rush-buying of infrastructure/capitalization
Improved: Rush buy buttons disabled outside of players own turn, to avoid MP rush buy bug
Fixed: Misleading display of rush buy costs in city manager
Added: Many missing SLIC functions
Fixed: Handling of SLIC errors

2004-04-03
Fixed: Bug preventing tile imps being built on terraformed land
Fixed: Bug with games played as players other than player 1

2004-03-28
Updated: German and English version of the Great Libary.
Updated: colors00.txt to replace some nearly identical colors and changed the 
  color order a little bit.
Fixed: Disabled restart key in network, hot seat and email games.
Fixed: Is the scenario editor launched for the first time in a game session, 
  player 1 is not selected anymore.
Added: Player selection, and with player selection color selection.
Fixed: Scenarios that allow players other than player 1 to be played.
Fixed: Games with more then 28 civilizations can now be saved and loaded.
Changed: Replaced all Neptune refferences by Uranus refferences. For some languages
  this needs to be checked if the right translation is used.
Improved: The path finding algorithm, as a downside this implementation could be
  very slow.
Added: Setting research goal to something already researched clears the goal.
Added: Itialian translation, unfortunatly it is not complete.
Fixed: Some more memory leaks.
Fixed: Pollution is not turned off automaticly when the scenario editor is used.
Added: Loading of build queues with capatalisation or infrastructure.
Fixed: When a trade route is pirated it is not anymore centered, just the sound 
  effect is played.
Added: Auto-center for bombardment
Fixed: Diplomatic proposals/responses sent from UI get the highest priority possible 
  so that the AI will not override them.
Fixed: Cannot rush buy capitalization/infrastructure.
Fixed: Problems with invalid data that lead to crashes.
Added: Sanity check on city styles (avoids crash when settling in Alexander).
Added: Savegames generated by CTP2 v.1.1 can now be loaded.
Fixed: Invisible pirates are not centered anymore.
Fixed: The Ai does not consider goals anymore that have become, e.g. destroyed cities.

2004-02-21
Added: Governments can now modify the properties of units, tile improvemnts, buildings,
  goods, wonders and feats.
Added: French translation is now complete.
Altered: Description text of the max players spinner on the new single player players screen
  to increase the understandibility.
Fixed: North-South-warp behaviour, concerning vision etc. Seems also to fix the Neptune map.
Fixed: Some memory leaks.
Fixed: A freeze bug that occurs under certain circumstances, if enemy moves are set on.
Altered: The Barbarian color is not black anymore, but rather some bright red.
Added: New and bigger message to allow the player to see on a glance what is going on when a
  message appears.
Fixed: Some things in the script.slc, to make sure better message icons are used, and more
  more messages now contain an eyepoint for better action localization. Also a doubled
  message was removed.

2004-01-31
Added: MAX_MATCH_LIST_CYCLES flag to sonst.txt of the Magnificant Samurai Scenario, so that 
  it work with this version of CTP2.
Possible crash fix: cpuinf32.dll is no longer used to examine the CPU, didn't work on non 
  intel CPUs anyway.
Improvement: Have the great library start with something more useful than Advanced Infantry
  Tactics.
Fixed: A number of SLIC functions that used calls with inappropriate arguments.
Fixed: Great library data between players in hotseat games is now cleared.
Added: In Advanced Options Menu some more useful descriptions for CloseOnEyepoint and
  ShowExpensive functions, a native speaker may have a look on the English version for
  language corrections.
Added: All the changed textfiles from patch 1.1/1.11.
Added: Missing diplomacy photos in the diplomanager.
Replaced: Buttons for the number of Civs in the single player new game screen by a spinner.
  This allows you to start a SP game with more then 7 opponents. The maximum is 31
  civilizations in one game. Unfortunatly only games up to 28 or 29 players can be reloaded.
Added: A maximum civs spinner on the same screen that allows you to set to how many civs can
  be at the same time in one game, while the limit is not reached revolting cities are
  converted into new civs, otherwise in Barbarian cities.
Added: On the same screen another spinner that will allow you to select your civ color,
  unfortunatly does not work, yet.
Replaced: Disclaimer strings are added for the old disclaimer image from the last versions,
  to allow easy localizability.
Fixed: A bug in the scenario.slc from the Magnificant Samurai Scenario, so that now a fort
  in Shichinen is built a site when it should be build.
Added: Up to 33 players are now supported with their own color. I hope you enjoy the new
  collor set. :D;)
Fixed: Some cut & paste errors repaired in Exclusions.cpp, no idea what it does now you
  have to ask Fromafar.
Fixed: Alexander scenario.slc, replaced the code in the "doesn't work, grrrr..." function
  with something that seems to do the job.
Added: Slic Database access, and a way to figure out via slic how big a database is. 
Fixed: Crash to desktop prevented in the HasAgreement function
Fixed: Crash to desktop prevented when no sound has been defined for a tile improvement.
Fixed: Unloaded buttons which caused an exit pop-up in the city window, don't ask me what
  Fromafar here means. ;)
Added: If during slic code execution a devision through 0 appears in DebugSlic=Yes mode a
  meaasge is give.
Fixed: Map wrap handling of the trade route computation is now consistent with the actual
  map type selection.
Fixed: Trade routes now follow rivers, roads, railroads and maglavs according the Freight
  flag in terrain.txt and tileimp.txt. So if you have a route between two cities and there
  is no road etc. you need much more caravans than if there is a road etc.
Fixed: Handled a CtD (stack overflow by infinite recursion) when the advances contain a
  loop in the prerequisite chain. Occurred (only) with the debug version in combination 
  with the LoTR scenario. Well maybe this is also useful somewhere else.
Fixed: Memory leaks

2003-12-14
Fixed: Prevents triggering - and forcing you to click away - the intimidating message 
  about having to sell city improvements when you rushbuy to a low gold level in an 
  otherwise sound economy.
Fixed: Some small updates for the Alexander scenario: text files only - no code.
Fixed: \ctp2_data\default\gamedata\tileimp.txt updated to align with 1.11
Fixed: \ctp2_data\default\uidata\keymap.txt updated to align with 1.11
Fixed: \ctp2_data\english\gamedata\add_str.txt updated to align with 1.11

2003-11-12
Fixed: English ldl_str.txt corrected to the correct description for Neptune world.
Fixed: Do not cause an Assert failure - in the debug version - when slaves revolt and 
  capture a city.
Fixed: Replaced Shield references with Food ones in the food functions.
Fixed: Removed an AI mental block which prevented it from placing gold improvements on 
  forest tiles. Unfortunately, this is not a very general solution: it will not work for 
  water tiles.

2003-09-12
The sound system should be in working order.
Added: Start screen shows legal mumbo jumbo.
Fixed: Hidden goods no longer show up in the Great Library.
Added: Pollution powergraph.
First fix for the PBEM (PBEM is still incomplete though).
Fixed: Coastal fortress and flak guns now work.
Fixed: Sea city sprite bug.
Added: function so that the population is displayed in the CityPopSoinner can be 
  used elsewhere in the program.
Fixed: Bug in Scenario Editor that prevents displaying of more then three city 
  style buttons in the city tab. Well these so far these buttons don't have icons 
  but at least they work now as expected.
Fixed: Bug in the Scenario Editor so that cities created by the Scenario Editor 
  have the right size as displayed in the CityPopSpinner and keep their style given 
  by the Scenario Editor. 
Fixed: Cities created by the scenario are now selected per default.
Attempt to fix bug concerning displaying of unfinished tile improvements. 
  Unfortunatly this doesn't work as expected as the graphics of the unfinished 
  improvements aren't updated as exspected.
Fixed: PercentComplete function.
Fixed: infinite rush build bug.
Fixed: view the movies without a sound card.
Fixed: GetNearestWater Slic function.
Fixed: automatic selection when clicking the eyepoint of a message window.
Implemented: FreeAllSlaves, DestroyBuilding, OpenBuildQueue, TileHasImprovement 
  and ArmyIsValid functions.
Fixed: Center on Pirating Bug.
Fixed: Display the underlying terrain information when clicking on an enemy object.
  This prevents easy detection of invisible enemy units.
Fixed: The science manager now display the real costs for the player.
Added: A flag to the ConstDB to expose the number of process matches for each army 
  so that this number is not hardencoded anymore to make the AI behaviour more 
  modder friendly.
Fixed: Recover production shields when disbanding a unit from the unit tab of the 
  city screen.
Fixed: Reduced the visibility to the player of an AI cheat in the pathfinding 
  algorithm.
Fixed: Make forts heal as cities, instead of fully in one turn.
Fixed: Veteran status is now used in combat.
Added: The music option to the drop down menue in the menue bar.
Added: When a tile improvement is constructed a sound is played like in CTP1.
Added: Strings in English and German for the new music option and for the ranking tab.
Added: Short cut to the music screen to the keymaps.
Added: A Sound flag, a Freight flag and remove the limit of 3 for contruction tiles.
  All in the TileImprovementDB.
Fixed: None of the rush buy buttons is disabled anymore when there is only one turn 
  left from completion.
Added: A user interface option: Enables you to view cost and effect of all known tile 
  improvements, even if you are short on PW. To activate this option, you have to edit 
  userprofile.txt and set ShowExpensive to Yes. 
  (Well has to be added, if the game doesn't do it automaticly.) 
  There is no in-game screen to modify this setting (not yet).
Fixed: AI-AI-Diplomacy actually happen now.
Added: Two new flags for the terrainDB to allow modders to customize the graphics for 
  goody huts, this modification allows the modder to define for each terrain a 
  different hut graphic, for example you could have glacier like style huts for icy 
  terrains or a dessert or oasis like ruin/hut for desserts and so on. The new flags 
  in the terrainDB are optional if both of these flags are missing or just one of them 
  a default image is used, like it is in the original CTP2. The terrain.txt here is a 
  modified version, I replaced some of the ruins graphics with another graphic that is 
  part of the tile file already. This terrain.txt is meant as a base for disscussion, 
  so far it just demonstraded that it does work.
Added: The maximum city size that you can create with the scenario editor is now 999.
Added: Flat Map and Neptun World option to the single player shape selection window. 
  This new features are only available in a single player game so far.
Fixed: Correct movement costs for ships above seatunnels.
Fixed: Do not overwrite the user's leader name with the default civilisation-dependent 
  name when there is no reason to.
Fixed: Do not trigger a disaster warning when there is no pollution at all.
Fixed: Some Hotseat issues with hiding information between players.
Implemented: SLIC functions GetRoundsToNextDisaster and GetCurrentPollutionLevel.
Added: Close message boxes automatically when clicking the eyepoint.
  A line CloseOnEyepoint=No will be added to userprofile.txt when you start a game with 
  the modified source. You have to change No to Yes to activate this option. 
  There is no in-game screen to change this setting.

