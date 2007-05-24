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
	void SendToVisible( CEncDec* encdec, CConnClient* thisclient, bufwrite* pak, int size, bool dothisclient );
	bool IsVisible( CConnClient* thisclient, CConnClient* otherclient );
	float distance( fPoint pos1, fPoint pos2 );
	unsigned GetNewClientID( );
	void SendToAll( CEncDec* encdec, bufwrite* pak, int size );
	bool LoadConfigs();

	// Packet control
	bool OnReceivePacket( CClientSocket* thisclient, unsigned char* P );
	bool CheckLogin( CConnClient* thisclient, unsigned char* P );
	bool PacketControl( CConnClient* thisclient, int size, unsigned char* P );
	bool SendCharList( CConnClient* thisclient, unsigned char* P );
	bool ResendCharList( CConnClient* thisclient, unsigned char* P );
	bool CharCreate( CConnClient* thisclient, unsigned char* P );
	bool CharDelete( CConnClient* thisclient, unsigned char* P );
	bool SendToWorld( CConnClient* thisclient, unsigned char* P );
	bool ObjectMove( CConnClient* thisclient, unsigned char* P );
	bool SendServerMsg( CConnClient* thisclient ,char* Format, ...);
	bool SpawnChar( CConnClient* thisclient, CConnClient* otherclient );
	bool SendChat( CConnClient* thisclient, unsigned char* P );
	bool ChangeInventory( CConnClient* thisclient, unsigned char* P );
	bool packetCommand( CConnClient* thisclient, unsigned char* P );
	bool SendNPCSellItems( CConnClient* thisclient, unsigned char* P );

	// GM Commands Functions
	bool pakGMNotice( CConnClient* thisclient, unsigned char* P );
	bool pakGMTele( CConnClient* thisclient, unsigned char* P, int x, int y );
	bool pakGMSpawn( CConnClient* thisclient, unsigned char* P, int mobcode, int mobid, char* name );

	// Lists
	UINT ClientIDList[0x10000];	// Clients List

	// configuration things
	int myport;
	int autoacc;
	int xprate;
	int droprate;
	int goldrate;
	int goldchance;
	int cliversion;
	std::string myhost;
	std::string myuser;
	std::string mydb;
	std::string mypass;
	std::string srvname;
	char* versionfile;
	char* datfile;

	//timestamp
	clock_t curtime;

    bool ServerOnline;
    pthread_t WorldThread[3];
    pthread_t MapThread[200];
	pthread_attr_t at;
};

extern class CConnServer* GServer;
extern class CEncDec* encdec;
extern class bufwrite* packet;
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
