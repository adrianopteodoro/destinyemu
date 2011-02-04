#include "game_sockets.h"

// Update Player Movement
bool CConnClient::PlayerMovement()
{
    if(!PlayerSession->inGame || !PlayerSession->isLoggedIn)
    {
        return true;
    }
	if( PlayerPosition->current.x==PlayerPosition->destiny.x && PlayerPosition->current.y==PlayerPosition->destiny.y )
    {
        return true;
    }
	float dx = 0,dy = 0,distance = 0;
	dx = PlayerPosition->destiny.x - PlayerPosition->current.x;
	dy = PlayerPosition->destiny.y - PlayerPosition->current.y;
	distance = sqrt( (dx*dx) + (dy*dy) );
	clock_t etime = clock() - PlayerTime->lastMoveTime;
	float speed = (float)(PlayerStats->Move_Speed * 0.01);
	float ntime = ( distance / speed * 1000 );
	PlayerTime->lastMoveTime = clock();
	if( ntime<=etime || distance==0 )
	{PlayerPosition->current = PlayerPosition->destiny;}
	else
    {
		PlayerPosition->current.x = dx*( etime / ntime ) + PlayerPosition->current.x;
		PlayerPosition->current.y = dy*( etime / ntime ) + PlayerPosition->current.y;
	}
    return true;
}
