// -----------------------------------------------------------------------------------------
// Inclusions
// -----------------------------------------------------------------------------------------
#include "sockets.h"
// -----------------------------------------------------------------------------------------
// Server Socket Constructor
// -----------------------------------------------------------------------------------------
CServerSocket::CServerSocket( )
{
}

// -----------------------------------------------------------------------------------------
// Server Socket Destructor
// -----------------------------------------------------------------------------------------
CServerSocket::~CServerSocket( )
{
}

// -----------------------------------------------------------------------------------------
// Start up our server
// -----------------------------------------------------------------------------------------
bool CServerSocket::StartServer( )
{
struct sockaddr_in ain;

	sock = socket( AF_INET, SOCK_STREAM, 0 );
	if (sock == INVALID_SOCKET)
    {
		Log( MSG_FATALERROR, "Could not create a socket" );
		return false;
	}

	ain.sin_family		= AF_INET;
	ain.sin_addr.s_addr	= INADDR_ANY;
	ain.sin_port		= htons( port );
	if ( bind( sock, (const sockaddr*)&ain, sizeof( ain ) ) )
    {
		Log( MSG_FATALERROR, "Could not bind socket" );
		closesocket( sock );
		sock = INVALID_SOCKET;
		return false;
	}

	if ( listen( sock, SOMAXCONN ) == -1 )
    {
		Log( MSG_FATALERROR, "Could not listen on socket" );
		closesocket( sock );
		sock = INVALID_SOCKET;
		return false;
	}

	isActive = true;

	if ( !this->OnServerReady( ) )
    {
		Log( MSG_FATALERROR, "Server could not start" );
		closesocket( sock );
		sock = INVALID_SOCKET;
		isActive = false;
		return false;
	}
	Log( MSG_INFO, "Server started on port %i and is listening.", port );
	ServerLoop( );
	// Nothing past here is ever really called
	OnServerDie( );
	closesocket( sock );

	return true;
}

// -----------------------------------------------------------------------------------------
// Server is started, lets run our loop :D
// -----------------------------------------------------------------------------------------
void CServerSocket::ServerLoop( )
{
fd_set		fds;
	int			activity;
	sockaddr_in	ClientInfo;
	SOCKET		NewSocket;
	timeval		timeout = {0,2000};
	OnServerStep();
	do
	{
		FD_ZERO( &fds );
    	FillFDS( &fds );
		FD_SET( sock, &fds );

		activity = select( 0, &fds, NULL, NULL, &timeout );
		if ( activity == 0 )
        {
            continue;
        }

		if ( activity < 0 && errno != EINTR ) {
			Log( MSG_ERROR, "Select command failed. Error #%i", WSAGetLastError() );
			isActive = false;
		}
		if ( FD_ISSET( sock, &fds ) )
        {
			int clientinfolen = sizeof( sockaddr_in );
            #ifdef __unix__
		    NewSocket = accept( sock, (sockaddr*)&ClientInfo, (socklen_t*)&clientinfolen );
            #else
			NewSocket = accept( sock, (sockaddr*)&ClientInfo, (int*)&clientinfolen );
            #endif
			// TODO: check if server is full
			if (NewSocket != INVALID_SOCKET)
				AddUser( NewSocket, &ClientInfo );
			else
				Log( MSG_ERROR, "Error accepting socket: %i", WSAGetLastError() );
		}
        HandleClients( &fds );
	} while( isActive );
}

// -----------------------------------------------------------------------------------------
// Fills out an FDS for the server
// -----------------------------------------------------------------------------------------
void CServerSocket::FillFDS( fd_set* fds )
{
	for(UINT i=0;i<ClientList.size( );i++)
	{
        CClientSocket* client = ClientList.at( i );
        if(client->isActive)
        {
		  FD_SET( (unsigned)client->sock, fds );
        }
        else
        {
            DisconnectClient( client );
        }
	}
}

// -----------------------------------------------------------------------------------------
// Handle all our clients
// -----------------------------------------------------------------------------------------
void CServerSocket::HandleClients( fd_set* fds )
{
    for(UINT i=0;i<ClientList.size( );i++)
	{
        CClientSocket* client = ClientList.at( i );
        if(!client->isActive)
            continue;
		if ( FD_ISSET( (unsigned)client->sock, fds ) )
		{
			if ( !client->ReceiveData( ) )
			{
				DisconnectClient( client );
			}
		}
	}
}

