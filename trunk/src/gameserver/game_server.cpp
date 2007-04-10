#include "game_sockets.h"

CConnServer::CConnServer( )
{
}

CConnServer::~CConnServer( )
{
}

void CConnServer::LoadConfigs()
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
            if (xmlStrcmp(p->name, (const xmlChar*)"autocreateacc") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->autocreateacc = strvalue;
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

bool CConnServer::OnServerReady( )
{
    GServer = this;
    CEncDec *encdec = new CEncDec();
    encdec->LoadLib();
    if ( ReadKeysFromFile( ".\\clientkeys.dat", this->CKeys ) > 0){
		Log( MSG_WARNING, "Error on reading Keys from file!" );
	}else{
	    Log( MSG_INFO, "All Client Keys Loaded" );
    }
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
	delete (CConnClient*)thisclient;
}

bool CConnServer::OnReceivePacket( CClientSocket* thisclient, unsigned char* P )
{
    CEncDec* encdec = new CEncDec();
    switch( P[0] + P[1] )
	{
        case 0x74: return CheckLogin    ( (CEncDec*)encdec, (CConnClient*)thisclient, P );
        case 0x24: return CharProcess   ( (CEncDec*)encdec, (CConnClient*)thisclient, P );
        case 0x26: return CharDelete    ( (CEncDec*)encdec, (CConnClient*)thisclient, P );
        case 0x34: return ObjectMove    ( (CEncDec*)encdec, (CConnClient*)thisclient, P );
    }
    switch( P[4] + P[5] )
	{
	    case 0x74: return CheckLogin    ( (CEncDec*)encdec, (CConnClient*)thisclient, P );
    }
	return true;
}
