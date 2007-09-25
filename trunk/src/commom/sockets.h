#ifndef __WYDEMU_SOCKETS__
#define __WYDEMU_SOCKETS__

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
	#include <winbase.h>
    #define close closesocket
#else
    #include <unistd.h>
    #include <netinet/in.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
    #include <cstdarg>
    #include <errno.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define PVOID void *
    #define ioctlsocket ioctl
    #define SOCKADDR struct sockaddr
    #define closesocket close
#endif
#include <mysql/mysql.h>
#include <libxml\parser.h>
#include <libxml\xmlmemory.h>
#include <libxml\threads.h>
#include <process.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <exception>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <vector>
#include <pthread.h>
#include <csignal>
#include "log.h"
#include "config.h"
#include "packetwriter.h"
#include "encdec.h"

typedef char SBYTE;
typedef unsigned char BYTE;
typedef short SWORD;
typedef unsigned short WORD;
typedef long SDWORD;
typedef unsigned long DWORD;
typedef long long SQWORD;
typedef unsigned long long QWORD;
#define pi 3.1415926535897932384626433832795
#define r2d pi/180

using namespace std;

// -----------------------------------------------------------------------------------------
// Base Socket Definition
// -----------------------------------------------------------------------------------------
class CBaseSocket {
public:
	CBaseSocket( );						// Our base socket constructor
	virtual ~CBaseSocket( );			// Our base socket destructor

	SOCKET			sock;				// This is our socket
	bool			isActive;			// Is this socket connected?
    unsigned long               Hash1;             // Wyd Hash1
    unsigned char               CKeys[512];        // Wyd Client Keys
    unsigned char               KeysLogin[16];     // Wyd Login Keys
    unsigned char               KeysChar[16];      // Wyd Char Keys
    unsigned char               encbuf[65355];
    int                         encsize;
};

// -----------------------------------------------------------------------------------------
// Client Socket Definition
// -----------------------------------------------------------------------------------------
class CClientSocket : public CBaseSocket {
public:
	CClientSocket( );									// Our client socket constructor
	~CClientSocket( );									// Our client socket destructor

	bool ReceiveData( );
	void SendPacket( unsigned char* P, int size );

	class CServerSocket*		ParentServer;			// Our associated server
	int                         PSize;
    int                         recvpkts;           // Received Packets

private:
	unsigned short				PacketSize;				// Size of the current packet
	unsigned short				PacketOffset;			// Current offset of the data
	unsigned char				Buffer[65355];		// Buffer to receive data into
};


// -----------------------------------------------------------------------------------------
// Server Socket Definition
// -----------------------------------------------------------------------------------------
class CServerSocket : public CBaseSocket {
public:
	CServerSocket( );									// Our server socket constructor
	~CServerSocket( );									// Our server socket destructor

	bool StartServer( std::string serverip );
	void ServerLoop( );
	void FillFDS( fd_set* fds );
	void HandleClients( fd_set* fds );
	void AddUser( SOCKET sock, sockaddr_in* ClientInfo );
	void DisconnectClient( CClientSocket* thisclient );
	bool DoSQL(char *Format, ...);

	virtual CClientSocket* CreateClientSocket( );
	virtual void DeleteClientSocket( CClientSocket* thisclient );
	virtual bool OnServerReady( );
	virtual void OnServerStep( );
	virtual void OnServerDie( );
	virtual bool OnReceivePacket( CClientSocket* thisclient, unsigned char *P );
	virtual bool OnClientConnect( CClientSocket* thisclient );
	virtual void OnClientDisconnect( CClientSocket* thisclient );

	std::vector<CClientSocket*>	ClientList;				// A list of all connected clients

	MYSQL*						mysql;
	unsigned short				port;
};

#endif
