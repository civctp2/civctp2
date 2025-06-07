THIS MATERIAL IS NOT MADE OR SUPPORTED BY ACTIVISION.
This version of the Call To Power 2 is developed by the CTP2 community.

Contact the authors at
https://github.com/civctp2/civctp2/issues

This is the current state of the source code project released for playtesting.
Keep in mind CTP2 is not free. You still need to buy the game.

Playtesters should be aware that these files are intended for a plain vanilla 
install of CTP2. It should also be no problem to install over a patched
version of CTP2, or over a previous playtest version.

In theory this version is now compatible with most of the mods, but special
actions may be required to make them work properly. Please visit the
thread mentioned below and ask if you need more information.

Installation instructions:
Unzip into your CTP2 directory and play. If you get warnings about 
overwriting existing files then you have unzipped to the correct location.
It might be wise, if you have the space available, to make a copy of
your CTP2 directory and unzip this into the copy, so that you can preserve an
unchanged copy for playing mods, etc.

If you want to use the 32 bit SDL version you have to do nothing. If you want to
use the 64 bit Windows version update your shortcuts pointing to 
.\ctp2_program\ctp\x64\ctp2.exe. If you love your settings then copy 
usercritmsgs.txt, userkeymap.txt, and userprofile.txt from .\ctp2_program\ctp\
to .\ctp2_program\ctp\x64\. If you insist on using the 32 bit DirectX version
then update your shortcuts to .\ctp2_program\ctp\ctp2-dx.exe. Of course, you
can start the executables from their folders directly.

The used language version is now a user setting, it is determined automatically
when you start this version for the first time according to your OS settings
and the available languages. If you are unhappy with the choice you can go to
the language options screen and change it there.

The SDL version has a windowed option, which you can select from the graphics
options. Make sure if you go for windowed mode that the selected resolution is
below your current screen resolution. Otherwise the frame and title bar will be
outside of the field of view and the mouse range on Windows and on Linux the
bottom and the right part will not be on screen. The window is not resizable.


