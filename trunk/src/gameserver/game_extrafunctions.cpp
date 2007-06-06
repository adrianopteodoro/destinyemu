#include "game_sockets.h"

void CConnServer::SendToAll( CEncDec* encdec, bufwrite* pak, int size )
{
    for(UINT i=0;i<ClientList.size();i++)
    {
        CConnClient* otherclient = (CConnClient*) ClientList.at( i );
		if (otherclient->PlayerSession->inGame)
		{
		    this->curtime = clock();
            this->encsize = encdec->WYD2_Encrypt( this->encbuf, pak->buff(), size, this->CKeys, this->Hash1, this->curtime );
            otherclient->SendPacket( this->encbuf, this->encsize );
		}
	}
}

// This function gets a new clientID for a npc, monster or mob
unsigned CConnServer::GetNewClientID( )
{
	for (unsigned i=1; i<=0xffff; i++)
    {
		if (ClientIDList[i]!=0 && time(NULL)-ClientIDList[i]>10)
        {
			ClientIDList[i] = 0;
			return i;
		}
	}
	return 0;
}

// calc distance
float CConnServer::distance( fPoint pos1, fPoint pos2 )
{
    float distance = 0;
	float dx = pos1.x - pos2.x;
	float dy = pos1.y - pos2.y;
	distance = sqrt( (dx*dx) + (dy*dy) );
    return distance;
}

// Send a packet too all X who are visible
// -- CLIENT --
void CConnServer::SendToVisible( CEncDec* encdec, CConnClient* thisclient, bufwrite* pak, int size, bool dothisclient )
{
    for(unsigned j=0; j<thisclient->VisibleClients.size(); j++)
    {
		CConnClient* otherclient = thisclient->VisibleClients.at( j );
		this->curtime = clock();
		this->encsize = encdec->WYD2_Encrypt( this->encbuf, pak->buff(), size, this->CKeys, this->Hash1, this->curtime );
		otherclient->SendPacket( this->encbuf, this->encsize );
	}
	if(dothisclient)
	{
	    this->curtime = clock();
        this->encsize = encdec->WYD2_Encrypt( this->encbuf, pak->buff(), size, this->CKeys, this->Hash1, this->curtime );
        thisclient->SendPacket( this->encbuf, this->encsize );
	}
}

// -----------------------------------------------------------------------------------------
// Check if an object is visible to a client
// -----------------------------------------------------------------------------------------
// -- CLIENT --
bool CConnServer::IsVisible( CConnClient* thisclient, CConnClient* otherclient )
{
	for(unsigned j=0; j<thisclient->VisibleClients.size(); j++)
    {
		if (otherclient==thisclient->VisibleClients.at(j))
            return true;
	}
	return false;
}
