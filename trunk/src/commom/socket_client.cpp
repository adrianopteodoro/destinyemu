#include "sockets.h"

// -----------------------------------------------------------------------------------------
// Client Socket Constructor
// -----------------------------------------------------------------------------------------
CClientSocket::CClientSocket( )
{
	PacketSize		= 6;		// Starting size
	PacketOffset	= 0;		// No bytes read, yet.
}

// -----------------------------------------------------------------------------------------
// Client Socket Destructor
// -----------------------------------------------------------------------------------------
CClientSocket::~CClientSocket( )
{
}

// -----------------------------------------------------------------------------------------
// Receive this client's socket
// -----------------------------------------------------------------------------------------
bool CClientSocket::ReceiveData( )
{
	int   ReceivedBytes;
	short BytesToRead;

	ZeroMemory( &Buffer, 65355 );

	ReceivedBytes = recv( sock, (char*)&Buffer[0], 65355, 0 );
	if ( ReceivedBytes <= 0 ) return false;
	this->PSize = ReceivedBytes;

	this->recvpkts++;

	if ( ReceivedBytes != 52 )
	{
        /*FILE *fh = NULL;
        fh = fopen( ".\\logs\\recv_packets.log", "a+" );
        if ( fh != NULL )
        {
		   fprintf( fh, "[PACKET] (SID:%08u) [SIZE %i]: ", sock, ReceivedBytes );
		   for ( int i=0; i<ReceivedBytes; ++i )
               fprintf( fh, "%02x ", (unsigned char)Buffer[i] );
           fprintf( fh, "\n" );
	       fclose( fh );
	    }*/
	    textcolor(12);
	    printf("[PACKET]: ", ReceivedBytes );
	    textcolor(11);
		for ( int i=0; i<ReceivedBytes; ++i )
            printf( "%02x ", (unsigned char)Buffer[i] );
        printf( "[SIZE %i]\n", ReceivedBytes );
	}

	if ( !ParentServer->OnReceivePacket( this, (unsigned char*)Buffer ) )
	{
		return false;
    }
    else
    {
        return true;
    }
}

// -----------------------------------------------------------------------------------------
// Send a packet to this client
// -----------------------------------------------------------------------------------------
void CClientSocket::SendPacket( unsigned char* P, int size )
{
    u_long iMode = 1;
    if(ioctlsocket(sock, FIONBIO, &iMode)!=0)
    {
        Log( MSG_WARNING, "error - ioctlsocket");
    }
	int retval = send( sock, (char*)P, size, 0 );
	iMode=0;
    if(ioctlsocket(sock, FIONBIO, &iMode)!=0)
    {
        Log( MSG_WARNING, "error - ioctlsocket");
    }
}

// -----------------------------------------------------------------------------------------
