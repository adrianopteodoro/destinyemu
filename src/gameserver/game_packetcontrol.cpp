#include "game_sockets.h"

bool CConnServer::PacketControl( CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, thisclient->PSize, (unsigned char*)this->CKeys );
    unsigned short opcode;
    memcpy( &opcode, &buff[4], 2 );
    if ( thisclient->PSize != 52 )
        Log( MSG_NOTICE, "Received OPCODE: 0x%04x", opcode );
    switch ( opcode )
    {
        case 0x020f: return CharCreate( thisclient, P );
        case 0x0211: return CharDelete( thisclient, P );
        case 0x0213: return SendToWorld( thisclient, P );
        case 0x0366: return ObjectMove( thisclient, P );
        case 0x0334: return SendServerMsg( thisclient, "Not implemented yet." ); // change srv
        case 0x0215: return SendServerMsg( thisclient, "Not implemented yet." ); // gametocharlist
        case 0x0290: true; // teleport
    }
}
