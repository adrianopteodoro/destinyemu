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
        if(90 > thisclient->PlayerSession->accesslevel)
            return true;
        char* tmp = strtok( (char*)&P[28] , " ");
        int x = atoi(tmp);
        if ((tmp = strtok(NULL, " "))==NULL)
        {
            SendServerMsg( thisclient, "Usage:_/telexy_<x>_<y>" );
            return true;
        }
        int y = atoi(tmp);
        Log( MSG_GMACTION, "%s: /telexy %i %i" , thisclient->PlayerInfo->char_name, x, y );
        return pakGMTele( thisclient, P, x, y );
    }
    return true;
}

bool CConnServer::pakGMNotice( CConnClient* thisclient, unsigned char* P )
{
    char* tmp = strtok( (char*)&P[28] , " ");
    if(tmp==NULL)
    {
        SendServerMsg( thisclient, "Usage:_/notice_<msg>" );
        return true;
    }
    packet->Free();
    packet->AddWord( 108, 0 );
    packet->AddByte( 0x01, 4 );
    packet->AddByte( 0x01, 5 );
    packet->AddStr( tmp, 12 );
    SendToAll( encdec, packet, 108 );
    Log( MSG_GMACTION, "%s: /notice %s" , thisclient->PlayerInfo->char_name, tmp );
    return true;
}

bool CConnServer::pakGMTele( CConnClient* thisclient, unsigned char* P, int x, int y )
{
    packet->Free();
    packet->AddByte( 92, 0 );
    packet->AddByte( 0x36, 4 );
    packet->AddByte( 0x03, 5 );
    packet->AddWord( thisclient->PlayerSession->clientid, 6 );
    packet->AddWord( 255, 12 );
    packet->AddWord( 1151, 14 );
    //this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 92, this->CKeys, this->Hash1, 0 );
    //thisclient->SendPacket( this->encbuf, this->encsize );
    return true;
}
