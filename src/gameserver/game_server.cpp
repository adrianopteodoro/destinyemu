#include "game_sockets.h"

CConnServer::CConnServer( )
{
    int intvalue;
    std::string strvalue;
    xmlDocPtr doc = xmlParseFile(".\\conf\\gameserver.xml");
    if (doc)
    {
        xmlNodePtr root, p;
        root = xmlDocGetRootElement(doc);
        if(xmlStrcmp(root->name,(const xmlChar*)"wydserver") != 0){
            xmlFreeDoc(doc);
            Log( MSG_FATALERROR, "Error on Loading \".\\conf\\gameserver.xml\"" );
        }

        p = root->children;

        while (p)
        {
            if (xmlStrcmp(p->name, (const xmlChar*)"port") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->port = intvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_port") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->myport = intvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_host") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->myhost = strvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_user") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->myuser = strvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_pass") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->mypass = strvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_db") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->mydb = strvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"servername") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->srvname = strvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"autoacc") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->autoacc = intvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"xp_rate") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->xprate = intvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"drop_rate") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->droprate = intvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"gold_rate") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->goldrate = intvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"gold_drop´chance") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->goldchance = intvalue;
                }
            }
            p = p->next;
        }
        xmlFreeDoc(doc);
        Log( MSG_INFO, "Loaded all configurations." );
    }
    else
    {
        Log( MSG_FATALERROR, "Error on Loading \".\\conf\\gameserver.xml\"" );
        #ifdef _WIN32
            system("PAUSE");
        #else
            return -1;
        #endif
    }
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
    Log( MSG_INFO, "Process Loaded. WorldDelay %i | MapDelay %i | VisualDelay %i",700,10,200);
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
    CEncDec* encdec = new CEncDec();
    switch( P[0] + P[1] )
	{
        case 0x74: return CheckLogin    ( (CEncDec*)encdec, (CConnClient*)thisclient, P );
    }
    switch( P[4] + P[5] )
	{
	    case 0x74: return CheckLogin    ( (CEncDec*)encdec, (CConnClient*)thisclient, P );
    }
	return PacketControl   ( (CEncDec*)encdec, (CConnClient*)thisclient, P );
}
