#include "game_sockets.h"

// Update Player Movement
bool CConnClient::PlayerMovement()
{
    if(!PlayerSession->inGame || !PlayerSession->isLoggedIn)
    {
        return true;
    }
	if( PlayerPosition->Cpos.x==PlayerPosition->Dpos.x && PlayerPosition->Cpos.y==PlayerPosition->Dpos.y )
    {
        return true;
    }
	float dx = 0,dy = 0,distance = 0;
	dx = PlayerPosition->Dpos.x - PlayerPosition->Cpos.x;
	dy = PlayerPosition->Dpos.y - PlayerPosition->Cpos.y;
	distance = sqrt( (dx*dx) + (dy*dy) );
	clock_t etime = clock() - PlayerTime->lastMoveTime;
	float speed = PlayerStats->Move_Speed * 0.01;
	float ntime = ( distance / speed * 1000 );
	PlayerTime->lastMoveTime = clock();
	if( ntime<=etime || distance==0 )
	{PlayerPosition->Cpos = PlayerPosition->Dpos;}
	else
    {
		PlayerPosition->Cpos.x = dx*( etime / ntime ) + PlayerPosition->Cpos.x;
		PlayerPosition->Cpos.y = dy*( etime / ntime ) + PlayerPosition->Cpos.y;
	}
    return true;
}
