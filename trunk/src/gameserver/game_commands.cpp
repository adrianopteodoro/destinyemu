#include "game_sockets.h"

bool CConnServer::packetCommand( CConnClient* thisclient, unsigned char* P )
{
    char* command = strtok( (char*)&P[12] , " ");
    if (command==NULL) return true;
    if (strcmp(command, "notice")==0)
    {
        if(90 > thisclient->PlayerSession->accesslevel)
            return true;
        return pakGMNotice( thisclient, P );
    }
    if (strcmp(command, "telexy")==0)
    {
        char* tmp = strtok( (char*)&P[28] , 0x00);
        int x = atoi(tmp);
        if ((tmp = strtok(NULL, 0x00))==NULL) return true; int y = atoi(tmp);
        if(90 > thisclient->PlayerSession->accesslevel)
            return true;
        return pakGMTele( thisclient, P, x, y );
    }
}

bool CConnServer::pakGMNotice( CConnClient* thisclient, unsigned char* P )
{
    std::string tmp;
    memcpy( &tmp[0], &P[28], 80 );
    packet->Free();
    packet->AddWord( 108, 0 );
    packet->AddByte( 0x01, 4 );
    packet->AddByte( 0x01, 5 );
    packet->AddStr( tmp.data(), 12 );
    SendToAll( encdec, packet, 108 );
    Log( MSG_GMACTION, "%s: /notice" , thisclient->PlayerInfo->char_name );
    return true;
}

bool CConnServer::pakGMTele( CConnClient* thisclient, unsigned char* P, int x, int y )
{
    packet->Free();
    packet->AddByte( 92, 0 );
    packet->AddByte( 0x36, 4 );
    packet->AddByte( 0x03, 5 );
    packet->AddWord( thisclient->PlayerSession->clientid, 6 );
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 92, this->CKeys, this->Hash1, 0 );
    thisclient->SendPacket( this->encbuf, this->encsize );
}
