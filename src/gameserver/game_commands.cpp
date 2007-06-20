#include "game_sockets.h"

int npcid = 1000;

bool CConnServer::packetCommand( CConnClient* thisclient, unsigned char* P )
{
    char* command = strtok( (char*)&P[12] , " ");
    if (command==NULL) return true;
    //Log( MSG_INFO,"Command: %s", command);
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
    if (strcmp(command, "spawn")==0)
    {
        if(90 > thisclient->PlayerSession->accesslevel)
            return true;
        char* tmp = strtok( (char*)&P[28] , " ");
        int mobcode = atoi(tmp);
        if ((tmp = strtok(NULL, " "))==NULL)
        {
            SendServerMsg( thisclient, "Usage:_/spawn_<mobcode>_<mobid>_<name>" );
            return true;
        }
        int mobid = atoi(tmp);
        if ((tmp = strtok(NULL, " "))==NULL)
        {
            SendServerMsg( thisclient, "Usage:_/spawn_<mobcode>_<mobid>_<name>" );
            return true;
        }
        char* name = tmp;
        Log( MSG_GMACTION, "%s: /spawn %i %i %s" , thisclient->PlayerInfo->char_name, mobcode, mobid, name );
        return pakGMSpawn( thisclient, P, mobcode, mobid, name );
    }
    if (strcmp(command, "time")==0)
    {
        time_t rawtime;
        struct tm * timeinfo;
        time(&rawtime);
        timeinfo = localtime ( &rawtime );
        SendServerMsg( thisclient, "%s", asctime(timeinfo));
        return true;
    }
    if (strcmp(command, "srv")==0)
    {
        char* tmp;
        if ((tmp = strtok( (char*)&P[28], " "))==NULL) return true; char* va1 = tmp;
        if ((tmp = strtok(NULL, " "))==NULL) return true; char* va2 = tmp;
        if ((tmp = strtok(NULL, " "))==NULL) return true; char* va3 = tmp;
        Log( MSG_GMACTION, "%s: /srv %s %s %s" , thisclient->PlayerInfo->char_name, va1, va2, va3 );
        return true;
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
    packet->AddWord( 27259, 12 );
    packet->AddWord( 1942, 14 );
    packet->AddWord( 124, 16 );
    packet->AddWord( 2456, 20 );
    packet->AddWord( 2001, 22 );
    packet->AddWord( 2456, 24 );
    packet->AddWord( 2001, 26 );
    packet->AddWord( 44, 28 );
    packet->AddWord( 1305, 30 );
    packet->AddWord( 255, 32 );
    packet->AddWord( 25000, 36 );
    packet->AddWord( 5796, 40 );
    packet->AddWord( 3145, 42 );
    packet->AddWord( 1305, 44 );
    packet->AddWord( 124, 48 );
    packet->AddWord( 65535, 50 );

    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 92, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
    return true;
}

bool CConnServer::pakGMSpawn( CConnClient* thisclient, unsigned char* P, int mobcode, int mobid, char* name )
{
    packet->Free();
    packet->AddWord( 172, 0 );
    packet->AddWord( 0x0364, 4 );
    packet->AddWord( 30000, 6 );
    // end header

    packet->AddWord( (int)thisclient->PlayerPosition->Cpos.x, 12 ); // pos x
    packet->AddWord( (int)thisclient->PlayerPosition->Cpos.y, 14 ); // pos y
    packet->AddWord( npcid, 16 );
    for (int i=0;i<12;i++)
        packet->AddByte( 0xcc, 18+i );
    packet->AddStr( name, 18 ); // char name
    //packet->AddWord( 75, 30 ); // chaos point
    packet->AddByte( mobid, 34 ); // mob id

    //packet->AddWord( 705, 46 );
    //packet->AddWord( 2380, 62 );
    packet->AddByte( 1, 66 );//0
    packet->AddByte( 34, 67 );//34
    packet->AddWord( 0, 69 );//player effect
    packet->AddWord( 2, 100 );// Level
    packet->AddWord( 3, 102 );// Defesa
    packet->AddWord( 93, 104 );// Atack
    packet->AddByte( mobcode, 106 );// mobcode
    packet->AddByte( 1, 107 );//84
    //------------------------------------
    packet->AddWord( 800, 108 );//max hp
    packet->AddWord( 100, 110 );//max mana
    packet->AddWord( 200, 112 );//current hp
    packet->AddWord( 100, 114 );//current mana
    //-------------------------------------
    packet->AddWord( 10, 116 );//str
    packet->AddWord( 70, 118 );//int
    packet->AddWord( 30, 120 );//dex
    packet->AddWord( 200, 122 );//con
    packet->AddWord( 2047, 124 );//5
    packet->AddWord( 2047, 126 );//5
    packet->AddWord( 0, 128 );//2
    packet->AddByte( 0, 130 );//43
    for (int i=0;i<29;i++)
    {
        packet->AddByte( 0xcc, 147+i );
    }
    packet->AddByte( 0x01, 172 );

    SendToVisible( encdec, thisclient, packet, 172, true );
    npcid++;
    return true;
}
