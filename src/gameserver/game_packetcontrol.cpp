#include "game_sockets.h"

bool CConnServer::PacketControl( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, thisclient->PSize, (unsigned char*)this->CKeys );
    unsigned short opcode;
    memcpy( &opcode, &buff[4], 2 );
    if ( thisclient->PSize != 52 )
        Log( MSG_NOTICE, "Received OPCODE: 0x%04x", opcode );
    switch ( opcode )
    {
        case 0x020f: return CharCreate( encdec, thisclient, P );
        case 0x0211: return CharDelete( encdec, thisclient, P );
        case 0x0213: return SendToWorld( encdec, thisclient, P );
        case 0x0366: return ObjectMove( encdec, thisclient, P );
        case 0x0334: return SendServerMsg( encdec, thisclient, "Not implemented yet." ); // change srv
        case 0x0215: return SendServerMsg( encdec, thisclient, "Not implemented yet." ); // gametocharlist
        case 0x0290:
            Log( MSG_NOTICE, "Teleport: 0x%02x 0x%02x 0x%02x 0x%02x", buff[12], buff[13], buff[14], buff[15] );
        break; // teleport
    }
}
