#ifndef __WYDEMU_SERVERS__
#define __WYDEMU_SERVERS__

#include "../commom/sockets.h"
#include "game_player.h"
#include "game_types.h"

class CConnServer : public CServerSocket
{
public:
	CConnServer ( );
	~CConnServer( );
    CConnClient* CreateClientSocket( );
    bool OnServerReady( );
	void DeleteClientSocket( CClientSocket* thisclient );

	// Packet control
	bool OnReceivePacket( CClientSocket* thisclient, unsigned char* P );
	bool CheckLogin( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool SendCharList( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool ResendCharList( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool CharProcess( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool CharCreate( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool CharDelete( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool SendToWorld( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool ObjectMove( CEncDec* encdec, CConnClient* thisclient, unsigned char* P );
	bool SendServerMsg( CEncDec* encdec, CConnClient* thisclient ,char* Format, ...);
	void LoadConfigs();
	int myport;
	std::string myhost;
	std::string myuser;
	std::string mydb;
	std::string mypass;
	char* versionfile;
	char* datfile;
};

extern class CConnServer* GServer;

bool readXMLInteger(xmlNodePtr node, const char* tag, int& value);
bool readXMLString(xmlNodePtr node, const char* tag, std::string& value);

#endif
