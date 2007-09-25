#include "game_sockets.h"
#include <fstream>

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
                    DoSQL("INSERT INTO characters (name,uid,mobid,max_hp,max_mp,cstr,cint,cdex,ccon,gold,posid,classid,skpoints,stpoints,skmpoints,clevel) \
                    VALUES ('%s', '%s', '%i', 50, 10, '%i', '%i', '%i', '%i', 0, '%i', '%i', '%i', '%i', '%i', '%i')",
                    charname, thisclient->PlayerSession->username,
                    mobid, cstr, cint, cdex, ccon, destpos, charclass, skpts, stpts, skmpts, level);
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
    Log( MSG_INFO, "Loading Config File..." );
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
            if (xmlStrcmp(p->name, (const xmlChar*)"ipadress") == 0)
            {
                if(readXMLString(p, "value", strvalue))
                {
                    this->ipadress = strvalue;
                    cfgcount++;
                }
            }
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
    Log( MSG_INFO, "Loading Language File..." );
//    int intvalue;
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

bool CConnServer::LoadNPCList()
{
    int strcount = 0;
    Log( MSG_INFO, "Loading NPC List..." );

    int posx;
    int posy;
    std::string strvalue;
    xmlDocPtr doc = xmlParseFile(".\\data\\NpcList.xml");
    if (doc)
    {
        xmlNodePtr root, p;
        root = xmlDocGetRootElement(doc);
        if(xmlStrcmp(root->name,(const xmlChar*)"npclist") != 0){
            xmlFreeDoc(doc);
            Log( MSG_FATALERROR, "Error on Loading \".\\data\\NpcList.xml\"" );
            return false;
        }

        p = root->children;

        while (p)
        {
            if (xmlStrcmp(p->name, (const xmlChar*)"npc") == 0)
            {
                if(readXMLInteger(p, "pos_x", posx) && readXMLInteger(p, "pos_y", posy) && readXMLString(p, "npcfile", strvalue))
                {
                    LoadNPCFile( &strvalue[0], posx, posy );
                    strcount++;
                }
            }
            p = p->next;
        }
        xmlFreeDoc(doc);
        Log( MSG_INFO, "Loaded %i NPCS in \".\\data\\NpcList.xml\".", strcount );
        return true;
    }
    else
    {
        Log( MSG_FATALERROR, "Error on Loading \".\\data\\NpcList.xml\"" );
        return false;
    }
}