// -----------------------------------------------------------------------------------------
// Add a new user to our server
// -----------------------------------------------------------------------------------------
void CServerSocket::AddUser( SOCKET sock, sockaddr_in* ClientInfo )
{
	CClientSocket* thisclient = this->CreateClientSocket( );
	if (thisclient==NULL)
    {
		closesocket( thisclient->sock );
		if (thisclient!=0) delete thisclient;
		thisclient=0;
		return;
	}


	thisclient->ParentServer = this;
	thisclient->sock = sock;
	thisclient->isActive = true;

	if (!OnClientConnect( thisclient )) {
		closesocket( thisclient->sock );
		if (thisclient!=0) delete thisclient;
		thisclient=0;
		return;
	}
	Log( MSG_INFO, "User connected from %s [SID: %08u]", inet_ntoa( ClientInfo->sin_addr ), thisclient->sock );
	ClientList.push_back( thisclient );
}

// -----------------------------------------------------------------------------------------
// Disconnect our user
// -----------------------------------------------------------------------------------------
void CServerSocket::DisconnectClient( CClientSocket* thisclient )
{
	OnClientDisconnect( thisclient );
	Log( MSG_INFO, "User disconnected [SID: %08u]", thisclient->sock );
	closesocket( thisclient->sock );
	thisclient->sock = INVALID_SOCKET;
	thisclient->isActive = false;
    for(UINT i=0;i<ClientList.size( );i++)
    {
        CClientSocket* client = ClientList.at( i );
        if( client == thisclient )
        {
            ClientList.erase( ClientList.begin( ) + i );
            break;
        }
    }
	DeleteClientSocket( thisclient );
}

// -----------------------------------------------------------------------------------------
// This function creates an appropriate client socket
// -----------------------------------------------------------------------------------------
CClientSocket* CServerSocket::CreateClientSocket ( )
{
	CClientSocket* thisclient;
    thisclient = new CClientSocket( );
    assert(thisclient);
	return thisclient;
}

// -----------------------------------------------------------------------------------------
// This function deletes an old client socket
// -----------------------------------------------------------------------------------------
void CServerSocket::DeleteClientSocket( CClientSocket* thisclient )
{
	delete thisclient;
}

// -----------------------------------------------------------------------------------------
// This function is called just before proccessing clients
// -----------------------------------------------------------------------------------------
void CServerSocket::OnServerStep( )
{
}

// -----------------------------------------------------------------------------------------
// This function is called just before the server starts
// -----------------------------------------------------------------------------------------
bool CServerSocket::OnServerReady( )
{
	return true;
}

// -----------------------------------------------------------------------------------------
// This function is called just before the server dies
// -----------------------------------------------------------------------------------------
void CServerSocket::OnServerDie( )
{
	// DOESNT WORK - DAMN CONSOLE APPS
}

// -----------------------------------------------------------------------------------------
// This function is called, if a client receives data
// -----------------------------------------------------------------------------------------
bool CServerSocket::OnReceivePacket( CClientSocket* thisclient, unsigned char *P )
{
	return true;
}

// -----------------------------------------------------------------------------------------
// This function is called, if a client connects
// -----------------------------------------------------------------------------------------
bool CServerSocket::OnClientConnect( CClientSocket* thisclient )
{
	return true;
}

// -----------------------------------------------------------------------------------------
// This function is called, if a client disconnects
// -----------------------------------------------------------------------------------------
void CServerSocket::OnClientDisconnect( CClientSocket* thisclient )
{
}

// -----------------------------------------------------------------------------------------
// MySQL command
// -----------------------------------------------------------------------------------------
// Do sql query
bool CServerSocket::DoSQL(char *Format, ...)
{
	char Buffer[2000];
	va_list ap; va_start( ap, Format );
	vsprintf( Buffer, Format, ap );
	va_end  ( ap );
    int mres = mysql_query( mysql, Buffer );
    if (mres!=0)
    {
        Log( MSG_SQL, "Could not execute query: %s", mysql_error( mysql ) );
        Log( MSG_SQL, "Query: %s", Buffer );
        return false;
    }
    else
    {
        return true;
    }
}

// -----------------------------------------------------------------------------------------