After installation, you can start a new game or continue a game from
a previous version. All you need to do - after loading your save game - is to
open the chat window by typing the apostrophe key (') and enter: /reloadslic
You have to do this once per save game.

For Chinese gamers: It is unclear if that version is just a fan translation.
                    However, right now it does not load and so is disabled.
                    Of course you can help fixing this. In case there is a 
                    retail version of that, which you use do the following.
                    You have to
                    first rename your ..\ctp2_data\english\ folder to
                    ..\ctp2_data\chinese\, then unzip the Apolyton Edition
                    over your CTP2 installation, and then edit your
                    ..\ctp2_program\ctp\civpath.txt. This file contains a line
                    saying "english", change it to "chinese".

Please report any problems, bugs, crashes, etc. as an issue on GitHub:
https://github.com/civctp2/civctp2/issues

The latest source code files can be found at: https://github.com/civctp2/civctp2

Visit BureauBert's http://www.ctp2.info/ on how to use some new additions.

This version comes with new tech posters, which are in the same directory as this readme.

Known bugs:

- When you start a new game from a running game than the game put the new game screen in the
  middle but does not paint the rest of the screen black so that you left with what was on the
  screen before. However, this does not disturb the function.
- The map on the Gaia Controller does not show the coverage over sea.

On Windows DirectX build only:
- The new shield that indicates in the control panel that more than one unit is on a tile does
  not display the number of units.
- The chat window has some problems selecting text with the mouse.

On Linux:
- MP is broken
- Slecting words in text field via double click treat language specific characters as non word
  characters.

The Great History: In case you are interested, here are the previous playtest threads on Apolyton:
http://apolyton.net/forums/showthread.php?t=103817 "PROJECT: Playtest"
http://apolyton.net/forums/showthread.php?t=115144 "PROJECT: Playtest II"
http://apolyton.net/forums/showthread.php?t=127059 "PROJECT: Playtest Thread III"
http://apolyton.net/forums/showthread.php?t=147969 "PROJECT: Playtest (the 4th Thread)"
http://apolyton.net/forums/showthread.php?t=161726 "PROJECT: Playtest (Thread No. 5)"
http://apolyton.net/forums/showthread.php?t=172035 "PROJECT: Playtest (Thread No. 6)"
http://apolyton.net/forums/showthread.php?t=185707 "CTP2 Apolyton Edition: Revision 990 (01-Jun-2009)"
http://apolyton.net/forums/showthread.php?t=187205 "CTP2 Apolyton Edition: Revision 1013 (25-Jul-2009)"
http://apolyton.net/forums/showthread.php?t=190316 "Apolyton CTP2 Edition: Revision 1056 (28-Feb-2010)"
http://apolyton.net/forums/showthread.php?t=191653 "Apolyton CTP2 Edition: Revision 1097 (07-Jun-2010)"
https://apolyton.net/forum/other-games/call-to-power-2/ctp2-source-code-project/245678-apolyton-ctp2-edition-revision-1111-12-jun-2011
https://apolyton.net/forum/other-games/call-to-power-2/ctp2-source-code-project/9479186-ctp2-apolyton-edition-2025-01-20

Changelog (Well, you don't have to read the full thing. ;)):
2025-06-XX
Added:    32 and 64 bit Windows SDL builds

::Start SDL specfic changes::
::Since there was no Windows SDL version these changes can only observes with the Linux version::

Added:    Select with the mouse and the shift key text in a text field.
Fixed:    Make the textfield ignore excape characters, except Ctrl+A,
          Ctrl+C, Ctrl+P, and Ctrl+X, which execute select all,
          copy, paste, and cut.
Added:    Make Ctrl+A select everything in a text field.
Added:    Set the carret position when you click in a text field.
Added:    Select a word in a text field if left clicked on it.
Added:    Select the text under the cursor in a text filed if the mouse
          drags over it. (Left mouse button is pressed.)
Added:    Localized version for selecting words, which however does not work
          on Linux.
Added:    Continue text selection if the mouse is dragging outside the text
          field.
Fixed:    Do not enter more caracters into a text field than its size.
Added:    Keep the carret in text fields always visible.
Changed:  Make the carret the same color as the text in text fields.
Added:    Cut, copy and paste in text fields.
Added:    Press ctrl and the arrow keys the carret jumps a word, if shift
          is also selected it selects the text to the next word boundary.
Added:    Highlighting with shift and the arrow keys in text fields.
Fixed:    A crash in the text field when everything was supposed to be selected.
Fixed:    Make the carret blink in text fields.

::End of text field specific changes / implementations, a text field has a lot of
::functionality, and it is still a single line text field without password
::function. However, that seems not be used.

Fixed:    Make the user interface responding to mouse and keyboard
          input while the AI is busy.
Fixed:    The game accepts input from localized keyboards, the apostrophe key
          now opens the chat screen.
Fixed:    Make the enter key on the keypad work.
Fixed:    The text field in the MP lobby accepts input.
Changed:  Move the fullscreen option in SDL builds from the command line to the
          the graphics options on the user interface.
Fixed:    Center the background image in the MP start screen.
::End SDL specfic changes::

::Other fixes, changes, additions
Added:    Option to the advanced options if units on sentinel should board a
          passing transporter. The default option is no boarding.
Added:    Description text for the Unix internet option on Linux.
Added:    Gossip
Fixed:    Allow gaining an advance via gossip, if the target civ has no advance
          tham this civ is boring.
Added:    Slic function void HearGossip(unit_t), basically for testing HearGossip, but
          modders may find it useful.
Fixed:    Allow with the mouse and the shift key selected more than one item in a 
          list box.
Fixed:    The Linux version uses the directory of the executable as working directory.
          This way you do not have to start CTP2 from that directory, but yoou can start
          it from anyway.
Changed:  The strings in the string database are now all in the same place accross
          languages, except additional strings for the languages, they are at the
          end. This makes translation easier, as new strings just need to be placed
          at the same location in the matching files.
Added:    French scenario text files
Fixed:    The game will not crash if you click on the eyeball in a message about a unit
          that became a veteran or elite unit and that is at a board of a transporter.
          Instead it brings you to that transporter, were the unit is.
Added:    New slic function for testing the fix for the veteran and elite messages.
          These may also be useful for modders:
              - void ToggleElite(unit, int (on/off))
              - int IsElite(unit)
              - void SetVeteran(unit)
              - void UnsetVeteran(unit)
              - void SetElite(unit)
              - void UnsetElite(unit)
Fixed:    Deactivate user profile options in MP that can be considered as cheats.
          These are.
             - CitiesLeaveRuins
             - EnergySupply&DemandRatio
             - AICityDefenderBonus
             - SectarianHappiness
             - AIMilitiaUnit
             - CityLeavesRuins
Fixed:    Select only units for uprising and militia that can be on the
          target terrain.
Fixed:    Make the happiness impact from bioinfection show up emmidiately in
          the city manager.
Fixed:    Update the happiness timer only on BeginTurn. This way the impact
          of bioinfaction will not disappear if you just press the optimize
          specialist button.
Changed:  Reduced the flash interval of the bio and nano infection icons. The
          flashing can be disabled entirly in const.txt.
Fixed:    Special rules from the userprofile, which can be set at a new game,
          cannot be changed during a game anymore, unless you use the scenario
          editor. Those are:
             - CityCaptureOptions
             - OneCityChallenge
             - RevoltInsurgents
             - RevoltCasualties
             - BarbarianSpawnsBarbarian
             - Upgrade
             - NewCombat
             - GoldPerUnitSupport
             - GoldPerCity
             - NoCityLimit
Fixed:    NoCityLimit has been fully implemented the user interface reflect it
          if it is on or off, and the AI also takes it into account for settling.
Added:    The special rules screen to the MP rules screen and the scenario editor.
Added:    Disable the Gaia Victiory on the special rules screen.
Fixed:    The special rule bloodlust, did nothing, now you have to conquer
          everybody else to win the disables all other victory types. If you have
          not won by the year 2300, you lose.
Added:    Automatic turn end to the advanced opstions.
Added:    A language database file: Language.txt and a userprofile option
          for selecting the language.
Added:    Automatic language selection based on the operating system and the 
          languages available.
Added:    Language selection screen in case you do not like the automatic choice.
          It also gives some informaton about the available languages such their
          state of completeness.
Added:    Czech and Polish fan translations, which are however incomplete.
Fixed:    The AI does not try to pillage tile improvments that have the flag
          CantPillage (Feature for modders).
Changed:  Move disclaimer.txt from the program diretory to the data directory.
          So that it can be truely localized. This means different version in
          different languages can coexists in the same copy of CTP2.
Added:    Left hand mouse option to the advance options.
Fixed:    Some memory corruption on BeginTurn in MP. Unclear whether this has
          an effect, but the debug version did not like that.
Fixed:    A terrain synchronization issue in MP. This made the game crash on
          the client side, when the client was joining an MP game. This bug
          was introduced in 2008 and effectively disabled MP in CTP2.
Added:    The run in background is now available on the user interface.
Fixed:    The user profile option run in background allows to run the game in
          the background.
Fixed.    Display the correct ping in the MP chat screen
Fixed:    Replace the boot server list for MP games so that the game looks for
          current servers. This may have to be fixed again when servers change.
Fixed:    Crash in multiplayer civ selection screen when no civ is selected.
Changed:  Accelerated start up time of the debug version, not really directly
          relevant for the player, but helps speeding up debugging.

2025-01-20 (No revision number we switched to Git)
Fixed:    The flames in the credit screen do not have a black background.
Added:    The GitHub contributors to the ApolytonEdition to the credit screen.
Fixed:    Do not show a border on main map if the tile borders to an
          unexplored tile to not give away that the tile changed the owner
          since you have seen it the last time.
Changed:  Turn the cursor into an erase cursor if the scenario editor is
          in erase mode so that you do not erase anything accidentally that
          you do not want to erase.
Fixed:    Make the erase button in the scenario editor stop erasing when
          you switch it off
Fixed:    Deselect any unit, city, terrain, and terrain improvement switch
          if the erase button is selected in the scenario editor
Fixed:    Do not cities as units on the radar map if they are not known
          by the player.
Fixed:    Show units immediately on the radar map if they were created by
          from the scenario editor.
Fixed:    Do not show tiles as white that were owned by nobody when last
          time visited, but have been claimed in the meantime.
Change:   Smoothen the drawing of tiles and borders on the radar map.
Removed:  The filter button on the radar map as this is not needed anymore
          with the change above.
Fixed:    Do not show undersea tunnels as land tiles on the radar map.
Changed:  Make non-exclusive graphics mode the default on Windows DirectX.
Changed:  Play a pirated route sound even if the position from that the
          the sounds come is under the fog of war. The player simply gets
          a message that the route was pirated and so a sound is adequate, too.
Fixed:    Do not play sounds that come from positions the player does not,
          either because the position is unexplored or is under the fog of war.
Fixed:    A crash when clicking on the top border of an empty list box.
Changed:  Increase the distance of cities the AI connects with roads so
          that cities that can be connected via road within its boarders
          are indeed connected.
Fixed:    Cities founded after the lunch of Globe Sat are also revealed to
          the Globe Sat owner.
Changed:  The AI considers less cities for siege. This way it focusses on
          more important cities and waits until it has built more units.
Fixed:    AI path finding for boarding units.
Changed:  The AI builds less units for garrison, as this is quite expansive
          and garrison units can only be used for city defense.
Changed:  Add some variety on the order food and production tile improvements
          depending on the AI's personality.
Fixed:    The AI also uses units on board of a ship for its goals.
Fixed:    The AI does not use units as escorts without attack or defense
          such as transporters.
Fixed:    The AI uses the most exploitive slider settings as long as the
          people are happy enough.
Changed:  The AI does not try to incite revolutions in Barbarian cities.
          As this does not help.
Fixed:    The AI only uses the minimum entertainers necessary. The minimum
          is now determined from the value in strategies.txt.
Fixed:    The AI does not hold diplomatic receptions if it cannot.
          10 turns must pass so that receptions can be hold again.
Changed:  The AI ignores trespassing for Diplomats, so that it can build
          embassies. Trespassing with diplomats is not considered a
          violation anyway.
Changed:  Slavers are not civilians, so why should abolitionists be?
Fixed:    If the AI rally its troops it will use a spot all the units can
          enter.
Fixed:    The AI builds transport ships earlier.
Changed:  Make the AI prefer sieging Barbarian cities a bit more.
Fixed:    A bug in AI path finding that prevented the AI from sieging cities.
Fixed:    Do not update the cause and effect tab in the empire manger if
          the sliders do not change position. Fixes some wired behavior.
Fixed:    The AI also builds missionary units during garrisoning.
Changed:  Ecotopian Ai players may build eco rangers.
Fixed:    Avoid a "No such object" message with game shutdown/crash after a
          slave uprise in a city without units.
Fixed:    A crash when more than one player where eliminated in the same
          turn.
Fixed:    A hang in the multi queue screen.
Fixed:    Freed slaves go to random cities, so that they do not starve in
          overcrowded cities.
Fixed:    When slaves uprise, the city population is reduced by the number
          of slaves units that died during a battle.
Fixed:    If the slave uprising is successful, and found a new civ. This
          civs establish diplomatic relationships with its former owners.
Changed:  The free slave success message got and eyepoint.
Changed:  Abolitionist can be build if the civ has no slavers but still owns
          slaves.
Changed:  The best trade route is now determined by the gold per caravan value,
          instead of the total gold.
Fixed:    A crash caused by active franchises of a dead civilization.
Changed:  Victory enslavement is now done on relative HPs of attacker
          and defender.
Changed:  The path finder prefers to circumvent unexplored territory for
          the human player.
Fixed:    Make sure that the path finder finds the shortest path.
Added:    Scroll bar for the city list when offering or requesting cities
          via diplomacy if there are more then 25 cities.
Improved: Visuals of list boxes.
Changed:  Make the army manager look like the other managers.
Added:    Sort age button o sort items in Great Library by age.
Fixed:    Slic Strings are resolved after loading even so the string
          database has been modified. Useful if you switch languages
          while playing. However, after installing a new version of
          the Apolyton Edition you still have to reloadslic.
Changed:  The unit panel shows a shield if there are more than one unit
          unit in different armies on the field. You can click the shield
          for details. Works in that case like the next unit button.
          If there is only one army with more than one unit it shows the details.
          The shield also shows the number of units on Linux, however the number
          is missing in the Windows version.
Changed:  The next unit button does not try to select multiple units if
          only one unit is on the tile.
Changed:  The build manager gives suggestions without preset conditions.
Fixed:    A hang that occurs if an empty name is given to a build queue.
Fixed:    Disable the load queue button in the build manager if there is
          nothing to load.
Changed:  Only one list in the Build manager has the focus so that it is
          clear which item is highlighted.
Fixed:    Make all managers supporting rush buy using the same underlying
          city data instead of copies, so that the managers are aligned.
Fixed:    Disable the clear queue button the build manager if the queue
          is empty.
Fixed:    Select the next item when an item is removed from the queue.
Fixed:    Show the item and the title of a custom queue.
Fixed:    Disable create custom queue button in the build manager when
          loading a queue.
Fixed:    Show the selected item for a loaded queue.
Changed:  Highlight the current build item after changing city.
Fixed:    Disable the Great Library button in the build manager if no item
          is selected.
Changed:  The good animation delay from 5 to 8 seconds.
Fixed:    The good animation settings are also respected for trade routes.
Fixed:    Draw fogged tiles instead of black tiles when scrolling up.
Fixed:    Fixed overwriting glyphs fixes mostly the capital C.
Added:    Animate goods in the help tile window.
Fixed:    Force fields can also be built in sea cities.
Added:    Trench tiles from CTP1 and enabled in new maps.
Fixed:    Only one animation is played when a city is being franchised.
Fixed:    If an advance is given by diplomacy or a goody hut the advance
          that is being researched can still be selected from the research
          dialog.
Fixed:    Make sure that every tile can be centered on every zoom level.
Fixed:    Draw correctly national and city influence borders while zooming.
Fixed:    The music is restarted after the victory movie has been shown. 
Added:    Messages about slaves taken during battle, and where they will work.
Changed:  Make civilian and special units invisible for globe sat.
Fixed:    The player is notified if a target is protected against conversion.
Fixed:    The AI does not try to establish embassies with civilizations that
          the AI is at war with or that has the Great Wall.
Changed:  Space plane move points and fuel reduced so that it is less a
          replacement for cargo helicopters.
Fixed:    Make the Alexander scenario loadable and center on Alexander when
          started.
Changed:  Rearrange city names and city icons drawn on the map.
Changed:  For modders, Sound and FailedSound in Orders.txt are now SoundRecords.
          This makes it easier to use them than bare integers.
Fixed:    Only play the advertisement special effect if advertisement was
          successful.
Fixed:    Units, cities, and goods are not redrawn while scrolling to avoid
          side effect.
Fixed:    Leviathon is given by plasma weaponry as stated in Great Library.
Fixed:    Cavalry becomes obsolete with tank warfare.
Fixed:    Make tanks only obsolete with fusion tanks, not before.
Added:    Obsolete advances for unit descriptions in the Great Library.
Changed:  Center the map on a bombarding unit.
Fixed:    Do not animate a special attack from the position of the attacker,
          if he is not visible, instead just animate it at the target position.
Fixed:    Do not draw trade routes below the map when wrap setting are enabled.
Fixed:    Draw trade routes while scrolling.
Fixed:    Animate launching a space plane.
Changed:  Pause the good-actor for 10 seconds after the animation has finished.
          So that the goods are not continuously animated.
Fixed:    Keep the units at their position while scrolling.
Fixed:    Color display of the unit shield and medal.
Fixed:    Play a general sound if no special attack sound is specified.
Fixed:    Play the correct success and failure special attack animations.
Fixed:    Only run the special effects for injoin and reform city, once.
Changed:  Do animate removing trade routes. Since this has neither sound
          nor a special effect it is just a waste of time.  
Fixed:    Draw units that are being removed.
Fixed:    Draw the animation sequence of units in the correct order.
Fixed:    Draw animated units when they go off screen or onto a ship.
Improved: Sped up trade manager, especially with many trade routes.
Fixed:    Saves games accumulated size if the game was played for a long time.
          A reload reset this.
Fixed:    The stored production in a city cannot become negative
Fixed:    Unknown cities cannot be revealed with the special cursor.
Changed:  Trade routes can now reveal the source and destination city if one
          piece is seen thanks to the gossip of traders.
Fixed:    Caravan demand for trade routes cannot be negative.
Added:    The trade route creation and cancel message gives the bonus
          values of the route.
Fixed:    The AI does not buy a good in a city that is already available there.
Changed:  The does not consider all possible trade routes, so that it does not
          waste caravans on low value trade routes.
Changed:  Reduced the value of domestic trade routes.
Changed:  Increased the value of trade routes with trade pact partners.
Changed:  The AI evaluates in all places the value of a trade route the same way.
          This avoids having the AI changing its mind once a trade route has been
          created.
          instead by a rough distance, since the rough distance might be
          significantly off. 
Fixed:    The AI takes into account for replacing trade routes that a cravan is
          lost.
Fixed:    The best trade route is determined by value per caravan.
Fixed:    The does not try to sue units that cannot be sued.
Fixed:    The city capture options are not shown for cities of size 1, since
          those are destroyed anyway.
Fixed:    Known city walls and force fields under the fog of war are still
          known after a reload.
Changed:  Emphatic Diplomats cannot be expelled like normal Diplomats so
          that diplomacy cannot be avoided.
Changed:  The coverage of obelisks has been increased so that the science
          victory can be achieved with less aggression.
Fixed:    The readiness costs when the readiness slider is moved.
Fixed:    Force fields are not drawn with the odd hole.
Fixed:    The diplomacy window lists all types of agreements.
Fixed:    For modders, the slic Liberate functions sends the conquering
          units home before the city is given to the Barbarians.
Added:    Eye point to unit expelled message.
Fixed:    Crash when a city is founded on an obelisk.
Fixed:    Accelerate the unit manager, import if the civilization has many
          units.
Fixed:    The AI builds sea tunnels.
Fixed:    The progress bar shows how many units got their orders this turn
          with separate color for units that are sleeping or are entrenched.
Fixed:    The victim of advertising gets a message.
Fixed:    Cities can only be franchised if the franchiser is next to the city.
Fixed:    If a franchise is replaced by a franchise from another civilization
          a message is send to the former owner.
Added:    Enslave settler message for attacker says where the new saves will be
          working.
Added:    Bought goods can add food, production, and gold to a city
Changed:  City window to account for that.
Changed:  For modders, the user interface definitions are now in the 
          ctp2_data/default/uidata/layouts/ folder.
Added:    If you are about pirating a trade route from a player you have a
          trade pact with, you must confirm whether you want to break it.
Changed:  Trade route drawing was cleaned up.
Added:    The help tile window reports the number of trade routes and their
          owners that go through a tile.
Added:    The trade manage shows the profit from trade that goes through cities.
Fixed:    Adjust the city influence if a neighbor city is gone or has shrunken.
Added:    The trade manager shows the losses due to piracy.
Added:    New options for the power graph.
Fixed:    New civilizations inherit their age from their parent civilization.
Added:    The number of advances needed and known for the chosen tech goal
          are now displayed in the tech tree and the advance chooser.
Added:    If a tech has been chosen than the prerequisites are marked in the
          Great Library.
Added:    The tech goal is now also shown in the Great Library if chosen.
Fixed:    You get a message if you earn some gold from a trade route that goes
          though your city.
Fixed:    The AI uses water units for attacking units and sieging cities.
Fixed:    The AI uses diplomats for establishing embassies and throwing parties.
Fixed:    The AI does not establish embassies and throws parties at civilizations
          it is at war with.
Fixed:    The AI does not build obelisks if they do not increase the coverage.
Fixed:    The AI does not build farm on installations such as watchtowers and
          thus removing them.
Fixed:    The AI uses sea engineers to add population to sea cities.
Fixed:    The AI will only use units for goals that can also execute that goal.
Fixed:    The AI does not go over the city limit anymore, unless specified
          in its personality.
Fixed:    Spies cannot plant nukes anymore if nukes have been eliminated.
Fixed:    The AI used Capitalization and Infrastructure if it has nothing else
          to build instead of leaving the build queue empty.
Fixed:    The AI unloads cargo from helicopters next to a city if it is full
          and it needs more than 12 units to defend this city.
Fixed:    If an army is pirating a trade route this is also saved and restored
          after reload.
Fixed:    You get a message if one of your trade routes got pirated.
Fixed:    The AI will siege a city only afterwards all the units in the stacks
          have been grouped.
Fixed:    If the AI decides to siege a city with more then 12 units it will
          attack with the bigger stack first.
Fixed:    The AI does not use a stack for something else if it is waiting for
          other units for an attack.
Fixed:    The AI does not use transport helicopters for attack.
Fixed:    The AI does not try to use aircraft carries to transport land units.
Fixed:    The AI gives the same priority for settling land and sea cities.
Fixed:    Calculating the civilization center so that the AI can use its
          resources properly.
Fixed:    The AI can now conquer one tile island cities and cities on peninsulas
          if land access is blocked. It only selects for those cities units that
          can beach assault.
Fixed:    The AI does not retreat units to a city if the city has no room for
          them.
Fixed:    AI path-finding for transporters.
Fixed:    The AI does not try to use units that need transport for a goal if
          the goal should be executed without transports.
Fixed:    The does not try to unload transporters in cities if there is not
          enough room for the cargo.
Fixed:    The AI does not rally its troops next to the attack target. So that
          the defenders of a city do not have all the time to bombard to the
          ground those units that are already there.
Fixed:    The AI also the units in the city garrison for goals defending
          a city. This prevents the AI from over defending.
Fixed:    The AI uses the correct unit count for its garrisons from
          its perceived treat level.
Fixed:    The AI only uses single units for pirating and pillaging.
Fixed:    The AI does not group helicopters into armies, but instead unloads the
          cargo of the helicopters and groups that into the army.
Fixed:    The AI does not consider civilizations it has not met yet as enemies. 
Changed:  The AI tries to enslave more or tries to free more slaves, depending
          on its attitude towards slavery.
Fixed:    The following actions are now cold war actions, that means they do
          not mean hot immediately. The AI treated them as cold war actions
          anyway:
          - Enslave settlers
          - Slave raid citizens
          - Free slaves
          - Incite slave uprising
          - Incite revolutions
          - Bioinfect a city
          - Plague a city
          - Nano infect a city
          However, nuking a unit is now a hot war action.
Fixed:    The AI prefers to sue units with its lawyers at home.
Fixed:    The unit power on a transporter is now also considered by the AI
          for its attacks. That stops the AI from loading and unloading
          transporters.
Fixed:    The AI does not attack players, it is neutral with.
Fixed:    The AI builds the Gaia Controller.
Unixed:   The Gaia Controller can only be build by big empires.
Fixed:    The AI terraforms dead tiles within its borders.
Fixed:    The AI does not use anything for research if it has nothing to research.
Added:    You can now trigger a disaster from the chat box, cool isn't it? :D
          But helped to fixed the following disaster related bugs:
Fixed:    Tiles with undersea tunnels cannot become dead tiles.
Fixed:    Remove ruins when the are flooded.
Fixed:    The AI will resettle the areas that were freed from cities
          by the raising sea level.
Fixed:    When swamp tiles are converted to shallow sea during the first
          disaster of global warming, also roads are removed from them. 
Fixed:    When the last disaster has been triggered from the database,
          no more disasters can be triggered.
Fixed:    When a tile is pillaged than also it neighbor tiles are redrawn, so
          that a road ends on that neighbor tile properly.
Added:    The chat window can be opened without the keyboard from the menu.
Fixed:    The AI will not sell buildings if this does not help to prevent
          bankruptcy.
Fixed:    The AI now also considers the city limits for choosing a government.
Added:    If you pirate somebodies trade-route you will get a message about how
          much gold you have robbed.
Fixed:    The wonder with the IncreaseSpecialists flag will increase the amount
          of happiness an entertainer adds.
Changed:  The AI builds roads around its cities for better troop deployment.
Improved: AI priorities to its goals, such as attack or siege
Improved: AI city spacing and settling
Added:    Feedback for the Easter eggs coming from the chat box
Added:    More goods, and some goods were rearranged
Added:    Mouse-wheel support
Fixed:    The animation settings for goods and trade routes are now respected
          for games loaded from file. Previously good animations were only played
          for newly created games.

2011-06-12 (Revision 1111)
Fixed:    The game does not crash, if a city is supposed to be converted and the
          converting unit was killed the turn before.
Fixed:    The AI does not try to disband settlers anymore.
Fixed:    After a reload, the correct strategy is loaded for the AI.
Fixed:    The AI does not use units for garrison if they are not needed. This
          frees troops for other tasks.
Fixed:    Thread and power calculation have been corrected so that the AI does
          not waste resources to fulfill its tasks.
Fixed:    The AI does not hunt enemy units far away from home, just because one
          has been seen by a spy.
Fixed:    The AI uses the nearest unit as rally point for rallying troops. This
          way the AI does not get distracted.
Fixed:    When the AI is rallying armies, it marks all armies as being used,
          even so they are not used. This prevents the AI from assigning those
          armies to other tasks, so that the original task can still be
          fulfilled.
Fixed:    The AI ungroups stealth units from non-stealth units if their goal
          is a stealth goal.
Fixed:    The AI does not move ships before all passengers are on board.
Fixed:    The AI does not use units for beach assault that cannot beach assault.
Fixed:    The AI unloads cargo helicopters if they reached land, and there is
          only land up to the target.
Fixed:    Finished the Oceanforming tab for modders.
Changed:  Reduced memory requirements for the AI and sped up the AI.
Fixed:    Fixed incorrect calculation of ZOC.
Fixed:    The AI unloads its cargo from helicopters in front of the target city
          instead of flying there with the helicopter.
Fixed:    Fixed city happiness calculation so that it depends correctly on crime
          and pollution. This way the numbers are the same if an entertainer has
          been added and then removed.
Changed:  The AI waits with building wonders until it has four cities or no
          space for more cities.
Changed:  The userprofile options crash logging, show city production, civ flags,
          enemy health, and new combat have been enabled by default. The
          userprofile option scroll delay has been disabled by default.
Changed:  The difficulty were renamed to classic civ difficulty names.
Updated:  The AE mod to the current state of the Apolyton Edition.
Changed:  The AI reconsiders the plans for its units like their paths if they
          reveal foreign units.
Fixed:    The calculates the strengths of its units correctly even if they are
          transported.
Changed:  The AI is more likely to conquer cities that are on another continent
          but on another side of a strait the AI has access to.
Changed:  The AI does not try to use its transporters as attackers, so that
          their goal does not fail.
Fixed:    The AI recalculates the needed numbers of transporters for it goals
          after a process match cycle, this prevents the goal from failing.
Fixed:    The AI continues on a goal even after grouping in a city.
Fixed:    Fixed city support cost calculation.
Added:    Added /beginscheduler command to the chat window. This way the AI
          unit movement can be tested without waiting on the other AIs.
Fixed:    When the AI steps on an invisible unit it is revealed to the AI so
          the Ai can bypass this unit or kill it.
Fixed:    Fixed the AI desire war calculation.
Fixed:    If a slave raid fails, because the slaver has not enough movement
          the victim does not hear that.

2010-06-07 (Revision 1097)
Fixed:    The AI handles incursion permission correctly even directly after a
          reload.
Fixed:    The AI now ranks correctly its opponents by their strength.
Changed:  If the visible player is a robot, then the displayed unit path is
          those for robots, this is useful for AI testing.
Added:    The intelligence window now shows opponent as very strong.
Added:    The AI avoids danger on the way of its units.
Fixed:    Games with a dead player in can be loaded again.
Fixed:    The scenario editor generates goods even if no new game was created
          during the active session.
Fixed:    A memory leak in the AI code does not eat all the memory in the
          system anymore, so that the game does not crash, because there is 
          no memory left.
Fixed:    The wonder movies play again, without crashing the game.
Changed:  The AI now conquers cities according to a global plan instead of
          picking those first that are closest to most of its units.
Fixed:    Upgraded units get room for cargo if the original unit had no cargo
          and the upgraded unit should be able to carry cargo.
Added:    Added Chinese text files.
Fixed:    The AI uses transporters even if a target is on the same continent
          but cannot reached, because for instance the way is blocked.
Fixed:    The AI conquers cities if it has a big stack adjacent to that city
          representing 2/3 of the needed power. This is no problem since the
          needed power is calculated with some buffer.
Changed:  The AI gives for conquest higher priority to cities that have a land
          connection to the existing AI empire.
Changed:  The AI gives for conquest higher priority to cities of empires with
          less than three cities.
Fixed:    The minimap shows the diplomatic relationships correctly even if you
          recenter the minimap.
Changed:  The AI prioritizes for conquest cities of its weakest enemy.


2010-02-28 (Revision 1056)
Fixed:    If the AI runs into a foreigner and if it desires war, it goes to
          battle.
Fixed:    The honor pollution agreement request does not set an embargo if
          accepted.
Fixed:    If the AI considers whom to go to war with, it considers humans and
          robots alike.
Fixed:    The AI unloads cargo from transport helicopters when it is at an army
          grouping point, so that the cargo units can be grouped.
Fixed:    The AI does not try to group land units into sea transporters anymore.
Changed:  Removed the CD-check, so that the retail version from gog.com runs
          with the Apolyton Edition.

2010-02-20 (Revision 1055)
Fixed:    The AI does not check its city limit, if city limit is disabled.
Changed:  The new game rules screen has been cleaned up and enhanced with
          descriptions of what the single rules are good for.
Fixed:    Possible crashes due to missing city styles have been fixed.
Fixed:    City turns to growth and production display now updates on begin
          turn and on city growth.
Fixed:    For modders, an advance can only have four EitherPrerequisites and
          Prerequisites combined (this is a limitation of the Great Library)
Changed:  EitherPrerequisite advances are shown in blue in the Great Library.
Changed:  For modders, combining EitherPrerequisites and Prerequisites in the
          same advance.
Fixed:    The AI now rolls over enemy units, if it is at war with them and the
          army is strong enough. This way the clears the way to an enemy cities
          and solves a problem it has with ZOCs.
Fixed:    If a units gains a health bonus the health bar does not overflow
          anymore.
Fixed:    If a unit gained a health bonus from a wonder and the wonder is lost
          the unit's health is reduced to the health without wonder.
Fixed:    If the AI needs for a goal a transporter it takes the transporter
          army pair that are closest to each other, this way the AI sticks to its
          decision and does not change transporters and armies for a goal all the
          time.
Fixed:    If a goody hut gives an advance then EitherPrerequisite and Prerequisite
          are now checked whether a player can have an advance.
Fixed:    Bombard range now uses all ranged boni used in normal combat and
          not just the veteran boni.
Fixed:    The AI now builds every turn tile improvements and roads.
Improved: The AI consider roads along paths that it had layed down in the same
          turn.
Changed:  The AI prefers to build gold tile improvements if it does not need to
          build food or production tile improvements.
Changed:  The AI builds in small cities tile improvements firs.
Changed:  The AI rush buys items in cities without garrison first, then it
          rush buys items in small cities.
Added:    The Ai now has build lists for small cities and cities near the 
          maximum size, so that it can build the right buildings there.
Fixed:    If map auto center is off the map does not center anymore when
          something is pillaged.
Fixed:    The game does not crash id you load a build queue into a city build
          queue that has been previously empty.
Fixed:    Cities that prevents slavery will only reduce slave raid change by
          the given percentage instead of stopping it entirely.
Changed:  Increased settle values for dessert and swamp so that the AI will
          eventually also will these areas, so less space for humans.
Fixed:    Tiles with tunnels do not use the move costs of deep sea for ships,
          instead they use the move costs of their terrain.
Fixed:    The AI calculates the goal priorities correctly, even if they are in
          different order in startegies.txt and goals.txt.
Fixed:    The AI stops building settlers if there is no space left.
Fixed:    The AI uses surplus settlers to increase the size of small cities.
Fixed:    The AI builds land settlers and sea settlers at the same time.
Fixed:    The game does not hang if it tries to optimize the sliders for a
          civilization that will starve when the happiness requirements have
          been met.
Fixed:    The AI does not build ships in that do not have access to an ocean
          but only access to a puddle.
Fixed:    The AI conquers empty cities with armies containing settlers.
Fixed:    The AI builds pollution reduction buildings in dirty cities.
Fixed:    The AI does not try to refuel units in cities that are full.
Fixed:    The ranking graph does not crash if the civilisations dead or alive
          is bigger 32.
Added:    The ranking tab now has a wonder line graph.
Fixed:    The AI builds slavers, or abolitionists, spies, and other special
          units at the same time.
Changed:  Cyber Ninjas have a higher elite change when they investigate a city.
Fixed:    Spies steal advances from cities of civilization that have advances
          to steal.
Fixed:    The AI builds happy buildings in unhappy cities.
Changed:  The AI builds wonders in high production cities.
Fixed:    The AI calculates its city ranks correctly, so that it does not
          almost all its cities to the wonder building list.
Added:    Added for each AI personality an island strategy, that will use an
          island specific advance list.
Restored: The Penicillin wonder now does what it did in the original game.
Changed:  The graphics are now drawn on a back buffer this removes drawing
          artifacts and saves your eyes.
Added:    The minimap has a button to show you the players' capitals.
Added:    The minimap has a button to show your relations with other
          civilisations. Blue is for you and your allies, green is for those
          you are at piece with, yellow is for the neutrals, white for those
          you do not know, and red is for those you are at war with.
Fixed:    Fixed a problem with AI transporter handling, that also may lead to
          a crash.
Fixed:    Fixed a problem when fort vision is removed because the tile under it
          had died.
Fixed:    The drawing system does not try to use an invalid memory pointer,
          this led to flicker artifacts at best and crashed at worst.
Fixed:    If a mod does not specify a tile improvement sound than the game
          handles it properly.
Fixed:    The AI does not try to use boats that cannot reach the target continent.
Fixed:    The game does not crash if a unit with a flag from a dead civilisation
          should be drawn. (Units of dead civilizations are not removed when 
          a civilization dies but later)
Fixed:    The mouse scroll wheel now works in all list boxes.
Fixed:    The new minimap buttons also show up in the French, German, Italian,
          Japanese, and Spanish version.
Fixed:    If you receive a map then all tiles that the giver sees or own are
          updated.
Fixed:    The AI does not use transporters carrying units that cannot be used
          for the goal that needs transport.
Changed:  Player vision handling, the human player uses the vision of his/her
          player directly instead of a copy, this saves memory.
Changed:  Lowered the AI priority for investigate city goals. There ar more
          worthy goals out there.
Fixed:    When the game is closed, the memory is cleaned up like in the debug.
          This fixes a potential crash when leaving the game.
Added:    New database flags for modders:
- DifficultyDB:
   TechCostKnownDeduction 0.0 - 1.0
         Deducts a percentage of an advance's cost based on % of total
         civs in the game that you know that already have the advance.
- UnitDB:
   RangedDefendCityBonus     Unit gets a ranged bonus in city defence.
   RangedAttackCityBonus     Unit gets a ranged bonus in city attack.
   IsSurfaceShip             Unit is a ship on the water surface.
   DefendSurfaceShipBonus    Unit gets a defence bonus for being a surface ship.
   AttackSurfaceShipBonus    Unit gets an attack bonus for being a surface ship.
   RangedSurfaceShipBonus    Unit gets a ranged for being a surface ship.
   AlwaysHeal                Units heals always even if it had been moved.
- GoalDB
   SlaveryProtectionBonus    Goal gets a bonus if it is protected against slavery.


2009-07-25 (Revision 1013)
Fixed:    Army grouping via slic in the Magnificant Samurai scenario works now.
Fixed:    Database errors do not show up anymore when the Magnificant Samurai
          scenario is loaded.
Fixed:    After an unit upgrade, its information is updated properly.
Fixed:    The game does not crash, if a civilization is conquered. (Another bug)
Added:    Cities show on the main map optionally what they are building.
Added:    A pollution main map icon for dirty cities.
Changed:  Other main map city icons have better transparency and outlines.
Changed:  Some good sprites were replaced so that they can be better seen.
Changed:  New modern city styles have now all the same color so that the 
          city styles cannot be distinguished so well anymore.
Fixed:    The game cannot be one anymore by just building the Solaris project.
          For archiving the science victory all the other components of the
          Gaia Controller are now necessary, too.
Fixed:    The AI does not consider tiles for new cities anymore if those
          tiles are already controlled by other cities.
Fixed:    The messages for nano infected and nano destroyed cities address
          now the victim player and do not throw slic errors anymore.
Changed:  The main map city icons for bio infection, nano infection, injoin
          and happiness attack now show the civilization color of the attacker.
Fixed:    The feat that is supposed to give extra science now gives extra
          science.
Fixed:    The feat that is supposed to increase the unit hit points now
          increases the unit hit points.
Added:    A political map option was added to the minimap.
Fixed:    When a city is founded inside foreign territory it adds national
          borders as it should do.
Fixed:    The AI does not unload units from transporters on the target continent
          before the final landfall.
Fixed:    Sea transporter paths do not end one tile too early.
Changed:  Unit support percent is now zero if production and unit support is
          zero.
Fixed:    Units disbanded in the city window will now give their production
          costs to back to the city, and the city window is updated accordingly.
Fixed:    When a city grows, the AI updates the values of positions for new
          cities, properly.
Changed:  The Empire Manager shows now more information.
Changed:  Crime for science is now calculated as all the other resources.
Fixed:    Wonders that are supposed to give a flat food bonus give a flat food
          bonus instead a food percent bonus.
Changed:  Laborers now benefit from the government coefficient, too.
Fixed:    Changing between buildings, wonders, and units costs now 25% of the
          stored production.
Fixed:    Database errors and Great Library errors in the Apolyton Edition
          scenario were removed.
Changed:  Production that is left from one item in the city build queue can be
          used for the next build item.
Changed:  When a city is captured all the stored production is lost.
Added:    New database flags for modders:
- UnitDB:
   VictoryEnslavementChance Unit's chance to enslave an enemy unit on victory
   GovernmentOnly           Unit can only be built with the given government.
                               The Unit is not destroyed on government change.
- TerrainImprovementDB:
   HealRate                 TerrainImprovent heals units on it with the given
                               rate.

2009-06-01 (Revision 990)
Fixed:    The game does not crash, if a civilization is wiped out.
Fixed:    The game does not hang if the AI cannot find enough transporters for
          a goal like sieging a city.
Added:    More of the government modified functionality for buildings and
          wonders was implemented.
Fixed:    The AI regard for another player is recomputed when first contact
          is made. This prevents the AI to give the human player a map exchange,
          a peace treaty, etc. on the first turn of contact. Now the human player
          has to build up trust, first.
Fixed:    Units with zero movepoints cannot move after an upgrade anymore.
Fixed:    The game now updates the displayed unit information including its picture.
Added:    New database flags for modders:
- UnitDB:
   UpgradeAnywhere         Unit can be upgraded outside of cities and forts
   UpgradeDoesNotHeal      Unit does not regain fuel, HP, and movepoints
                           after an upgrade.

2009-05-17 (Revision 981)
Fixed:    The AI does not try to use units for conquest if those units were
          assigned for city garrison. Such a attack does not fail anymore.
Fixed:    Cell unit strength computation was fixed so that the AI knows how
          much units it needs to conquer your cities.
Changed:  The empire manager now shows the actual numbers used for rations and
          wages.
Changed:  The original gameplay was restored, including terrain, units, elite
          units were disabled, and the trade bonus through cities was removed.
Changed:  Upgrade paths now match those of the original Apolyton Pack mod.
Fixed:    The Great Library now shows the right information about terraforming.
Fixed:    Active defense was added to the units destroyer, plasma destroyer,
          leviathan, morey striker, and interceptor to match the Great Library.
Changed:  "Smooth borders" option graphics to just show thicker borders so they
          are more visible than the original.
Fixed:    Trenches are generated again when a map is created and if
          PERCENT_TRENCH in Const.txt is zero. PERCENT_TRENCH is set to zero
          until we have converted the CTP1 trenches to CTP2 trenches.
Fixed:    The map is now generated with a proper distribution of hills and
          mountains.
Fixed:    The AI garrisons its just conquered cities, properly.
Fixed:    The game does not crash anymore if a new civ is added and that civ is
          destroyed immediately and another new civ is created then again.
          May happen through slave uprising.
Added:    A "New Combat" option was added to the rules screen. If it is on, front
          line units of a defending army will use their defense stat (rather
          than attack) to attack the front line units of the attacking army.
Changed:  Units with defense > 0 but attack = 0 no longer die immediately, and
          will at least defend themselves. If New Combat rule is enabled they
          will also counter-attack in a battle. Only an army that contains units
          all with attack = 0 and defense = 0 will die immediately.
Added:    The slic functions CreateBuilding and CreateWonder were added. The
          difference with events  with same name is that these functions do
          not effect stored production in a city. And unlike the CreateWonder event,
          CreateWonder function does not trigger a wonder movie.
Added:    "City Land Attack", "City Air Attack" and "City Sea Attack" where
          added to the battleview window to show these the city building
          bonuses during battle.
Fixed:    Many unit bonuses used in combat that were not working at all before.
Added:    Added the slic function UnitMovementLeft(unit). It returns an integer 
          of the unit's movement remaining. 100 = 1 move, 33 = a single road move etc.
Fixed:    ActiveDefence to only fire when there's a valid target and still alive.
          This removes the exploit of making a full army of active-defender's
          waste their active defense on one unit.
Removed:  Counter-bombard and counter-active-defense units do not fire anymore
          after an active defender has fired, this removes the big disadvantage
          of being the defender, and it was very cheesy to look at.
Fixed:    Units that have Attack=0 cannot attack anymore even if they have the
          "CanAttack" flag (Cyber Ninja for example).
Changed:  Bombard so bombarding units and defenders use same bonuses
          (such as terrain defense, veteran etc) as they do in normal combat.
Fixed:    The GetNeedsIrrigation flag now finds irrigation squares next to a city
          with a irrigation source of another tile improvement.
Changed:  The grid color was changed from white to black to match the other
          zoom levels.
Fixed:    Movebonus ("all terrain as x") units.txt flag.
Added:    A no ruins option was added to rules window.
Changed:  The government comparison tab of the empire manager displayed more
          detailed information, and displays yet researched researched
          governments so that you can compare them with your current government.
Changed:  The AI goal calculation is now also unit dependent and the AI was
          sped up.
Changed:  The city manager was redesigned to display all information from the
          tabs in one window.
Added:    Some tile graphics and all the rules for submarine canyon/trench
          tiles were added.
Changed:  The UpgradeTo flag does not obsolete units anymore.
Fixed:    The game does not hang anymore if the AI tries to load a transporter
          that cannot move to its load position.
Added:    A random map settings option was added to the map settings window.
Fixed:    The "activate" and "disband" buttons in the city manager will be
          after disbanding a unit.
Changed:  The diplomacy manager and diplomacy proposal window were resized to
          fit on 600 pixels high resolution again. The width of the proposal
          window were increased and its layout were modified, for easier
          modification later.
Fixed:    The Diplomat photo now shows up when the AI sent you a proposal.
Changed:  The advance options window was cleaned by removing options already
          present in other windows. Also other option windows were cleaned up.
Added:    A Custom start and end ages option was added to single-player rules
          screen.
Fixed:    The map does not auto-center on moving stealth units that you can't see.
Changed:  Elite medal icon now replaces veteran medal icon, rather than sitting
          below it.
Changed:  No units can be expelled from a tile anymore if there is at least one
          unit that cannot be expelled.
Fixed:    The AI uses any units inside a newly founded city for garrison.
Changed:  The AI uses less units for city garrison for the higher higher 
          defense levels. This is closer to the original game.
Changed:  An enemy transporter does not show the cargo icon anymore if it only
          carries stealth units.
Fixed:    If you click on a hidden enemy stealth unit or on another kind of 
          unit or city inside the fog of war your units or cities will
          deselected.
Changed:  Slic event debug messages are now off by default.
Added:    The AI will free slaves when it conquers a city if it neither owns 
          slaves nor it has any slavers.
Fixed:    The AI builds caravans again.
Fixed:    Rare crash at start-up (race condition with mouse initialization).
Fixed:    The game does not crash anymore if the riot casualties option is used.
Changed:  The map does not center on a pirate event if AutoCenter is off.

2008-10-08 (Revision 907)
Fixed:    The tile highlight square at the mouse pointing position does not
          blink anymore. In general, blinking items draw attention even if
          there is nothing important. In that case it was blinking always at
          the of the GUI repainting speed, which just makes the player tired.
Fixed:    The AI now uses the right settle city bounding rectangle for settling.
Changed:  The AI only considers a city in its goal assignment as unexplored if
          no tile improvement is visible. That is the same way as a human player
          behaves, since if he sees a tile improvement without a city then he
          concludes it must be in the dark.
Added:    Extra upgrade command for the order button bank. The upgrade command
          has to be enabled before the game start from special rules screen.
          The upgrade command button is only visible if an army is active
          that contains an upgradable unit.
Fixed:    The AI does rebuild its capitol if it has lost its capitol.
Fixed:    The ration, workday, and wages sliders now add happiness, even if
          the overall slider happiness is positive. This behavior is the 
          expected one, even if the programmers capped the total increase,
          originally.
Changed:  Cities are now watchful for seven turns like in Cradle, so that
          slavers go more to other cities.
Changed:  If the AI build for offense it builds city walls first. Makes its
          defense stronger and protects from slavery.
Fixed:    Slaves do not go to cities if those cities are at the population
          maximum.
Fixed:    Slave raid failure message now displays the target city correctly.
Fixed:    Slavers do not go to cities with walls.
Fixed:    The AI does not consider boats with cargo as defenders although
          the cargo cannot defend. (e.g. Slavers)
Changed:  Cleaned up various AI code files to improve code reliability and
          speed.
Fixed:    The game does not crash anymore if a new civilization has the same
          player number that was used by a civilization killed previously.
Fixed:    A crash while the game checked whether a unit can settle.
Changed:  Instead of moving units first to a goal and then canceling the goal
          if it has not enough units when the units were already moved, it now
          does not send its units to those goals at all.
Changed:  The AI now considers all goals on the base of nearby units.
Changed:  The AI uses an algorithm for rallying troops that works.
Fixed:    Slavers now go to cities with at least to population points.
Fixed:    Some problems in multiplayer, but not all.
Changed:  Some settings in goods.txt and DiffDB.txt for the Apolyton Edition Mod.
Fixed:    A crash with missing population assignment data.
Fixed:    The AI can now settle more than one city per turn.
Fixed:    A potential crash in the char window.
Fixed:    A bug caused by adding new civilisations during the game.
Fixed:    The AI strategy is restored after a game reload, this fixes the AI's
          settling behavior for instance.
Fixed:    A bug that makes the game crash if a unit is killed after a reload
          before it had its turn.

2008-05-24 (Revision 875)
Fixed:    The progress bar in the city manager, so that it does overlap.
Fixed:    The edges of the borders of the load/save window.
Fixed:    AI force matching. AI force matching is now based on attack,
          defense, bombard, ranged, and foreign troop vales instead of just
          threat.
Fixed:    Non-Combat units cannot attack anymore.
Fixed:    AE mod loads without error pop ups
Fixed:    Fortified units will be defortified if they are put to sleep.
Fixed:    Sleeping units will be awaken if fortified.
Fixed:    The first save game in PBEM is now labeled with turn 0.
Fixed:    Prevented some crashes with patch 1.1 save games.

2008-04-12 (Revision 866)
Added:    A settle in city event, so that the player can add a pop via settling
          in a city without interfering with the AI.
Fixed:    A newly founded city does not show up as underwater city when it is
          founded.
Fixed:    Barbarians attack someone else if you are protected by the Great Wall.
Fixed:    A combat event error pop up when you close the battle window.
Fixed:    The map is only centered if the AutoCenter option is enabled.
Fixed:    The city control panel is updated even if the visible player is a
          robot.
Fixed:    The slic file path is stored even if slic debugging is set to no. This
          fixes a crash but possibly needs slic reloading to have an effect on
          on games saved before this version.
Changed:  Exposed the tile set indices of the road tiles to tileimp.txt.
Fixed:    The AI selects a new advance or a new government in the same turn
          when they become available.
Fixed:    A bug that prevented the AI from attacking.
Fixed:    At debarking, a new army is only created if it can be filled from
          the ship with at least one unit. This removes an event error
          concerning creating empty armies.
Fixed:    An event error concerning waking units.
Fixed:    RiotCasualities method: This may fix a crash.

2008-02-10 (Revision 849)
Fixed:    The single player civilization screen updates the leader names again.
          It displays the civilizations in alphabetical order again,
          in scenarios only selectable civilizations are displayed, and the game
          is started with the selected civilization.
Added:    The AI road pathing can now be visualized if an according option is on.
Fixed:    The AI now builds a road network from its threatened cities.
Fixed:    The AI now adds special units to the build list even if not all
          garrison units are build.
Fixed:    The nationality of slaves who are working in a city are now displayed.
Fixed:    City icons can be seen by everyone again.
Added:    If an AI army is in the vision range of a goody hut, the goody hut gets
          an high goal bonus.
Changed:  Slaves go to more than the closest city.
Fixed:    The AI now sorts its plans according their match values, plans that
          need an high amount of units get an extra bonus.
Changed:  If you try to move an army into a transporter with less room than the
          army has units, the army is partially moved into the transporter.
Fixed:    The chat window command /attach p now turns as it should be the
          player p into an AI. This allows watching the AI while it is playing.
Fixed:    The initial city interface is no more shown if the visible player 
          is an AI.
Fixed:    Autosaves are now done even if the visible player is an AI.
Fixed:    The game can now be saved even if the visible player is an AI.
Fixed:    The AI fills now also empty build queues at the end of a turn so that
          newly founded and captured cities do not end turn with an empty build
          queue.
Fixed:    Settlers now display the full first ring of a city.
Fixed:    Non-Settler units cannot settle on top of a city anymore.
Fixed:    The AI now checks all its cities for rush buying of items.
Fixed:    The AI does not use the move costs at the current position and 
          the target position anymore to estimate the distance.
Fixed:    The AI calculates now its transporter capacity before it needs it
          and not after planning its goals.
Fixed:    The AI uses now the transporter that is closest to the troops to be
          transported and not the transporter that is closest to the target.
Fixed:    The AI does not try to use full transporters.
Fixed:    Now the AI calculates its garrison needed for guarding slaves correctly.
Fixed:    The AI does not try to move its sea transporters over land to reach 
          the troops that have to go on board.
Fixed:    The AI starts to move their transporters to the goal on the same turn
          when they have been filled.
Fixed:    If the AI needs for a stack more than one transporter it now uses more
          than one transporter.
Fixed:    Holding diplomatic receptions costs now the displayed amount of gold.
Changed:  The unit herald is now a shield like in Civ2 and CTP1.
Added:    Messages for failed slave raids with and without dying.
Removed:  A superfluous message that one of your cities was captured.
Fixed:    Expelling now removes move points.
Changed:  The domestic tab of the intelligence screen now shows the capital if
          you know were it is or if it has a wonder.
Changed:  The domestic tab of the intelligence screen now shows instead of cities
          with wonders all cities if you have an embassy otherwise the cities
          with wonders and the cities you know are shown.
Added:    New database flags for modders:
- StartegyDB:
   UseBaseMoveCostsForRoads         Strategy makes the AI to plan roads by using
                                    the base terrain costs instead of the 
                                    terrain move costs including terrain 
                                    improvements if its value is not zero. Its 
                                    default value is 0 and taken from ConstDB.
   BuildRoadsToClosestCities        Strategy makes the AI to plan roads from 
                                    each to n other cities. The default value
                                    for n is 5 and taken from ConstDB.
   BaseRoadPriorityVsThreatRank     Strategy gives the AI a base road build 
                                    priority to that the threat rank is added.
                                    Its default value is 0.01 and taken from 
                                    ConstDB.
- ConstDB:
   UseBaseMoveCostsForRoads         Default value for the flag in StrategyDB.
   RoadAlreadyThereCostsCoefficient The part of costs the AI deducts for tiles
                                    along a path that already has roads.
                                    The default value is 0.5.
   BuildRoadsToClosestCities        Default value for the flag in StrategyDB.
   BaseRoadPriorityVsThreatRank     Default value for the flag in StrategyDB.
   ConiderNumCitiesForSlaves        Const gives the number of closest cities
                                    that are considered to send the captives
                                    of a slave raid.
- GoalDB:
   InVisionRangeBonus               Goal adds a bonus if the target is in the
                                    vision range of an army.
   NoBarbarianBonus                 Goal adds a bonus if a target is in vision
                                    range if no Barbarians are around (i.e.
                                    no Barbarians can pop up from a goody hut).
   CanAttackBonus                   Goal adds a bonus for armies that can attack
                                    if the target is in vision range and 
                                    Barbarians are around. (i.e. Barbarians can
                                    pop up from goody huts).

2007-12-14 (Revision 836)
Fixed:    AI now bombards all nearby units if it has the opportunity.
Fixed:    AI does not try to fortify units that are scheduled to be disbanded.
Fixed:    AI does not try to initialize diplomatic state with dead players.
Fixed:    AI now stops pirating trade routes properly.
Fixed:    Removed some more events calls with invalid arguments.
Changed:  The chat window command /importmap reports if you try to import a map
          from a not existing file and reports if the map size of the imported
          map does not match the map size of the loaded map.
Fixed:    The turns until a city grows are now correctly displayed on the map
          when you modify the farmer assignment.
Fixed:    The turns until a city grows are now correctly displayed on the map
          when you modify the empire slider settings.
Fixed:    The key mapping screen now displays the correct key.
Changed:  Increased the number of player in a PBEM or HotSeat game to 32.
Changed:  Moved the experimental religion stuff to its own AE scenario.
Fixed:    The city sprawl code should now work.
Added:    New database flags for modders:
- ConstDB:
   TurnsAcceptedForOnePop Const makes the AI to add more farmers to a city if
                          the city needs more than the specified number of turn.
                          This flag provides the default value for those in
                          the StrategiesDB.

2007-11-19 (Revision 826)
Fixed:    GetPersonalityType slic function
Fixed:    PBEM terrain improvement construction
Fixed:    Barbarians do not build wonders anymore
Fixed:    Shadowed tiles are not displayed as they would have a river anymore
Changed:  Global Warming and Ozone Depletion code is now executed from events.
Fixed:    World meridian calculation
Fixed:    Made world meridians symmetrical.
Fixed:    Global warming does not change deserts.
Fixed:    Global warming makes snow on white mountains melting.
Fixed:    The AI targets of the orders Sue and Injoin
Fixed:    The equatorial meridian now also suffers ozone depletion.
Fixed:    The unit mobile SAM is now a full bombard unit.
Added:    Armenia and Kurdistan as civilisations
Added:    Rudimentary religious feats wonders, buildings, and units.
Fixed:    Unit grouping: Units have to be on the same tile to be grouped. This
          fixes insecure slic code.
Fixed:    River mouths are not deleted due to global warming or ozone depletion.
Changed:  Increased richness of mountains and rivers on fresh created maps.
Changed:  Slic events are tested for valid arguments, if one argument is invalid
          the event execution is stopped and an error message is displayed.
Fixed:    ClickedArmy event has now a valid army if the clicked army is not
          owned by the clicker.
Fixed:    ZOCs are now cleared again.
Changed:  Revered the show order of diplomatic proposals in human-human
          diplomacy in PBEM and HotSeat.
Fixed:    Rush buy in PBEM.
Changed:  German grammar about civilisations, units, and goods.
Fixed:    Diplomacy greetings are sent to all players also robot players. This
          fixes a bug in PBEM and HotSeat that prevented the human player not
          on screen to receive the greeting. (Fix is untested)

2007-10-20 (Revision 812)
Fixed:    The wonder tab of the info window shows again the right construction 
          year of wonders, also the date of messages is shown correctly again.
Fixed:    The diplomacy window cannot be closed by using hotkeys that open other
          screens, so that you cannot close this screen without rejecting or 
          accepting diplomatic proposals.
Fixed:    The scenario editor cannot opened in PBEM mode per hotkey.
Added:    HotSeat and PBEM human to human diplomacy support.
Added:    The following civilisations: Han, Yamato, Sumer, Kush, and Macedonia
Fixed:    The map editor generates now the coast line again if needed.
Fixed:    Savegame power graphs

2007-08-07 (Revision 787)
Changed:  Replaced the old ConstDB by a new one.
Changed:  Harappan flag now is Pakistan's flag.
Changed:  Jamaica headers that mapped to Poland are now labeled as Poland
Added:    Jamaica is now a civ (as it was in CTP1)
Fixed:    Crash caused by CitySpecialIcons (will rework)
Fixed:    Tribe screen no longer shows barbarians and works
Changed:  Greek flag is now from the 19th century (easier to see)
Changed:  Strategies.txt and Goals.txt to modify the AI

2007-08-02 (Revision 785)
Changed:  Empire selection start up now with a list box
Fixed:    Multiplayer window loads (but lobby cant be reached)
Fixed:    Custom scenario turns from turnlength.txt are shown again.
Deleted:  ShowCityIcon from BuildingDB and WonderDB replaced with new flags
Added:    New database flags for modders:
- BuildingDB:
   ShowCityIconTop      Building displays this icon on the map above its city
   ShowCityIconBottom   Building displays this icon on the map below its city
- WonderDB:
   ShowCityIconTop      Wonder displays this icon on the map above its city
   ShowCityIconBottom   Wonder displays this icon on the map below its city

2007-07-25 (Revision 773)
Changed:  TargetCivilians flag now uses a probability like normal bombard
Changed:  PrecisionStrike flag now uses a probability like normal bombard
Added:    Cities on a trade route receive gold if neither the trade route
          belongs to the city owner nor the destination city is owned by
          the same civilisation
Changed:  Byzantine diploflag and unit flag
Fixed:    Error in implementation of Elite and Leader code
Added:    Religion icons for modders
Changed:  Difficulty and Risk level new game pop-up is now a listbox
Added:    New database flags for modders:
- ConstDB:
   CITY_ON_TRADE_ROUTE_BONUS
                        Const gives a city a trade route bonus if such a trade
                        route passes through the city
   CAPTURED_CITY_KILL_POP
                        Const specifies how many civilians are killed
                        when city is captured

2007-06-09 (Revision 745)
Changed:  The Army stacking flag is no longer displayed above the health bar
Added:    OnePerCiv buildings are destroyed if their city is captured
Added:    Elite Status, a promotion above Veteran Status, has double
          Veteran coefficient in combat
Added:    Stacked Leaders double defense or offensive boni
Fixed:    Slic Message for Guerrilla Spawn
Fixed:    Slic Message for Insurgent Spawn
Fixed:    Slic Message for Sinking Ships
Fixed:    MaxCityWonders works now as intended
Fixed:    MaxCityBuildings works now as intended
Added:    New database flags for modders:
- ConstDB:
   COMBAT_ELITE_CHANCE  Const gives a chance a veteran unit
                        is promoted to elite status
   COMBAT_LEADER_CHANCE Const gives a chance that a leader appears
                        if an elite unit is victorious in combat
- BuildingDB:
   EnablesAllVeterans   Building makes any unit built in a city a veteran.
   EnablesSeaVeterans   Building makes any sea unit built in a city a veteran.
   EnablesLandVeterans  Building makes any land unit built in a city a veteran.
   EnablesAirVeterans   Building makes any air unit built in a city a veteran.

2007-05-19 (Revision 735)
Disabled: Sectarian happiness functionality has been removed
Fixed:    Unit Upgrading now updates the army movement type

2007-05-16 (Revision 732)
Disabled: MaxCityWonders because it caused a crash
Disabled: MaxCityBuildings because it caused a crash
Added:    New database flags for modders:
- BuildingDB:
   NeedsFeatToBuild     Building needs feat achieved by owner to be built.
   NeedsAnyPlayerFeatToBuild
                        Building needs feat achieved by any player to be built.
- UnitDB:
   NeedsFeatToBuild     Unit needs feat archived by owner to be built.
   NeedsAnyPlayerFeatToBuild
                        Unit needs feat archived by any player to be built.
- WonderDB:
   NeedsFeatToBuild     Wonder needs feat archived by owner to be built.
   NeedsAnyPlayerFeatToBuild
                        Wonder needs feat archived by any player to be built.
- WonderDB:
   ProhibtSlavers       Wonder prevents building of additional slaver units.
Added:    New Options
- Rules Screen
   No AI City Limit     Government City Limit happiness effect for AI can
                        be toggled on/off
   No City Limit        Government City Limit happiness effect can be
                        toggled on/off

2007-05-01 (Revision 726)
Fixed:    Humans playing as Barbarians cannot declare war from diplomanager.
Changed:  The Alexander scenario now uses an improved AI strategies.txt.
Fixed:    In the Alexander scenario TerrainImprovemnts have now a sound.
Fixed:    The Samurai Scenario should now be playable.
Fixed:    The Great Library search function does not find hidden database
          records anymore.
Fixed:    Only one worker is subtracted when a settler is built (instead of two)
Added:    Profile option that make razed cities to leave a ruin
          TerrainImprovement
Added:    New database flags for modders:
- ConstDB:
   MAX_CITY_WONDERS     Const sets the max. number of wonders a city can build.
   MAX_CITY_BUILDINGS   Const sets the max. number of buildings a city can build.
- WonderDB:
   BuildingEffectEverywhere
                        same as BuildingEverywhere but flag is more                  # So it is a plain simple copy of it? It's even not a synonym for the same flag. For some reason I added that aka (as known as) keyword to *.cdb files. #this flag help distinguish it from ACtualBuildingEverywhere. So should I delete this then?
                        explanatory      # This does not have the same effect. - Its a copy of it. ActualBuildingEverywhere is the other one #
- TerrainImprovementDB:
   RiverOnly            TerrainImprovement can only be built on a river.
   IsUrban              TerrainImprovement is urban.
   IsIrrigation         TerrainImprovement is irrigation.
   NeedsIrrigation      TerrainImprovement must be build one square from
                        IsIrrigation TerrainImprovements or a river.
   NextToCity           TerrainImprovement must be built 1 square next to a city
   IsWonder             TerrainImprovement can only be built once.
                        It must be built in a square owned by
                        the city that built the wonder.
Added:    New Options
- Rules Screen
   AImilitia            Empty AI cities generate a cheap unit at the
                        beginning of a turn
   No AI gold deficit   AI city gold income does not drop below 0
   No AI production deficit
                        AI city production does not drop below 0
   Gold per city        City gold costs multiplied by the city limit
   Gold per unit        Unit gold costs per turn multiplied by the wage rate
   Aicitydefense        AI cities have a defense increase based on population
                        size
- Scenario Editor
   DebugAI Button       toggles showing the AI unit's goals
- Gameplay Screen
   ShowEnemyHealth      toggles showing AI health bar
   ShowDebugAI          toggles showing the AI unit's goals

2007-04-09 (Revision 719)
Fixed:    Bug that made human units automatically upgrade
Fixed:    Bug that prevented disbanding of units in neutral territory
Fixed:    Diplomacy screen size was reduced
Added:    Nation Flags replaced diplomacy leader photographs
Changed:  BarbarianSpawnBarbarian code to limit Barbarians overrunning
          the entire map
Added:    DebugAI profile option - displays AI goals for units

2007-03-22 (Revision 707)
Fixed:    Rules screen enabled with new play options
Added:    Effects of SectarianHappiness are displays in the CityWindow
Added:    New Options
- Rules Screen
   One City Challenge   Human player cannot build settlers
   City Capture Options slic message with option on/off
   Revolt Insurgents    Revolting cities spawn Barbarians
   RevoltCasualties     Revolting cities lose random population
   BarbarianSpawnBarbarian
                        Barbarian units spawn more Barbarian units (risk dependent)
   SectarianHappiness   Causes more unhappiness in cities of a different style
- Graphics screen
   Smooth Borders       Show original pixel line or border icons
- Gameplay Screen
   City Capture Options slic message can be toggled on/off
Changed:    Database flags for modders:
- TerrainImprovementDB:
   BonusFoodExport      TerrainImprovement randomly generates a food value that
                        is divided equally to each city
   BonusGoldExport      TerrainImprovement randomly generates a gold value that
                        is divided equally to each city

2007-03-12 (Revision 705)
Fixed:    Crash caused by SectarianHappiness because the founder civ was 
          destroyed
Fixed:    Crash caused by Sink code
Added:    Graphic window option to show army names
Added:    Graphics window option to show civilisation flags
Added:    New strategies.txt that emphasizes more city captures
Fixed:    Location of Nation Flag is now right of the herald
Deleted:  Old IsReligion# system

2007-03-01 (Revision 693)
Added:    Hostile terrain healthpoint cost is now based on barbarian risk.
Fixed:    SpawnBarbarian code for entrenched units that can spawn Barbarians.
Added:    New userprofile option to show civflags under the player color flag.
Fixed:    Civ flags moved below player color flag.
Added:    TerraformOcean button in tile bank. This allows modders to use
          up to 60 of the 64 available tileimp slots (previously only 48/60)
Added:    Slic messages for sinking ship (again), hostile terrain, insurgent
          spawn, guerrilla spawn
Added:    New concept of energy supply and demand. It calculates a ratio used
          as a modifier for gold and production.
Added:    New userprofile option to disable the new energy supply/demand
          concept.
Added:    New database flags for modders to enable the new energy
          supply/demand concept:
- TerrainImprovementDB:
   ProducesEnergy       TerrainImprovement generates energy
   EnergyHunger         TerrainImprovement consumes energy
- BuildingDB:
   ProducesEnergy       Building generates energy
   EnergyHunger         Building consumes energy
   ProducesEnergyPerPop Building generates energy per citizen
   EnergyHungerPerPop   Building generates energy per citizen
- WonderDB:
   ProducesEnergy       Wonder generates energy
   EnergyHunger         Wonder consumes energy
   ProducesEnergyPerPop Wonder generates energy per citizen
   EnergyHungerPerPop   Wonder generates energy per citizen
- UnitDB:
   ProducesEnergy       Unit generates energy
   EnergyHunger         Unit consumes energy

2007-02-22 (Revision 692)
Fixed:     Moved Goal text from appearing inside the science box in the advance
           research screen
Added:     Civilization Flags for Units (most artwork from Civ2 modders Michael 
           D McCart & Michael D Raney)
Added:     Unit Hidden Nationality - these units are displayed as barbarians

2007-02-01 (Revision 687)
Added:     Religious City Icons (up to 10 Religions) appear if building has
           IsReligion1(or 2 up to 10)
Added:     National Borders are now displayed with icons for smooth appearance.
Changed:   Aligned city name, population rectangle, and turns to population
           growth rectangle.
Changed:   Next Pop rectangle is now black like the city name to differentiate
           it from City Population.
Added:     Player Capitals now have a star icon on the map.
Fixed:     Implemented HasAirport icon.

2006-10-05 (Revision 647)
Fixed:    No new civ message if the city in questions joins another civ.
Added:    New database flags for modders:
- TerrainImprovementDB:
   BonusScience         TerrainImprovement gives bonus science to its city.
   HappyInc             TerrainImprovement increases happiness of its city.
- WonderDB:
   ActualBuildingEverywhere
                        Wonder creates this building everywhere.
- BuildingDB:
   CreatesMiltiaUnit    Building creates militia unit.
- DifficultyDB:
   AIMilitiaUnit        Difficulty creates militia units in empty AI cities.
   RevoltInsurgents     Difficulty creates Barbarians around rioting cities in
                        dependence on RiskDB BabarianChance.

2006-09-09 (Revision 634)
Fixed:    AI slider handling
Added:    For modders: STRATEGY_TOO_MANY_CITIES and STRATEGY_NO_REVOLUTION are
          loaded automatically if present in strategies.txt and not present in
          personalities.txt. This way modders have less work if they want to
          make the AI respect the city limit.

2006-08-26 (Revision 632)
Fixed:    Missing wonder started messages
Fixed:    Broken city happiness display
Fixed:    Autosave - Quicksave permutation
Added:    New database flags for modders:
- DifficultyDB:
   AIFreeUpgrade        Difficulty allows the AI to upgrade its units for free.
   AINoShieldHunger     Difficulty maintains AI armies without shields.
   AINoGoldHunger       Difficulty maintains AI armies without gold.
   GoldPerUnitSupport   Difficulty adds this * GoldHunger * wages * readiness per unit.
   GoldPerCity          Difficulty adds this * cities * CityLimit
- TerrainImprovementDB:
   CanBuildWasteland    TerrainImprovement can be built in unowned territory.
   CanBuildAlly         TerrainImprovement can be built in territory of your
                        allies. TerrainImprovements with move cost reduction do
                        not need this flag, because they can be built in ally
                        territory anyway.

2006-07-15 (Revision 619)
Fixed:    Units can now sink by using the ConstDB chance value.
Changed:  AIs war declaration messages have now pop-up status. (Pretty annoying)
Added:    AutoExpireTreatyBase to userprofile.txt to set the turns when treaties
          expire. Actually this is a cheat but necessary as long expiration is
          hard encoded.
Fixed:    Crash in games with more players than governments is the GovernmentDB.
Changed:  gw.txt has a new alternative format.
Added:    New database flags for modders:
- DifficultyDB:
   AINoCityLimit        Difficulty does not punish AI for city limit violation.
   AINoSinking          Difficulty makes AI ships unsinkable.
- ResourceDB:
   IsBonusGood          Good that not present in city radius can be enabled by
                        tileimp or building.
- AdvanceDB:
   EitherPrerequisite   Advance just needs one of the prerequisites to be 
                        researched. This allows a fluid tech tree like Civ4.
- TerrainImprovementDB:
   PrerequisiteTileImp  TerrainImprovement needs another tileimp to be built.
- WonderDB:
   EnablesPunativeAirstrikes Wonder allows bombardment without war declaration.
- CivilisationDB:
   A bunch of flags that E did not mention.
- AdvanceDB
   A bunch of flags that E did not mention.

2006-07-02 (Revision 602)
Fixed:    CanBombard
Fixed:    CanCloak
Fixed:    Crash concerning access units.
Added:    New database flags for modders:
- WonderDB:
   ExcludedByWonder     Wonder cannot be built with this wonder.
- BuildingDB:
   ExcludedByWonder     Building cannot be built with this wonder.
   NeedsFeatToBuild     Building needs a certain feat to be built.
- UnitDB:
   ExcludedByWonder     Unit cannot be built with this wonder.
   PrerequisiteWonder   Unit needs a certain wonder to be built.
   NeedsFeatToBuild     Unit needs a certain feat to be built.

2006-06-09 (Revision 587)
Added:    Capture City options: Occupy, Liberate(give to barbs), and Raze 
          (needs to be improved and should be more similar to those of 
          ApolytonPack)
Fixed:    Barbarians are not affected by hostile terrain
Added:    New database flags for modders:
 - BuildingDB:
   TreasuryInterest     Building gives a percentage of treasury of their 
                        civilisation to their civilisation.

2006-06-07 (Revision 586)
Changed:  CanExportTileValue now works as a radius if you want only one radius
          IntBorderRadius should be 0
Added:    New database flags for modders:
- TerrainImprovementDB:
   CanExportGood        TerrainImprovement that is a colony can export good in
                        its radius to a another city.
   SpawnBarbarians      TerrainImprovement spawns Barbarian units.
- DifficultyDB:
   BarbarainCamps       Difficulty's Barbarian camp TerrainImprovement.
   BarbarianCities      Difficulty allows Barbarian cities.
   BarabariansSpawnBarbarians Difficulty makes Barbarian units to spawn more 
                              Barbarian units.

2006-05-27 (Revision 577)
Changed:  Reform City destroys a building if you have a building that conflicts
          with SettleBuilding of the reforming unit
Added:    New database flags for modders:
- BuildingDB:
   IncreaseHP           Building gives extra HitPoints on first built.
   CantSell             Building cannot be sold.
   ExcludedByBuilding   Building cannot be built if this building is present.
- WonderDB:
   GoldPerBuildingAnywhere Wonder gives gold per specified building multiplied 
                           by population.
- GovernmentDB:
   HasGulags            Government enslaves each turn a citizen.
   IsCapitalist         Government turns per turn a citizen into a merchant.
   IsTechnocracy        Government turns per turn a citizen into a scientist.
   IsAgraraian          Government turns per turn a citizen into a farmer.
  HasMindlessTelevision Government turns per turn a citizen into an entertainer.
   IsXenophobic         Government reduces population of foreign cities by one 
                        each turn.

2006-05-24 (Revision 576)
Changed:  Removed War from slave attacks now just regard cost (Must be modified)
Fixed:    No more war popups for attacking Barbarian units.
Fixed:    Pillage of tile improvements in unowned territory.
Fixed:    Bomber movement now works like before.
Added:    New database flags for modders:
- UnitDB:
   HostileTerrainCost   Unit loses HitPoints in hostile terrain.
   ImmuneToHostileTerrain Unit is immune to costs of hostile terrain.
   SettleImprovement    Unit finishes with enough PW tileimps when entrenched.
   SettleBuilding       Unit builds building in a foreign city on conversion.
- DifficultyDB:
   Barbariancamps       Diff. makes an entrenching Barb. unit to create a city.
   SectarianHappiness   Difficulty reduces city happiness in conquered cities.

2006-05-16 (Revision 574)
Added:    Tileimps with colony and BonusProductionExport, BonusGoldExport, 
          CanExportTileValue flags can add shields outside player radius.
Added:    New database flags for modders:
- UnitDB:
   CanHarvest           Unit that can entrench adds terrain gold to treasury.

2006-05-13 (Revision 572)
Added:    New database flags for modders:
- DifficultyDB:
   AICityDefenderBonus  Difficulty gives AI cities a defence population bonus.
- UnitDB:
   NeedsFeatToBuild     Unit need a certain feat to be built.
   CivilisationOnly     Unit can only built by given civilisation.
- BuildingDB:
   NeedsFeatToBuild     Building need a certain feat to be built.
   CivilisationOnly     Building can only built by given civilisation.
- WonderDB:
   NeedsFeatToBuild     Wonder need a certain feat to be built.
   CivilisationOnly     Wonder can only built by given civilisation.
   OnePerCiv            Wonder can be build in every civ once.
- AdvanceDB:
   NeedsCityGoodAnyCity Advance needs a good in any city to be researched.
- ResourceDB:
   AddsASlave           Good collected in a city adds each turn a slave to this
                        city. Actually it is a stupid idea, because soon your
                        city is full of slaves.

2006-05-03 (Revision 571)
Added:    New database flags for modders:
- UnitDB:
   UpgradeTo            Unit can be upgraded to given unit with enough gold.
                        Upgrade is done by putting the unit to sleep in a city.
                        Note an own upgrade order is still missing.
- DifficultyDB:
   NoAIGoldDeficit      AI does not pay for gold deficits. (Optional AI cheat)
   NoAIProductionDeficit AI does not pay for prod. deficits. (Opt. AI cheat)

2006-04-29 (Revision 567)
Added:    New database flags for modders:
- UnitDB:
   TargetsCivilians     Unit bombard civilians instead other units.
   PrecisionStrike      Unit bombards buildings instead other units.
   MultipleAttack       Unit can attack repeatedly per turn with enough MPs.
- ResourceDB:
   AvailableAdvance     Good needs a specific advance to be collected by a city.
   VanishAdvance        Good cannot be collected with this advance owned.
   CantTrade            Good cannot be traded. It acts like a bonus.
   HappyInc             Good increases city happiness if it is traded.

2006-04-14 (Revision 560)
Added:    Pirates, unlike in Civ2 only boats with attack, no transports.
Fixed:    Disbanding a settler in a city does not disband all other units there.
Fixed:    Goody hut crash occurring if UnitDB is bigger than AdvanceDB.
Added:    New database flags for modders:
- UnitDB:
   CanCaptureTile       Unit captures an enemy tile instead of pillaging it. 
   CanBeGifted          Unit can be gifted to a non-enemy player. A diplomatic
                        regard bonus is awarded. It is based on attack divided
                        by five. (The five should go to ConstDB or DiffDB or 
                        CivilisationDB, etc.)
- TerrainImprovementDB:
   DeniedToEnemy        TerrainImprovement does not give move boni to enemies.

2006-03-31 (Revision 559)
Added:    New database flags for modders:
- UnitDB:
   ObsoleteUnit         Unit it is obsolete by another unit instead by a tech.
   MoveBonus            Unit specific move costs for all kind of terrain 
                        including roads. (Has to be fixed)
   GoldHunger           Unit needs this amount of gold per turn as support.
   GoldPerUnitSupport   Should be the same as above. (No idea what the difference is)
- WonderDB:
   BorderRadius         Wonder increases the border radius of the civ's cities

2006-03-22 (Revision 554)
Added:    Accidental move into an enemy or city causes a popup message for war 
          declaration confirmation. For now war must be declared from the 
          diplomacy screen. (To be fixed)
Added:    New database flags for modders:
- AdvanceDB:
   GoodyHutExcluded     Advance cannot be obtained from a goody hut.

2006-03-11 (Revision 553)
Added:    New database flags for modders:
- UnitDB:
   GoodyHutExcluded     Unit cannot be gained from a goody hut.
- TerrainImprovementDB:
   TerrainEffect
      EnablesGood       Tileimp on a certain terrain gives a good.

2006-03-03 (Revision 552)
Added:    New database flags for modders:
- BuildingDB:
   EnablesVeterans      Building makes city to build veteran units only.
- UnitDB:
   PopCostsToBuild      Unit costs a city these number of population points.
                        A city disband warning is still missing.

2006-03-01 (Revision 551)
Added:    Building upkeep is now affected by readiness.
Added:    New database flags for modders:
- BuildingDB:
   GoldPerUnit          Building adds amount of gold per unit to civilization.
   GoldPerUnitReadiness Building adds gold with readiness impact per unit.
   BuildingFeat         Building needs other building in percent of cities.
   SquaredBorderRadius  Building increases city radius like a fort.
   IntBorderRadius      Building increases city radius like a fort.
- UnitDB:
   MultipleAttacks      Unit can attack more than once per turn (unfinished).
- CityStyleDB:
   ProductionPercent    CityStyle gives a percentage production bonus.
   FoodPercent          CityStyle gives a percentage food bonus.
   CommercePercent      CityStyle gives a percentage gold bonus.
   SciencePercent       CityStyle gives a percentage science bonus.
   BonusGold            CityStyle gives a gold bonus.
   BonusFood            CityStyle gives a food bonus.
   BonusProduction      CityStyle gives a production bonus.
   BonusScience         CityStyle gives a science bonus.
- AgeCityStyleDB:
   ProductionPercent    AgeCityStyle gives a percentage production bonus.
   FoodPercent          AgeCityStyle gives a percentage food bonus.
   CommercePercent      AgeCityStyle gives a percentage gold bonus.
   SciencePercent       AgeCityStyle gives a percentage science bonus.
   BonusGold            AgeCityStyle gives a gold bonus.
   BonusFood            AgeCityStyle gives a food bonus.
   BonusProduction      AgeCityStyle gives a production bonus.
   BonusScience         AgeCityStyle gives a science bonus.

2006-02-15 (Revision 543)
Fixed:    Mistake in tileset.cpp which prevented loading of tile file
Fixed:    Report start of wonder when deleting the first item and the second 
          item is a wonder
Fixed:    Building maintenance deficit spending.
Added:    Default string loading for mods that lacks those strings.
Added:    DoNotImportDefaults flag to prevent default string loading in 
          updated mods.
Fixed:    Corrected endless path computation in AOM games.
Fixed:    ColorSet to prevent crash during start-up.
Changed:  Changed message box behaviour:
          - Left click on message in message list opens the message.
          - Left click again closes the open message.
          - Left click on another message with open message closes the
            the open messages and opens the clicked message.
          - Right click on message in message list deletes the message
            irrespective it is open or closed.
Added:    Settlers can now settle in cities and thus add an pop.
Fixed:    If on a tile are a city and an entrenching unit the city is selected
          first before the entrenching unit.
Fixed:    Endless path computation observed in AOM game
Improved: Build queue handling.
Fixed:    Corrected building maintenance deficit spending handling.
Fixed:    Keymap screen to get the correct keys with the right text.
Fixed:    Report the start of a wonder when in the build queue the first
          item is deleted and the second one is a wonder.
Fixed:    Problems with invisible sprites.
Fixed:    Crash related to loading of a 1.1 savegames of a mod that uses 
          extended database slic access. However slic must still be reloaded
          to make the game run properly.
Changed:  Very slow growth is now displayed as ---.
Added:    New database flags for modders:
- BuildingDB:
   NeedsCityGoodAll     Building needs all the goods in that list to be built.
   NeedsCityGood        Building can only be built with this good in the city.
   EnablesGood          Building gives a certain good.
   OnePerCiv            Building is limited to one city in the empire.
   GoldPerCity          Building adds the given amount of gold to the city.
- CityStyleDB:
   HappyInc             CityStyle increases city happiness.
- CivilisationDB:
   HappyInc             Civilisation increases city happiness.
- TerrainImprovementDB:
   EnablesGood          Terrain Improvement gives a certain good.
   CantPillage          Terrain Improvement cannot be pillaged.
- UnitDB:
   NeedsCityGoodAll     Unit needs all the goods in that list to be built.
   NeedsCityGood        Unit can only be built with this good in the city.
   NeedsCityGoodAnyCity Unit can only be built if a city in the empire has a 
                        certain good.
   Sneakattack          Unit can attack without war being declared.
   Sneakbombard         Unit can bombard without war being declared.
   CanBombardTiles      Unit can bombard tileimps and destroy them.
   CollateralTileDamage Unit destroys tileimps at attack location.
   NonLethalBombard     Unit bombard only weakens other units.
   New combat modifiers like WoodenShipBonus and WoodenShip
- WonderDB:
   NeedsCityGoodAll     Wonder needs all the goods in that list to be built.
   NeedsCityGood        Wonder can only be built with this good in the city.
   EnablesGood          Wonder gives a certain good.

2005-10-23 (Revision 477)
Fixed:    Miscelaneous crashes
Fixed:    Broken/incorrect links in the great library
Fixed:    Topic display bug in great library
Fixed:    Hosting of MP games on XP machines
Fixed:    SLIC error related to missing city
Fixed:    Prediction of movement times for ships moving through cities
Changed:  Message boxes to be non-modal
Added:    DebugSlic option to the advance options.
Added:    Animated good option to the advanced options.
Added:    Two more colour sets

2005-09-12 (Revision 453)
Fixed:    Bug with gold income
Added:    City limit to domestic control panel
Changed:  Information window remains open across turns
Changed:  Opening score tab of information window no longer closes other 
          windows
Added:    Gold loss due to conversion and production loss due to franchising 
          to national manager resource tab
Added:    Special attack window to display the cost of a special attack
Added:    Construction time to tileimp tracker window
Fixed:    Crashes when deleting tileimps
Added:    Option to end game on data base error
Fixed:    MP database sync check
Added:    Option for female leader pictures
Fixed:    Age display in MP summary
Fixed:    Colors used on single player selection screen

2005-08-02 (Revision 434)
Fixed:    Bugs with /importmap
Fixed:    Misc crashes
Fixed:    Updating of values caused by alteration of sliders on national 
          management dialog
Fixed:    Bug preventing rush buying an item inserted before Capitalization 
          or Infrastructure
Added:    Buttons linking the National, City and Build Managers

2005-06-28 (Revision 403)
Fixed:    Crash caused by loading savegames with the wrong number of goods
Fixed:    Misc crashes
Added:    More AOM support
Changed:  Method of generating advances and units from goody huts to speed 
          it up
Fixed:    Problem preventing play with fewer than 3 civs
Removed:  Automatic tutorial advice on low difficulty levels, since it breaks 
          mods and generally causes problems (we have a specific tutorial 
          button now on the main menu for those that want one)

2005-05-23 (Revision 368)
Added:    Brief changelog to playtest readme
Fixed:    Incorrect bonuses from goods
Fixed:    Misc crashes
Added:    Some more credits

2005-05-17 (Revision 354)
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
Changed: The method by which teleevangelists are recognized as special
Changed: Method of caravan cost fix - now mods should have reasonable caravan costs
Changed: Default colour scheme to get player 1 blue by default
Added: Detailed changelog to playtest version

2005-03-05
Disabled: AI unit text, so that you don't see the AI goals.
Fixed: Crashes and memory leaks.
Fixed: Minor translation problems with the French and German string files.
Updated: strategies.txt, advancelists.txt, buildlistsequences.txt and unitbuildlist.txt, 
  to make the AI more aggressive and separate the AI personalities more.
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
Restored: Old operator of squad strength. That caused the huge delays in the last version
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
Fixed: Science popup in hotseat and PBEM is delayed until the appropriate player's turn.
Fixed: The risk levels in the drop down menu in the scenario editor are now in the 
  right order.
Fixed: Turn count in PBEM don't say anymore always "Turn 0"
Changed: The same algorithm for SP and MP is used to generate Barbarians
Changed: New terrain values

2004-12-28
Added: An attempt to fix range bombardment, but it does not seems to work.
Fixed: Vision
        - The status bar only shows what it should show. (Terrain status of the last visit)
        - Help tile window shows terrain statistics and improvements from the last visit.
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
Improved: The music screen unconfirmed user changes are not resetted when returning to 
  the music screen after visiting the music track selection screen and removed memory 
  leaks.
Fix: Unblock the client user interface when the server has handled the city production.
Restored: Database compatibility of ConstDB for MP testing.
Untested fix: Make cities grow and make science handled well in PBEM.
Updated: Some player slic built-ins so that they can be not only used in string 
  replacement but also in obtaining integer values.
Repaired: Some invalid links in the English Great Library of the Alexander scenario.
Updated: GAMEID to match the value of the new anet.inf
Fixed: Some crashes in concerning diplomat data and the SP new tribe screen.
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
  terraform option available for a terrain with better food stats
Added: Changes to favor Respect of non-trespassing Treaty and retreat units
Added: Checks to avoid Army blocked in grouping phase
Changed: The Group size check
Changed: Rollback goals in double search
Added: Methods to check if there is enough room for army along path, check danger along path
Changed: Several values in goals.txt
Fixed: typo corrected
Changed: useless compiler warning disabled
Added: CanTransport and IsCivilian, and a few other consistency changes to the Army class
Fixed: Prevented a crash when a space plane gets assigned multiple orders
Changed: Updated strategy database so that when two strategies are merged values aren't 
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
Fixed: Visual tile improvement progression
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
Fixed: Sorting by governor type
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
Added: Redetermination of the city style when setting the civilization in HotSeat
Fixed: Strange tile visibility patterns in HotSeat mode
Fixed: Enabled the end turn button when unblanking the screen in HotSeat mode
Fixed: Blank the interface when loading a saved hot seat game 
Fixed: Recipients of messages
Fixed: Increased animation queue to 12, making unit-in-wrong-place bug less likely
Fixed: Cities do not revolt twice in a row
Fixed: Slic files for the Alexander the Great scenario
Added: Crash prevention for missing strings and images
Fixed: Sort order for governors in in status tab for blank entries
Improved: Sort logic for listboxes
Fixed: Sort order in NationalManager for turns until production is finished
Added: New message for the distinction if a unit was caught or not in city investigation
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
Changed: Naming convention for AutoSaves
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
Fixed: Check which prevented suing of franchises
Fixed: When starting a scenario, use difficulty settings selected by user
Changed: French translation of "Network Game"
Changed: Implementation of new SLIC global variables

2004-04-30
Added: New SLIC global variables
Fixed: Bug with scenario editor and players who have been removed
Added: New SLIC functions
Changed: Some French and German strings
Improved: Governor AI choice of squares on which to place tile imps
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
Updated: German and English version of the Great Library.
Updated: colors00.txt to replace some nearly identical colors and changed the 
  color order a little bit.
Fixed: Disabled restart key in network, hot seat and email games.
Fixed: Is the scenario editor launched for the first time in a game session, 
  player 1 is not selected anymore.
Added: Player selection, and with player selection color selection.
Fixed: Scenarios that allow players other than player 1 to be played.
Fixed: Games with more then 28 civilizations can now be saved and loaded.
Changed: Replaced all Neptune references by Uranus references. For some languages
  this needs to be checked if the right translation is used.
Improved: The path finding algorithm, as a downside this implementation could be
  very slow.
Added: Setting research goal to something already researched clears the goal.
Added: Italian translation, unfortunately it is not complete.
Fixed: Some more memory leaks.
Fixed: Pollution is not turned off automatically when the scenario editor is used.
Added: Loading of build queues with capitalisation or infrastructure.
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
Fixed: The AI does not consider goals anymore that have become, e.g. destroyed cities.

2004-02-21
Added: Governments can now modify the properties of units, tile improvements, buildings,
  goods, wonders and feats.
Added: French translation is now complete.
Altered: Description text of the max players spinner on the new single player players screen
  to increase the understandability.
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
Added: MAX_MATCH_LIST_CYCLES flag to sonst.txt of the Magnificent Samurai Scenario, so that 
  it work with this version of CTP2.
Possible crash fix: cpuinf32.dll is no longer used to examine the CPU, didn't work on non 
  Intel CPUs anyway.
Improved: Have the great library start with something more useful than Advanced Infantry
  Tactics.
Fixed: A number of SLIC functions that used calls with inappropriate arguments.
Fixed: Great library data between players in hotseat games is now cleared.
Added: In Advanced Options Menu some more useful descriptions for CloseOnEyepoint and
  ShowExpensive functions, a native speaker may have a look on the English version for
  language corrections.
Added: All the changed text files from patch 1.1/1.11.
Added: Missing diplomacy photos in the diplomanager.
Replaced: Buttons for the number of Civs in the single player new game screen by a spinner.
  This allows you to start a SP game with more then 7 opponents. The maximum is 31
  civilizations in one game. Unfortunately only games up to 28 or 29 players can be reloaded.
Added: A maximum civs spinner on the same screen that allows you to set to how many civs can
  be at the same time in one game, while the limit is not reached revolting cities are
  converted into new civs, otherwise in Barbarian cities.
Added: On the same screen another spinner that will allow you to select your civ color,
  unfortunately does not work, yet.
Replaced: Disclaimer strings are added for the old disclaimer image from the last versions,
  to allow easy localization.
Fixed: A bug in the scenario.slc from the Magnificent Samurai Scenario, so that now a fort
  in Shichinen is built a site when it should be build.
Added: Up to 33 players are now supported with their own color. I hope you enjoy the new
  color set. :D;)
Fixed: Some cut & paste errors repaired in Exclusions.cpp, no idea what it does now you
  have to ask Fromafar.
Fixed: Alexander scenario.slc, replaced the code in the "doesn't work, grrrr..." function
  with something that seems to do the job.
Added: Slic Database access, and a way to figure out via slic how big a database is. 
Fixed: Crash to desktop prevented in the HasAgreement function
Fixed: Crash to desktop prevented when no sound has been defined for a tile improvement.
Fixed: Unloaded buttons which caused an exit pop-up in the city window, don't ask me what
  Fromafar here means. ;)
