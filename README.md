# DESTINY EMULATOR
## (DEVELOPMENT VERSION)

- 01/04/2007
    - Added: wyd_server.xml, new type of config file and put in configs folder. [Snaity]

- 06/04/2007
    - Fix: Character can walk in the world. [Snaity]

- 10/04/2007
    - Changed: Character and Items Loading from Database, item is now defined by slotnum (0 to 14 is equiped items and 15 to 77 is items on inventory) [Snaity]

- 17/04/2007
    - Fix: Fixed Char List now not show a blue cape in char with no cape in your invetory. [Snaity]
    - Fix: Fixed a bug on char deletion with password verify, its work now. [Snaity]
    - Test: Show other player packet test. [Snaity]

- 22/04/2007
    - Changed: gameserver.xml to serverconfig.xml and put out from conf dir. [Snaity]
    - Added: Data dir and startchar.xml with new chars start items, stats, gold, and inventory. [Snaity]
    - Adding: Threads for NPC, MOBS, DROPS and players things. [Snaity]
    - Moding: Source code, for better loads and data acessing. [Snaity] 

- 02/05/2007
    - Added: Visibility Process, the start way to view MOBS, DROPS, NPCS and Players, its working now you can see other players in game but not show he's walking in world. [Snaity]

- 07/05/2007
    - Added: Dev-Cpp Project File, to people using Dev-Cpp IDE to compile GNU C++ Projects. [Snaity]

- 16/05/2007
    - Solved: Packet Reception, server is receiving some part of packet incorretly and 2 or more packets in one receive data. [Snaity]
    - Fixed: You can see now other players walking in the world, but its in testing. [Snaity]

- 18/05/2007
    - Added: GM Comands Functions, and added a /notice <msg> GM Command, this command send a notice to all connected players and only acesslevel > 90 account can use it. [Snaity]
    - Added: Client Version Block, you now can set it on config file. [Snaity]

- 23/05/2007
    - Added: GM Command /spawn <mobcode> <mobid> <mobname> for NPC/MONSTERS test. [Snaity]

- 12/06/2007 - Revision 59:
    - Changed and Fix: Database fields to correctly, owner field in chars_items is now by character id not more by character name, removed storage_gold from character table and moved to account table. [Snaity]
    - Added: Support of "StartChars.xml" u can configure here the start stats and items of new characters. [Snaity]

- 20/06/2007 - Revision 60:
    - Added: NPC Support, now can add NPC to game but its not complete, see ".\data\NpcList.xml" and ".\npcs\' Folder. [Snaity]

- 26/06/2007 - Revision 62:
    - Added: GM Command /telexy <posx> <posy>, you need to put accesslevel above of 90. [Snaity]
    - Added: Armia NPCs, but NPCs not working. [GateKiller]

- 03/07/2007 - Revision 63:
    - Added: More NPCs, and not 100% working. [GateKiller]

- 08/07/2007 - Revision 64:
    - Fixed: Bugs on NPC Shop/SkillLearn Disconnecting client after click on it. [darcker]
    - Fixed: Now u can view other players attack actions. [Snaity]
    - Added: Option to define server IP Address on "serverconfig.xml" see the comments for more info. [Snaity]
    - Added: Shop Editing on NpcFiles see "Aki.xml" for Example. [darcker & Snaity]
    - Added: Reading of ItemList.xml but not now usable, this for future things. [darcker]

- 02/08/2007 - Revision 65:
    - Fixed: Player CP on entering on world. Now CP Status is Write. [GateKiller]
    - Added: All Shops Items to Respectives NPCs. [GateKiller]
    - Fixed: Visibilty Range, Range is Lowered to Prevent Lag. [Snaity]

- 27/08/2007 - Revision 68:
    - Upgrade: some libs, and directory formation.
