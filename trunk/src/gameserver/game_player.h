#ifndef _GAME_PLAYER_H_
#define _GAME_PLAYER_H_

#include "../commom/sockets.h"
#include "game_types.h"

#define ClearItem(i) { i.itemid=0; i.add1=0; i.add2=0; i.add3=0; i.val1=0; i.val2=0; i.val3=0; }

class CConnClient : public CClientSocket
{
public:
	CConnClient ( );
	~CConnClient ( );

	//****************
	// Player Structures
    CAccount* PlayerSession; // Account information
    CStats* PlayerStats; // Player Stats
    CInfo* PlayerInfo; // Player information (classid, mobid, gold, exp, ...)
    CPosition* PlayerPosition; // Player Position

    // Inventory
    CItem items[77];

    // General Functions
    bool loaddata();
    bool savedata();
};

#endif
