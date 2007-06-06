#include "game_sockets.h"

bool CConnClient::VisiblityList( )
{
	std::vector<CConnClient*>	newVisibleClients;
	// Clients
    for(UINT i=0;i<GServer->ClientList.size();i++)
    {
        CConnClient* otherclient = (CConnClient*)GServer->ClientList.at(i);
		if ( this==otherclient || !otherclient->PlayerSession->inGame )
            continue;
        float distance = GServer->distance( this->PlayerPosition->Cpos, otherclient->PlayerPosition->Cpos );
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
    VisibleClients.clear();
	VisibleClients = newVisibleClients;
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
