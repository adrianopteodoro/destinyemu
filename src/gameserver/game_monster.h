#ifndef _MONSTER_
#define _MONSTER_

#include "game_sockets.h"

class CMonster
{
    public:
        CMonster( fPoint position, unsigned int spawnid=0 );
        ~CMonster();

		// times
		currentition* Position;
        clock_t SpawnTime;
     	clock_t lastSighCheck; 
};

#endif