bool CConnServer::LoadNPCFile( char* filename, int posx, int posy )
{
    std::string npcfile;
    npcfile = ".\\npcs\\";
    npcfile += filename;
    int slotnum;
    int intvalue;
    std::string strvalue;
    xmlDocPtr doc2 = xmlParseFile(npcfile.c_str());

    CNPC* thisnpc = new (nothrow) CNPC;
    for(unsigned j=0; j<15; j++)
        ClearItem( thisnpc->inventory[j] );
    thisnpc->pos.x = (float)posx;
    thisnpc->pos.y = (float)posy;

    if (doc2)
    {
        xmlNodePtr root2, p2, z2;
        root2 = xmlDocGetRootElement(doc2);
        while (root2)
        {
            if(xmlStrcmp(root2->name,(const xmlChar*)"npc") == 0)
            {
                if(readXMLString(root2, "name", strvalue))
                {
                    thisnpc->npcname = strvalue;
                }
                if(readXMLInteger(root2, "mobid", intvalue))
                {
                    thisnpc->mobid = intvalue;
                }
                if(readXMLInteger(root2, "mobcode", intvalue))
                {
                    thisnpc->mobcode = intvalue;
                }
                if(readXMLInteger(root2, "level", intvalue))
                {
                    thisnpc->level = intvalue;
                }
                if(readXMLInteger(root2, "str", intvalue))
                {
                    thisnpc->n_str = intvalue;
                }
                if(readXMLInteger(root2, "int", intvalue))
                {
                    thisnpc->n_int = intvalue;
                }
                if(readXMLInteger(root2, "dex", intvalue))
                {
                    thisnpc->n_dex = intvalue;
                }
                if(readXMLInteger(root2, "con", intvalue))
                {
                    thisnpc->n_con = intvalue;
                }
                p2 = root2->children;
                while(p2)
                {
                    if(xmlStrcmp(p2->name,(const xmlChar*)"inventory") == 0)
                    {
                        z2 = p2->children;
                        while(z2)
                        {
                            if(xmlStrcmp(z2->name,(const xmlChar*)"item") == 0)
                            {
                                if(readXMLInteger(z2, "slotnum", intvalue))
                                    slotnum = intvalue;
                                if(readXMLInteger(z2, "itemid", intvalue))
                                    thisnpc->inventory[slotnum].itemid = intvalue;
                                if(readXMLInteger(z2, "add1", intvalue))
                                    thisnpc->inventory[slotnum].add1 = intvalue;
                                if(readXMLInteger(z2, "val1", intvalue))
                                    thisnpc->inventory[slotnum].val1 = intvalue;
                                if(readXMLInteger(z2, "add2", intvalue))
                                    thisnpc->inventory[slotnum].add2 = intvalue;
                                if(readXMLInteger(z2, "val2", intvalue))
                                    thisnpc->inventory[slotnum].val2 = intvalue;
                                if(readXMLInteger(z2, "add3", intvalue))
                                    thisnpc->inventory[slotnum].add3 = intvalue;
                                if(readXMLInteger(z2, "val3", intvalue))
                                    thisnpc->inventory[slotnum].val3 = intvalue;
                            }
                            z2 = z2->next;
                        }
                    }
                    if(xmlStrcmp(p2->name,(const xmlChar*)"shopitems") == 0)
                    {
                        z2 = p2->children;
                        while(z2)
                        {
                            if(xmlStrcmp(z2->name,(const xmlChar*)"item") == 0)
                            {
                                if(readXMLInteger(z2, "slotnum", intvalue))
                                    slotnum = intvalue;
                                if(readXMLInteger(z2, "itemid", intvalue))
                                    thisnpc->shopitems[slotnum].itemid = intvalue;
                                if(readXMLInteger(z2, "add1", intvalue))
                                    thisnpc->shopitems[slotnum].add1 = intvalue;
                                if(readXMLInteger(z2, "val1", intvalue))
                                    thisnpc->shopitems[slotnum].val1 = intvalue;
                                if(readXMLInteger(z2, "add2", intvalue))
                                    thisnpc->shopitems[slotnum].add2 = intvalue;
                                if(readXMLInteger(z2, "val2", intvalue))
                                    thisnpc->shopitems[slotnum].val2 = intvalue;
                                if(readXMLInteger(z2, "add3", intvalue))
                                    thisnpc->shopitems[slotnum].add3 = intvalue;
                                if(readXMLInteger(z2, "val3", intvalue))
                                    thisnpc->shopitems[slotnum].val3 = intvalue;
                            }
                            z2 = z2->next;
                        }
                    }
                    p2 = p2->next;
                }
                this->NPCList.push_back( thisnpc );
            }
            root2 = root2->next;
        }
        xmlFreeDoc(doc2);
        return true;
    }
    else
    {
        return false;
    }
}

