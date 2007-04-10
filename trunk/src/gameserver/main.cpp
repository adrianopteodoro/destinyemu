#include "game_sockets.h"

CConnServer* GServer;
int servernum;

int main(int argc, char *argv[])
{

    // Show Emulator Header
    ShowHeader();

    // Initialize MySQL
    MYSQL mysql;
	mysql_init( &mysql );

    // Initialize Winsock
	WSADATA wsa;
	int err = WSAStartup( MAKEWORD(2,2), &wsa );

	if ( err != 0 || wsa.wVersion != MAKEWORD(2,2) )
	{
		Log(MSG_FATALERROR, "Error initializing Winsock 2.2: %i\n", WSAGetLastError( ) );
		system("PAUSE");
		return -1;
	} else { Log(MSG_INFO, "Initialized and verified Winsock 2.2" ); }

	CConnServer *server = new CConnServer();
	server->LoadConfigs();
    // Initialize our mysql connection
	if ( !mysql_real_connect( &mysql, server->myhost.c_str(), server->myuser.c_str(), server->mypass.c_str(), server->mydb.c_str(), 0, NULL, 0 ) )
	{
		Log(MSG_FATALERROR, "Error connecting to MySQL server: %s\n", mysql_error( &mysql ) );
		system("PAUSE");
		return -1;
	}
	else
	{
        Log(MSG_INFO, "Connected to MySQL server" );
    }
    server->mysql = &mysql;
	server->StartServer();

	// Clean up everything
	delete server;
	mysql_close( &mysql );
	WSACleanup( );
	return 0;
}
