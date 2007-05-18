#include "game_sockets.h"

bool CConnServer::PacketControl( CConnClient* thisclient, int size, unsigned char* P )
{
    unsigned short opcode;
    unsigned short cid;
    memcpy( &opcode, &P[4], 2 );
    memcpy( &cid, &P[6], 2 );
    if ( size != 52 )
    {
        FILE *fh = NULL;
        fh = fopen( ".\\logs\\recv_packets.log", "a+" );
        if ( fh != NULL )
        {
		   fprintf( fh, "[RECVPACKET]\nCLIENTID: %i\nOPCODE: 0x%04x\nSIZE: %i\nSID: %08u\nDATA:\n", cid, opcode, thisclient->PSize, sock );
		   for ( int i=0; i<thisclient->PSize; ++i )
               fprintf( fh, "%02x ", P[i] );
           fprintf( fh, "\n\n" );
	       fclose( fh );
	    }
	}

    switch ( opcode )
    {
        case 0x020f: return CharCreate( thisclient, P );
        case 0x0211: return CharDelete( thisclient, P );
        case 0x0213: return SendToWorld( thisclient, P );
        case 0x0366: return ObjectMove( thisclient, P );
        case 0x0369: return true; // Unknow
        case 0x0333: return SendChat( thisclient, P );
        case 0x0334: return packetCommand( thisclient, P );
        case 0x0215: return SendServerMsg( thisclient, "Not implemented yet." ); // gametocharlist
        case 0x0290: return true; // teleport
        case 0x0376: return ChangeInventory( thisclient, P );
        case 0x03a0: return true;
        default:
            Log( MSG_WARNING, "Received Unknow OPCODE: 0x%04x", opcode );
        break;
    }
}
