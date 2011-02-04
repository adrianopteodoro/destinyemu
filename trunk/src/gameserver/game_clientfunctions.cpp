#include "game_sockets.h"

bool CConnClient::VisiblityList( )
{
	std::vector<CConnClient*>	newVisibleClients;
	std::vector<CNPC*>			newVisibleNPCs;
	// Clients
    for(UINT i=0;i<GServer->ClientList.size();i++)
    {
        CConnClient* otherclient = (CConnClient*)GServer->ClientList.at(i);
		if ( this==otherclient || !otherclient->PlayerSession->inGame )
            continue;
        float distance = GServer->distance( this->PlayerPosition->current, otherclient->PlayerPosition->current );
        if ( GServer->IsVisible( this, otherclient ) )
        {
            if ( distance < MAXVISUALRANGE && !otherclient->isInvisibleMode )
			{
				newVisibleClients.push_back( otherclient );
            }
			else
			{
				this->ClearObject( otherclient->PlayerSession->clientid );
            }
        }
        else
        {
			if ( distance < MINVISUALRANGE && !otherclient->isInvisibleMode )
            {
			    newVisibleClients.push_back( otherclient );
				GServer->SpawnChar(this,otherclient);
			}
        }
    }
    // Npcs
	for(unsigned i=0; i< GServer->NPCList.size(); i++)
    {
		CNPC* thisnpc = GServer->NPCList.at(i);
		float distance = GServer->distance( this->PlayerPosition->current, thisnpc->pos );
		if ( GServer->IsVisible( this, thisnpc ) )
        {
			if ( distance < MAXVISUALRANGE )
            {
				newVisibleNPCs.push_back( thisnpc );
            }
			else
			{
        		this->ClearObject( thisnpc->clientid );
            }
		}
        else
        {
			if ( distance < MINVISUALRANGE )
            {
				newVisibleNPCs.push_back( thisnpc );
				GServer->SpawnNPC( this, thisnpc );
			}
		}
	}
    VisibleClients.clear();
    VisibleNPCs.clear();
	VisibleClients = newVisibleClients;
	VisibleNPCs = newVisibleNPCs;
    return true;
}

// Erase a object from the user
bool CConnClient::ClearObject( unsigned int otherclientid )
{
	//BEGINPACKET( pak, 0x794 );
	//ADDWORD( pak, otherclientid );
	//SendPacket( &pak );
	return true;
}
