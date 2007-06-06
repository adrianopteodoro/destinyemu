#include "game_sockets.h"

bool CConnServer::LoadCreateChar( CConnClient* thisclient, int charclass, char* charname, int destpos )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
    char* cname;
    int mobid;
    int level;
    int cstr;
    int cint;
    int cdex;
    int ccon;
    int skpts;
    int stpts;
    int skmpts;
    int charid;
    int slotid;
    int itemid;
    int add1;
    int add2;
    int add3;
    int val1;
    int val2;
    int val3;
    xmlDocPtr doc = xmlParseFile(".\\data\\StartChar.xml");
    if (doc)
    {
        xmlNodePtr root, p, subp;
        root = xmlDocGetRootElement(doc);
        if(xmlStrcmp(root->name,(const xmlChar*)"startitems") != 0){
            xmlFreeDoc(doc);
            Log( MSG_FATALERROR, "Error on Loading \".\\data\\StartChar.xml\"" );
            return false;
        }

        p = root->children;

        while (p)
        {
            switch(charclass)
            {
                 case 0:
                      cname = "transknight";
                 break;
                 case 1:
                      cname = "foema";
                 break;
                 case 2:
                      cname = "bestmaster";
                 break;
                 case 3:
                      cname = "hunter";
                 break;
            }
            if (xmlStrcmp(p->name, (const xmlChar*)cname) == 0)
            {
                if(readXMLInteger(p, "mobid", mobid) &&
                readXMLInteger(p, "level", level) &&
                readXMLInteger(p, "str", cstr) &&
                readXMLInteger(p, "int", cint) &&
                readXMLInteger(p, "dex", cdex) &&
                readXMLInteger(p, "con", ccon) &&
                readXMLInteger(p, "skpts", skpts) &&
                readXMLInteger(p, "stpts", stpts) &&
                readXMLInteger(p, "skmpts", skmpts))
                {
                    DoSQL("INSERT INTO characters (name,uid,mobid,max_hp,max_mp,cstr,cint,cdex,ccon,gold,posid,classid,skpoints,stpoints,skmpoints) \
                    VALUES ('%s', '%s', '%i', 50, 10, '%i', '%i', '%i', '%i', 0, '%i', '%i', '%i', '%i', '%i')",
                    charname, thisclient->PlayerSession->username,
                    mobid, cstr, cint, cdex, ccon, destpos, charclass, skpts, stpts, skmpts);
                    if(!DoSQL( "SELECT id FROM characters WHERE name='%s'", charname ))
                        return false;
                    result = mysql_store_result( mysql );
                    row = mysql_fetch_row(result);
                    charid = atoi(row[0]);
                }
                subp = p->children;
                while(subp)
                {
                    if (xmlStrcmp(subp->name, (const xmlChar*)"inventory") == 0)
                    {
                        if(readXMLInteger(subp, "slot", slotid) &&
                        readXMLInteger(subp, "itemid", itemid) &&
                        readXMLInteger(subp, "add1", add1) &&
                        readXMLInteger(subp, "add2", add2) &&
                        readXMLInteger(subp, "add3", add3) &&
                        readXMLInteger(subp, "val1", val1) &&
                        readXMLInteger(subp, "val2", val2) &&
                        readXMLInteger(subp, "val3", val3))
                        {
                            DoSQL("INSERT INTO char_items (owner,slotnum,itemid,add1,addval1,add2,addval2,add3,addval3) \
                            VALUES ('%i', '%i', '%i', '%i', '%i', '%i', '%i', '%i', '%i')",
                            charid, slotid, itemid, add1, val1, add2, val2, add3, val3);
                        }
                    }
                    subp = subp->next;
                }
            }
            p = p->next;
        }
        xmlFreeDoc(doc);
        Log( MSG_INFO, "Account \"%s\" has created character \"%s\".", thisclient->PlayerSession->username, charname );
        return true;
    }
    else
    {
        Log( MSG_FATALERROR, "Error on Loading \".\\data\\StartChar.xml\"" );
        return false;
    }
}

bool CConnServer::LoadConfigs()
{
    int cfgcount = 0;
    int intvalue;
    std::string strvalue;
    xmlDocPtr doc = xmlParseFile(".\\serverconfig.xml");
    if (doc)
    {
        xmlNodePtr root, p;
        root = xmlDocGetRootElement(doc);
        if(xmlStrcmp(root->name,(const xmlChar*)"wydserver") != 0){
            xmlFreeDoc(doc);
            Log( MSG_FATALERROR, "Error on Loading \".\\serverconfig.xml\"" );
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
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_port") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->myport = intvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_host") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->myhost = strvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_user") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->myuser = strvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_pass") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->mypass = strvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"mysql_db") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->mydb = strvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"servername") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->srvname = strvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"autoacc") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->autoacc = intvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"xp_rate") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->xprate = intvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"drop_rate") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->droprate = intvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"gold_rate") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->goldrate = intvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"gold_dropchance") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->goldchance = intvalue;
                    cfgcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"client_version") == 0)
            {
                if(readXMLInteger(p, "value", intvalue))
                {
                    this->cliversion = intvalue;
                    cfgcount++;
                }
            }
            p = p->next;
        }
        xmlFreeDoc(doc);
        Log( MSG_INFO, "Loaded %i configurations in \".\\serverconfig.xml\".", cfgcount );
        return true;
    }
    else
    {
        Log( MSG_FATALERROR, "Error on Loading \".\\serverconfig.xml\"" );
        return false;
    }
}

bool CConnServer::LoadLanguage()
{
    int strcount = 0;
    int intvalue;
    std::string strvalue;
    lang = new CLanguage;
    assert(lang);
    xmlDocPtr doc = xmlParseFile(".\\data\\Language.xml");
    if (doc)
    {
        xmlNodePtr root, p;
        root = xmlDocGetRootElement(doc);
        if(xmlStrcmp(root->name,(const xmlChar*)"language") != 0){
            xmlFreeDoc(doc);
            Log( MSG_FATALERROR, "Error on Loading \".\\data\\Language.xml\"" );
            return false;
        }

        p = root->children;

        while (p)
        {
            if (xmlStrcmp(p->name, (const xmlChar*)"id_invalid") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    lang->id_invalid = strvalue;
                    strcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"id_online") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    lang->id_online = strvalue;
                    strcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"id_notactive") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    lang->id_notactive = strvalue;
                    strcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"pwd_invalid") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    lang->pwd_invalid = strvalue;
                    strcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"name_alreadyexist") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    lang->name_alreadyexist = strvalue;
                    strcount++;
                }
            }
            if (xmlStrcmp(p->name, (const xmlChar*)"wrong_cliver") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    lang->wrong_cliver = strvalue;
                    strcount++;
                }
            }
            p = p->next;
        }
        xmlFreeDoc(doc);
        Log( MSG_INFO, "Loaded %i strings in \".\\data\\Language.xml\".", strcount );
        return true;
    }
    else
    {
        Log( MSG_FATALERROR, "Error on Loading \".\\data\\Language.xml\"" );
        return false;
    }
}