Added: If during slic code execution a division through 0 appears in DebugSlic=Yes mode a
  message is given.
Fixed: Map wrap handling of the trade route computation is now consistent with the actual
  map type selection.
Fixed: Trade routes now follow rivers, roads, railroads and maglevs according the Freight
  flag in terrain.txt and tileimp.txt. So if you have a route between two cities and there
  is no road etc. you need much more caravans than if there is a road etc.
Fixed: Handled a crash (stack overflow by infinite recursion) when the advances contain a
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
Fixed: Replaced Shield references with Food ones in the food functions.
Fixed: Removed an AI mental block which prevented it from placing gold improvements on 
  forest tiles. Unfortunately, this is not a very general solution: it will not work for 
  water tiles.

2003-09-12
Fixed: Sound system
Added: Start screen shows legal mumbo jumbo.
Fixed: Hidden goods no longer show up in the Great Library.
Added: Pollution powergraph.
First fix for the PBEM (PBEM is still incomplete though).
Fixed: Coastal fortress and flak guns now work.
Fixed: Sea city sprite bug.
Added: function so that the population is displayed in the CityPopSpinner can be 
  used elsewhere in the program.
Fixed: Bug in Scenario Editor that prevents displaying of more then three city 
  style buttons in the city tab. Well these so far these buttons don't have icons 
  but at least they work now as expected.
