#include "game_monster.h"

CMonster::CMonster( fPoint mposition, unsigned int mspawnid )
{
	SpawnTime = clock( );
	lastSighCheck = clock( );
	Position = new currentition;
	Position->source = mposition;
    Position->current = mposition;
    Position->destiny = mposition; 
    Position->respawn = mspawnid;
    Position->lastMoveTime = clock( );
}
