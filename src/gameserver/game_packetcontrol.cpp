#include "game_sockets.h"

bool CConnServer::PacketControl( CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, thisclient->PSize, (unsigned char*)this->CKeys );
    unsigned short opcode;
    memcpy( &opcode, &buff[4], 2 );
    if ( thisclient->PSize != 52 )
    {
        FILE *fh = NULL;
        fh = fopen( ".\\logs\\recv_packets.log", "a+" );
        if ( fh != NULL )
        {
		   fprintf( fh, "[RECVPACKET]\nOPCODE: 0x%04x\nSIZE: %i\nSID: %08u\nDATA:\n", opcode, thisclient->PSize, sock );
		   for ( int i=0; i<thisclient->PSize; ++i )
               fprintf( fh, "%02x ", buff[i] );
           fprintf( fh, "\n\n" );
	       fclose( fh );
	    }
	    textcolor(14);
	    printf("[RECVPACKET] OPCODE 0x%04x - SIZE %i\n", opcode, thisclient->PSize );
	    textcolor(7);
		for ( int i=0; i<thisclient->PSize; ++i )
            printf( "%02x ", buff[i] );
        printf( "\n" );
	}

    switch ( opcode )
    {
        case 0x020f: return CharCreate( thisclient, P );
        case 0x0211: return CharDelete( thisclient, P );
        case 0x0213: return SendToWorld( thisclient, P );
        case 0x0366: return ObjectMove( thisclient, P );
        case 0x0369: return true; // Unknow
        case 0x0333: return SendChat( thisclient, P );
        case 0x0334: return SendServerMsg( thisclient, "Not implemented yet." ); // change srv
        case 0x0215: return SendServerMsg( thisclient, "Not implemented yet." ); // gametocharlist
        case 0x0290: return true; // teleport
    }
}
