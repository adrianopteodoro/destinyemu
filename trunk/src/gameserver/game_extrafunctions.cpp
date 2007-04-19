#include "game_sockets.h"

void CConnServer::SendToVisible( CEncDec* encdec, CConnClient* thisclient, bufwrite* pak, bool dothisclient )
{
    for(unsigned j=0; j<thisclient->VisibleClients.size(); j++)
    {
		CConnClient* otherclient = thisclient->VisibleClients.at( j );
		this->encsize = encdec->WYD2_Encrypt( this->encbuf, pak->buff(), pak->psize(), this->CKeys, this->Hash1, 0 );
		otherclient->SendPacket( this->encbuf, this->encsize );
	}
	if(dothisclient)
	{
        this->encsize = encdec->WYD2_Encrypt( this->encbuf, pak->buff(), pak->psize(), this->CKeys, this->Hash1, 0 );
        thisclient->SendPacket( this->encbuf, this->encsize );
	}
}