bool CConnServer::LoadItemList()
{
	int itemscount = 0;
	Log( MSG_INFO, "Loading ItemList..." );
	int intvalue;
	std::string strvalue;
	xmlDocPtr doc = xmlParseFile(".\\data\\ItemList.xml");

	CEquip* newitem = new (nothrow) CEquip;

	if (doc)
	{
		xmlNodePtr root, p;
		root = xmlDocGetRootElement(doc);
		if(xmlStrcmp(root->name,(const xmlChar*)"itemlist") != 0){
			xmlFreeDoc(doc);
			Log( MSG_FATALERROR, "Error on Loading \".\\data\\ItemList.xml\"" );
			return false;
		}

        p = root->children;
		while (p)
		{
			if (xmlStrcmp(p->name, (const xmlChar*)"item") == 0)
			{
				if(readXMLInteger(p, "id", intvalue))
					newitem->itemid = intvalue;
				if(readXMLString(p, "name", strvalue))
					newitem->name = strvalue.c_str();
				if(readXMLInteger(p, "mesh", intvalue))
					newitem->mesh = intvalue;
				if(readXMLInteger(p, "req_lvl", intvalue))
					newitem->rlvl = intvalue;
				if(readXMLInteger(p, "req_str", intvalue))
					newitem->req_str = intvalue;
				if(readXMLInteger(p, "req_int", intvalue))
					newitem->req_int = intvalue;
				if(readXMLInteger(p, "req_dex", intvalue))
					newitem->req_dex = intvalue;
				if(readXMLInteger(p, "req_con", intvalue))
					newitem->req_con = intvalue;
				if(readXMLInteger(p, "price", intvalue))
					newitem->price = intvalue;
				if(readXMLInteger(p, "pos", intvalue))
					newitem->pos = intvalue;
				if(readXMLInteger(p, "equip_type", intvalue))
					newitem->wtype = intvalue;
				if(readXMLInteger(p, "pos", intvalue))
					newitem->pos = intvalue;
				if(readXMLInteger(p, "N0", intvalue))
					newitem->effects[0].effectid = intvalue;
				if(readXMLInteger(p, "V0", intvalue))
					newitem->effects[0].value = intvalue;
				if(readXMLInteger(p, "N1", intvalue))
					newitem->effects[1].effectid = intvalue;
				if(readXMLInteger(p, "V1", intvalue))
					newitem->effects[1].value = intvalue;
				if(readXMLInteger(p, "N2", intvalue))
					newitem->effects[2].effectid = intvalue;
				if(readXMLInteger(p, "V2", intvalue))
					newitem->effects[2].value = intvalue;
				if(readXMLInteger(p, "N3", intvalue))
					newitem->effects[3].effectid = intvalue;
				if(readXMLInteger(p, "V3", intvalue))
					newitem->effects[3].value = intvalue;
				if(readXMLInteger(p, "N4", intvalue))
					newitem->effects[4].effectid = intvalue;
				if(readXMLInteger(p, "V4", intvalue))
					newitem->effects[4].value = intvalue;
				if(readXMLInteger(p, "N5", intvalue))
					newitem->effects[5].effectid = intvalue;
				if(readXMLInteger(p, "V5", intvalue))
					newitem->effects[5].value = intvalue;
				if(readXMLInteger(p, "N6", intvalue))
					newitem->effects[6].effectid = intvalue;
				if(readXMLInteger(p, "V6", intvalue))
					newitem->effects[6].value = intvalue;
				if(readXMLInteger(p, "N7", intvalue))
					newitem->effects[7].effectid = intvalue;
				if(readXMLInteger(p, "V7", intvalue))
					newitem->effects[7].value = intvalue;
				if(readXMLInteger(p, "N8", intvalue))
					newitem->effects[8].effectid = intvalue;
				if(readXMLInteger(p, "V8", intvalue))
					newitem->effects[8].value = intvalue;
				if(readXMLInteger(p, "N9", intvalue))
					newitem->effects[9].effectid = intvalue;
				if(readXMLInteger(p, "V9", intvalue))
					newitem->effects[9].value = intvalue;
				if(readXMLInteger(p, "N10", intvalue))
					newitem->effects[10].effectid = intvalue;
				if(readXMLInteger(p, "V10", intvalue))
					newitem->effects[10].value = intvalue;
				if(readXMLInteger(p, "N11", intvalue))
					newitem->effects[11].effectid = intvalue;
				if(readXMLInteger(p, "V11", intvalue))
					newitem->effects[11].value = intvalue;					

				//increment item
				itemscount++;
				this->EquipList.push_back( newitem );
			}
			p = p->next;
		}
		xmlFreeDoc(doc);
		Log( MSG_INFO, "Loaded %i Items in \".\\data\\ItemList.xml\".", itemscount );
		return true;
	}
	else
	{
		Log( MSG_FATALERROR, "Error on Loading \".\\data\\ItemList.xml\"" );
		return false;
	}
}

