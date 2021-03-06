#include "game_sockets.h"

CConnServer* GServer;
pthread_mutex_t MainMutex, PlayerMutex, MonsterMutex, DropMutex;

int main(int argc, char *argv[])
{
    // Show Emulator Header
    ShowHeader();
    Log( MSG_NOTICE, "SVN Nightly Build" );
printf("\tYou AGREE with terms:\n\tThis is a Nightly Build Development version\n\tnot work with it on a full server,\n\tor other things this is for testings\n\tonly and help reporting bugs to team.\n\
\t\n\tThis is a Alpha Development Version.\n\t\n\tPlease Report all bugs, to Tracker.\n\n");
    StartSignal( );
    // Initialize MySQL
    MYSQL mysql;
    if (!mysql_thread_safe())
    {
        Log( MSG_ERROR, "Mysql is not tread-safe" );
        #ifdef _WIN32
        system("pause");
        #endif
        return -1;
    }
	mysql_init( &mysql );

	pthread_mutex_init( &MainMutex, NULL );
    pthread_mutex_init( &PlayerMutex, NULL );
    pthread_mutex_init( &MonsterMutex, NULL );
    pthread_mutex_init( &DropMutex, NULL );

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
	if(server==NULL)
        return -1;
    pthread_attr_init(&server->at);
    pthread_attr_setdetachstate(&server->at, PTHREAD_CREATE_JOINABLE);
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
	server->StartServer(server->ipadress);

	// Clean up everything
	pthread_attr_destroy(&server->at);
	delete server;
	mysql_close( &mysql );
	WSACleanup( );
	StopSignal( );
	pthread_mutex_destroy( &DropMutex );
	pthread_mutex_destroy( &MonsterMutex );
	pthread_mutex_destroy( &PlayerMutex );
	pthread_mutex_destroy( &MainMutex );
	return EXIT_SUCCESS;
}
