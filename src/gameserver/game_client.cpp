#include "game_sockets.h"

CConnClient::CConnClient( )
{
    PlayerSession = new CAccount;
    assert(PlayerSession);
    // Account
	PlayerSession->userid = 0;
    memset( PlayerSession->username, '\0', 17 );
    memset( PlayerSession->password, '\0', 33 );
	PlayerSession->accesslevel = 100;
	PlayerSession->isLoggedIn = false;
	PlayerSession->inGame = false;
	// Stats
    PlayerStats = new CStats;
    assert(PlayerStats);
    PlayerStats->MaxHP = 0;
    PlayerStats->MaxMP = 0;
	PlayerStats->HP = 0;
	PlayerStats->MP = 0;
	PlayerStats->MaxWeight = 0;
	PlayerStats->Str = 0;
	PlayerStats->Dex = 0;
	PlayerStats->Int = 0;
	PlayerStats->Con = 0;
	PlayerStats->Attack_Power = 0;
	PlayerStats->Defense = 0;
	PlayerStats->Magic_Defense = 0;
	PlayerStats->Accury = 0;
	PlayerStats->Critical = 0;
	PlayerStats->Dodge = 0;
	PlayerStats->Attack_Speed = 0;
	PlayerStats->Move_Speed = 0;
	//Position
	PlayerPosition = new CPosition;
	assert(PlayerPosition);
	PlayerPosition->RespawnID = 0;
	PlayerPosition->Pos_x = 0;
	PlayerPosition->Pos_y = 0;
	// Player Info
	PlayerInfo = new CInfo;
	assert(PlayerInfo);
	memset( PlayerInfo->char_name, '\0', 17 );
	PlayerInfo->classid = 0;
	PlayerInfo->Exp = 0;
	PlayerInfo->Gold = 0;
	PlayerInfo->Level = 0;
	PlayerInfo->mobid = 0;
	PlayerInfo->SkillPoints = 0;
	PlayerInfo->StatPoints = 0;
	PlayerInfo->Storage_Gold = 0;

	VisibleClients.clear();
	isInvisibleMode = false;
}

CConnClient::~CConnClient( )
{
}

bool CConnClient::loaddata()
{
    MYSQL_RES *result;
	MYSQL_ROW row;
	if(!GServer->DoSQL("SELECT id,cexp,clevel,gold,skpoints,stpoints,storage_gold,max_hp, \
	max_mp,cstr,cint,cdex,ccon,mobid,pos_x,pos_y,classid FROM characters \
	WHERE name='%s' AND uid='%s'", PlayerInfo->char_name, PlayerSession->username))
        return false;
	result = mysql_store_result( GServer->mysql );
    row = mysql_fetch_row(result);
    PlayerSession->userid = 30000 + atoi(row[0]);
    PlayerInfo->Exp = atoi(row[1]);
    PlayerInfo->Level = atoi(row[2]);
    PlayerInfo->Gold = atoi(row[3]);
    PlayerInfo->SkillPoints = atoi(row[4]);
    PlayerInfo->StatPoints = atoi(row[5]);
    PlayerInfo->Storage_Gold = atoi(row[6]);
    PlayerStats->MaxHP = atoi(row[7]);
    PlayerStats->MaxMP = atoi(row[8]);
    PlayerStats->HP = atoi(row[7]);
    PlayerStats->MP = atoi(row[8]);
    PlayerStats->Str = atoi(row[9]);
    PlayerStats->Int = atoi(row[10]);
    PlayerStats->Dex = atoi(row[11]);
    PlayerStats->Con = atoi(row[12]);
    PlayerInfo->mobid = atoi(row[13]);
    PlayerPosition->Pos_x = atoi(row[14]);
    PlayerPosition->Pos_y = atoi(row[15]);
    PlayerInfo->classid = atoi(row[16]);
    mysql_free_result(result);
    if(!GServer->DoSQL("SELECT slotnum,itemid,add1,add2,add3,addval1,addval2,addval3 FROM char_items WHERE owner='%s'", PlayerInfo->char_name))
	   return true;
	result = mysql_store_result(GServer->mysql);
	for(unsigned j=0; j<77; j++)
            ClearItem( items[j] );
	while(row = mysql_fetch_row(result))
    {
		UINT itemnum = atoi(row[0]);
		items[itemnum].itemid = atoi(row[1]);
		items[itemnum].add1 = atoi(row[2]);
		items[itemnum].add2 = atoi(row[3]);
		items[itemnum].add3 = atoi(row[4]);
		items[itemnum].val1 = atoi(row[5]);
		items[itemnum].val2 = atoi(row[6]);
		items[itemnum].val3 = atoi(row[7]);
    }
    mysql_free_result(result);
	PlayerSession->isLoggedIn = true;
    //GServer->DoSQL("UPDATE accounts SET online=true where id=%u", PlayerSession->userid );
	return true;
}

bool CConnClient::savedata()
{
    MYSQL_RES *result;
	MYSQL_ROW row;
}