Fixed: Bug in the Scenario Editor so that cities created by the Scenario Editor 
  have the right size as displayed in the CityPopSpinner and keep their style given 
  by the Scenario Editor. 
Fixed: Cities created by the scenario are now selected per default.
Attempt to fix bug concerning displaying of unfinished tile improvements. 
  Unfortunately this doesn't work as expected as the graphics of the unfinished 
  improvements aren't updated as expected.
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
Added: The music option to the drop down menu in the menu bar.
Added: When a tile improvement is constructed a sound is played like in CTP1.
Added: Strings in English and German for the new music option and for the ranking tab.
Added: Short cut to the music screen to the keymaps.
Added: A Sound flag, a Freight flag and remove the limit of 3 for construction tiles.
  All in the TerrainImprovementDB.
Fixed: None of the rush buy buttons is disabled anymore when there is only one turn 
  left from completion.
Added: A user interface option: Enables you to view cost and effect of all known tile 
  improvements, even if you are short on PW. To activate this option, you have to edit 
  userprofile.txt and set ShowExpensive to Yes. 
  (Well has to be added, if the game does not do it automatically.)
  There is no in-game screen to modify this setting (not yet).
Fixed: AI-AI-Diplomacy actually happen now.
Added: Two new flags for the TerrainDB to allow modders to customize the graphics for 
  goody huts, this modification allows the modder to define for each terrain a 
  different hut graphic, for example you could have glacier like style huts for icy 
  terrains or a dessert or oasis like ruin/hut for desserts and so on. The new flags 
  in the TerrainDB are optional if both of these flags are missing or just one of them 
  a default image is used, like it is in the original CTP2. The terrain.txt here is a 
  modified version, I replaced some of the ruins graphics with another graphic that is 
  part of the tile file already. This terrain.txt is meant as a base for discussion, 
  so far it just demonstrated that it does work.
Added: The maximum city size that you can create with the scenario editor is now 999.
Added: Flat Map and Neptune World option to the single player shape selection window. 
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

