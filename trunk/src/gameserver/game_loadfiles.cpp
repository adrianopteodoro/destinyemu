#include "game_sockets.h"

bool CConnServer::LoadConfigs()
{
    int intvalue;
    std::string strvalue;
    xmlDocPtr doc = xmlParseFile(".\\serverconfig.xml");
    if (doc)
    {
        xmlNodePtr root, p;
        root = xmlDocGetRootElement(doc);
        if(xmlStrcmp(root->name,(const xmlChar*)"wydserver") != 0){
            xmlFreeDoc(doc);
            Log( MSG_FATALERROR, "Error on Loading \".\\conf\\gameserver.xml\"" );
            return false;
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
            if (xmlStrcmp(p->name, (const xmlChar*)"gold_dropchance") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->goldchance = intvalue;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"client_version") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->cliversion = intvalue;
                }
            }
            p = p->next;
        }
        xmlFreeDoc(doc);
        Log( MSG_INFO, "Loaded all configurations." );
        return true;
    }
    else
    {
        Log( MSG_FATALERROR, "Error on Loading \".\\conf\\gameserver.xml\"" );
        return false;
    }
}
