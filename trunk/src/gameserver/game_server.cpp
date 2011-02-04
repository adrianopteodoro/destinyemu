#include "game_sockets.h"

CConnServer::CConnServer( )
{
    for(unsigned i=0; i<0xffff; i++)
        ClientIDList[i] = 1;
    LoadConfigs();
    LoadLanguage();
    LoadNPCList();
    LoadItemList();
}

CConnServer::~CConnServer( )
{
}

// Load or main process
void CConnServer::OnServerStep( )
{
    pthread_create( &WorldThread[WORLD_THREAD], &at, WorldProcess, NULL);
    pthread_create( &WorldThread[VISUALITY_THREAD], &at, VisibilityProcess, NULL);
    pthread_create( &MapThread[0], &at, MapProcess, NULL);
}

bool CConnServer::OnServerReady( )
{
    ServerOnline = true;
    GServer = this;
    clock_t timer = clock();
    CEncDec *encdec = new CEncDec();
    encdec->LoadLib();
    if ( ReadKeysFromFile( ".\\clientkeys.dat", this->CKeys ) > 0){
		Log( MSG_WARNING, "Error on reading Keys from file!" );
	}else{
	    Log( MSG_INFO, "Succefully loaded Encryption Keys from \"clientkeys.dat\"" );
    }
    Log( MSG_INFO, "Server Rates: XP - %ix | DROP - %ix | GOLD - %ix", this->xprate, this->droprate, this->goldrate );
    float loadtime = (float)( clock() - timer ) / CLOCKS_PER_SEC;
	Log(  MSG_INFO, "Server took %.4f seconds to load", loadtime );
    return true;
}

CConnClient* CConnServer::CreateClientSocket( )
{
	CConnClient* thisclient;
	thisclient = new CConnClient( );
	return thisclient;
}

bool CConnServer::OnClientConnect(CClientSocket* thisclient)
{
	unsigned char rawData[39] =
	{
		0x13, 0x57, 0x01, 0x23, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x70, 0xB7, 0x40, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 
		0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	};
	thisclient->SendPacket(rawData, sizeof(rawData));
	return true;
}

void CConnServer::DeleteClientSocket( CClientSocket* thisclient )
{
	pthread_mutex_lock( &PlayerMutex );
	delete (CConnClient*)thisclient;
	pthread_mutex_unlock( &PlayerMutex );
}

// disconect all the clients
void CConnServer::DisconnectAll()
{
    for(UINT i=0;i<ClientList.size();i++)
    {
        CConnClient* otherclient = (CConnClient*) ClientList.at(i);
		if (otherclient->PlayerSession->inGame)
		{
            otherclient->isActive = false;
        }
	}
    Sleep(10);
}

bool CConnServer::OnReceivePacket( CClientSocket* thisclient, unsigned char* P )
{
    if(P[0] + P[1] == 0x74)
	{
		return CheckLogin    ( (CConnClient*)thisclient, P );
    }
	else if(P[4] + P[5] == 0x74)
	{
	    return CheckLogin    ( (CConnClient*)thisclient, P );
    }
	else
    {
        int packet_size = 0;
        int lastbyte = 0;
        int DecLen = 0;
        while (thisclient->PSize != lastbyte)
        {
            memcpy( &packet_size, &P[lastbyte], 2 );
            unsigned char* buff = (unsigned char*)malloc(packet_size);
            DecLen = encdec->WYD2_Decrypt(buff, &P[lastbyte], packet_size, this->CKeys);
			unsigned short opcode;
			unsigned short size;
			memcpy( &size, &buff[0], 2 );
			memcpy( &opcode, &buff[4], 2 );
			Log(  MSG_INFO, "OPCODE: 0x%04x SIZE: %d", opcode, size );
			FILE *fh = NULL;
			char* file = new char[60];
			sprintf_s(file, 60, ".\\logs\\packet_%04x.bin", opcode);
			fh = fopen( file, "wb" );
			if ( fh != NULL )
			{
				fwrite(buff, 1, packet_size, fh);
				fclose( fh );
			}
            lastbyte = lastbyte + packet_size;
            return PacketControl   ( (CConnClient*)thisclient, packet_size, buff );
        }
    }
    return true;
}
