#ifndef __WYDEMU_SERVERS__
#define __WYDEMU_SERVERS__

#include "../commom/sockets.h"
#include "game_player.h"
#include "game_types.h"
#define MAXVISUALRANGE 100
#define MINVISUALRANGE 60
#define WORLD_THREAD 0
#define VISUALITY_THREAD 1
#define SHUTDOWN_THREAD 2

class CConnServer : public CServerSocket
{
public:
	CConnServer ( );
	~CConnServer( );
    CConnClient* CreateClientSocket( );
    bool OnServerReady( );
    void OnServerStep( );
	void DeleteClientSocket( CClientSocket* thisclient );
	void DisconnectAll();

	// Functions
	void SendToVisible( CEncDec* encdec, CConnClient* thisclient, bufwrite* pak, bool dothisclient );

	// Packet control
	bool OnReceivePacket( CClientSocket* thisclient, unsigned char* P );
	bool CheckLogin( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool PacketControl( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool SendCharList( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool ResendCharList( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool CharCreate( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool CharDelete( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool SendToWorld( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool ObjectMove( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool SendServerMsg( CEncDec* encdec, CConnClient* thisclient ,char* Format, ...);

	// configuration things
	int myport;
	int autoacc;
	int xprate;
	int droprate;
	int goldrate;
	int goldchance;
	std::string myhost;
	std::string myuser;
	std::string mydb;
	std::string mypass;
	std::string srvname;
	char* versionfile;
	char* datfile;

    bool ServerOnline;
    pthread_t WorldThread[3];
    pthread_t MapThread[200];
	pthread_attr_t at;
};

extern class CConnServer* GServer;
extern pthread_mutex_t MainMutex, PlayerMutex, MonsterMutex, DropMutex;

PVOID VisibilityProcess( PVOID TS );
PVOID MapProcess( PVOID TS );
PVOID WorldProcess( PVOID TS );
PVOID ShutdownServer( PVOID TS );

void StartSignal( );
void StopSignal( );
void HandleSignal( int num );

bool readXMLInteger(xmlNodePtr node, const char* tag, int& value);
bool readXMLString(xmlNodePtr node, const char* tag, std::string& value);

#endif
