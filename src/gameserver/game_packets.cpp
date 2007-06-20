#include "game_sockets.h"

CEncDec* encdec = new CEncDec();
bufwrite* packet = new bufwrite();

bool CConnServer::ChangeInventory( CConnClient* thisclient, unsigned char* P )
{
    packet->Free();
    for (int i=0;i<21;i++)
        packet->AddByte( P[i], i );
    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 20, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::SendChat( CConnClient* thisclient, unsigned char* P )
{
    packet->Free();
    for (int i=0;i<109;i++)
        packet->AddByte( P[i], i );
    SendToVisible( encdec, thisclient, packet, 108, false );
	return true;
}

bool CConnServer::ObjectMove( CConnClient* thisclient, unsigned char* P )
{
    unsigned short sdid;
    unsigned short dx;
    unsigned short dy;
    unsigned short cx;
    unsigned short cy;

    memcpy( &sdid, &P[6], 2 );
    memcpy( &cx, &P[12], 2 );
    memcpy( &cy, &P[14], 2 );
    memcpy( &dx, &P[24], 2 );
    memcpy( &dy, &P[26], 2 );

    packet->Free();
    for (int i=0;i<53;i++)
        packet->AddByte( P[i], i );
    SendToVisible( encdec, thisclient, packet, 52, false );
    thisclient->PlayerPosition->Cpos.x = dx;
    thisclient->PlayerPosition->Cpos.y = dy;
    thisclient->PlayerPosition->Dpos.x = dx;
    thisclient->PlayerPosition->Dpos.y = dy;
	return true;
}

bool CConnServer::CharDelete( CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_RES *result2;
	MYSQL_ROW row2;
    char delcharname[15];
    char delpassword[12];
    memcpy( delcharname, &P[16], 15 );
    memcpy( delpassword, &P[32], 12 );

    if(!DoSQL( "SELECT * FROM accounts WHERE username='%s' AND password='%s'", thisclient->PlayerSession->username, delpassword ))
        return false;
    result = mysql_store_result( mysql );
    if (mysql_num_rows( result ) == 1)
    {
        if(!DoSQL( "SELECT id FROM characters WHERE name='%s'", delcharname ))
            return false;
        result2 = mysql_store_result( mysql );
        row2 = mysql_fetch_row(result2);
        DoSQL("DELETE FROM char_items WHERE owner='%i'", row2[0]);
        DoSQL("DELETE FROM characters WHERE name='%s'", delcharname);
        return ResendCharList( thisclient, P );
    }
    else
    {
        return SendServerMsg( thisclient, &lang->pwd_invalid[0] );
    }
}

bool CConnServer::CharCreate( CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_RES *result2;
	MYSQL_ROW row2;
    char newcharname[15];
    memcpy( newcharname, &P[16], 15 );
    if(!DoSQL( "SELECT name FROM characters WHERE name='%s'", newcharname ))
        return false;
    result = mysql_store_result( mysql );
    if (mysql_num_rows( result ) == 0)
    {
        LoadCreateChar( thisclient, (int)P[32], newcharname, (int)P[12] );
        return ResendCharList( thisclient, P );
    }
    else
    {
        return SendServerMsg( thisclient, &lang->name_alreadyexist[0] );
    }
}

bool CConnServer::SendToWorld( CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
    packet->Free();

    if(!DoSQL( "SELECT name FROM characters WHERE uid='%s' AND posid='%i'", thisclient->PlayerSession->username, P[12] ))
        return false;
    result = mysql_store_result( mysql );

    packet->AddWord( 1244, 0 ); // packet size
    packet->AddWord( 276, 4 ); // packet id
    packet->AddWord( thisclient->PlayerSession->clientid, 6 ); //Player ID

    while ( row = mysql_fetch_row( result ) )
    {
        Log( MSG_INFO, "Account \"%s\" has selected the character \"%s\"", thisclient->PlayerSession->username, row[0] );
        strcpy( thisclient->PlayerInfo->char_name, row[0] );
    }
    // Load Char data
    thisclient->loaddata();

    packet->AddWord( (int)thisclient->PlayerPosition->Cpos.x, 12 );
    packet->AddWord( (int)thisclient->PlayerPosition->Cpos.y, 14 );
    packet->AddStr( thisclient->PlayerInfo->char_name, 16 ); // charname
    packet->AddDWord( thisclient->PlayerInfo->Gold, 40 ); // gold
    packet->AddDWord( thisclient->PlayerInfo->Exp, 44 ); // earned experience
    packet->AddWord( (int)thisclient->PlayerPosition->Cpos.x, 48 );
    packet->AddWord( (int)thisclient->PlayerPosition->Cpos.y, 50 );
    packet->AddByte( thisclient->PlayerInfo->classid, 36 ); // classindent
    packet->AddWord( thisclient->PlayerInfo->Level, 80 ); // lvl
    packet->AddWord( thisclient->PlayerStats->Defense, 82 ); // defense
    packet->AddWord( thisclient->PlayerStats->Attack_Power, 84 ); // ataque
    packet->AddWord( thisclient->PlayerStats->MaxHP, 88 ); // hp total
    packet->AddWord( thisclient->PlayerStats->MaxMP, 90 ); // mp total
    packet->AddWord( thisclient->PlayerStats->HP, 92 ); // hp
    packet->AddWord( thisclient->PlayerStats->MP, 94 ); // mp
    packet->AddWord( thisclient->PlayerStats->Str, 96 ); // str
    packet->AddWord( thisclient->PlayerStats->Int, 98 ); // int
    packet->AddWord( thisclient->PlayerStats->Dex, 100 ); // dex
    packet->AddWord( thisclient->PlayerStats->Con, 102 ); // con
    packet->AddByte( 200, 107 ); // skill master1
    packet->AddByte( 200, 104 ); // skill master2
    packet->AddByte( 200, 105 ); // skill master3
    packet->AddByte( 200, 106 ); // skill master4
    packet->AddByte( thisclient->PlayerInfo->mobid, 108 ); // mobile id (if player class id)
    packet->AddByte( 43, 110 ); // Player Add1
    packet->AddByte( 0, 111 );
    packet->AddByte( 86, 112 ); // Player Add2
    packet->AddByte( 0, 113 );
    packet->AddByte( 28, 114 ); // Player Add3
    packet->AddByte( 0, 115 );
    packet->AddByte( 4, 768 ); // resist1
    packet->AddByte( 4, 769 ); // resist2
    packet->AddByte( 4, 770 ); // resist3
    packet->AddByte( 4, 771 ); // resist4

    for (int a=0;a<5;a++)
        packet->AddByte( 0xff, 760+a );

    for (int a=0;a<18;a++)
        packet->AddByte( 0xff, 778+a );

    /*for (int a=0;a<200;a++)
        packet->AddByte( 150,600+a );*/

    //packet->AddWord( 150, 87 );
    packet->AddByte( thisclient->PlayerSession->clientid, 774 ); // ClientID

    // Load Inventory Items
    for (int i=0;i<78;i++)
    {
        packet->AddWord( thisclient->items[i].itemid, (8*i)+116 );
        packet->AddByte( thisclient->items[i].add1, (8*i)+118 );
        packet->AddByte( thisclient->items[i].val1, (8*i)+119 );
        packet->AddByte( thisclient->items[i].add2, (8*i)+120 );
        packet->AddByte( thisclient->items[i].val2, (8*i)+121 );
        packet->AddByte( thisclient->items[i].add3, (8*i)+122 );
        packet->AddByte( thisclient->items[i].val3, (8*i)+123 );
    }

    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 1244, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );

    thisclient->PlayerSession->inGame = true;
    thisclient->ready = true;
	return true;
}

bool CConnServer::ResendCharList( CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_RES *result2;
	MYSQL_ROW row2;
    packet->Free();

    // Packet Header
    packet->AddWord( 1824, 0 ); // packet size
    packet->AddWord( 274, 4 ); // packet id

    // Char List
    if(!DoSQL("SELECT id,name,cexp,clevel,gold, \
	cstr,cint,cdex,ccon,mobid,posid FROM characters \
	WHERE uid='%s'", thisclient->PlayerSession->username ))
        return false;
    result = mysql_store_result( mysql );
    CCharacter chars[6];
    unsigned posid = 0;
    for (int j=0;j<15;j++)
    {
        ClearItem( chars[0].eqitems[j] );
        ClearItem( chars[1].eqitems[j] );
        ClearItem( chars[2].eqitems[j] );
        ClearItem( chars[3].eqitems[j] );
    }
    while (row = mysql_fetch_row( result ))
    {
        strcpy( chars[posid].char_name, row[1] );
        chars[posid].Exp = atoi(row[2]);
        chars[posid].Level = atoi(row[3]);
        chars[posid].gold = atoi(row[4]);
        chars[posid].Str = atoi(row[5]);
        chars[posid].Int = atoi(row[6]);
        chars[posid].Dex = atoi(row[7]);
        chars[posid].Con = atoi(row[8]);
        chars[posid].Mobid = atoi(row[9]);
        if(!DoSQL("SELECT slotnum,itemid,add1,add2,add3,addval1,addval2,addval3 FROM char_items WHERE owner='%s'", row[0]))
            return true;
        result2 = mysql_store_result( mysql );
        while(row2 = mysql_fetch_row(result2))
        {
            unsigned itemnum = atoi(row2[0]);
            chars[posid].eqitems[itemnum].itemid = atoi(row2[1]);
            chars[posid].eqitems[itemnum].add1 = atoi(row2[2]);
            chars[posid].eqitems[itemnum].add2 = atoi(row2[3]);
            chars[posid].eqitems[itemnum].add3 = atoi(row2[4]);
            chars[posid].eqitems[itemnum].val1 = atoi(row2[5]);
            chars[posid].eqitems[itemnum].val2 = atoi(row2[6]);
            chars[posid].eqitems[itemnum].val3 = atoi(row2[7]);
        }
        posid++;
    }
    mysql_free_result( result );

    unsigned charnum = 0;
    for (int k=0;k<posid;k++)
    {
        int charpos;
        if(!DoSQL("SELECT posid FROM characters WHERE name='%s'", chars[k].char_name ))
        return false;
        result = mysql_store_result( mysql );
        while ( row = mysql_fetch_row( result ) )
            charpos = atoi(row[0]);
        packet->AddByte( 0x41, (2*charpos)+12 );
        packet->AddByte( 0x08, (2*charpos)+13 );
        packet->AddByte( 0x30, (2*charpos)+20 );
        packet->AddByte( 0x08, (2*charpos)+21 );
        packet->AddWord( chars[k].Level, (28*charpos)+92 ); // char level
        packet->AddStr( chars[k].char_name, (16*charpos)+28 ); // char name
        packet->AddByte( chars[k].Mobid, (128*charpos)+204 ); // mobid
        packet->AddWord( chars[k].Str, (28*charpos)+108 ); // char str
        packet->AddWord( chars[k].Int, (28*charpos)+110 ); // char int
        packet->AddWord( chars[k].Dex, (28*charpos)+112 ); // char dex
        packet->AddWord( chars[k].Con, (28*charpos)+114 ); // char con
        packet->AddDWord( chars[k].gold, (4*charpos)+724 ); // char gold
        packet->AddDWord( chars[k].Exp, (4*charpos)+740 ); // char exp

        unsigned slotnum = 0;
        for (int i=0;i<15;i++)
        {
            packet->AddWord( chars[charnum].eqitems[slotnum].itemid, (8*slotnum)+(128*charpos)+212 );
            packet->AddByte( chars[charnum].eqitems[slotnum].add1, (8*slotnum)+(128*charpos)+214 );
            packet->AddByte( chars[charnum].eqitems[slotnum].val1, (8*slotnum)+(128*charpos)+215 );
            packet->AddByte( chars[charnum].eqitems[slotnum].add2, (8*slotnum)+(128*charpos)+216 );
            packet->AddByte( chars[charnum].eqitems[slotnum].val2, (8*slotnum)+(128*charpos)+217 );
            packet->AddByte( chars[charnum].eqitems[slotnum].add3, (8*slotnum)+(128*charpos)+218 );
            packet->AddByte( chars[charnum].eqitems[slotnum].val3, (8*slotnum)+(128*charpos)+219 );
            slotnum++;
        }
        charnum++;
    }

    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 756, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::SendCharList( CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_RES *result2;
	MYSQL_ROW row2;
    packet->Free();

    // Packet Header
    packet->AddWord( 1824, 0 ); // packet size
    packet->AddWord( 270, 4 ); // packet id
    packet->AddWord( thisclient->PlayerSession->userid, 6 ); //Player ID

    // Char List
    if(!DoSQL("SELECT id,name,cexp,clevel,gold, \
	cstr,cint,cdex,ccon,mobid,posid FROM characters \
	WHERE uid='%s'", thisclient->PlayerSession->username ))
        return false;
    result = mysql_store_result( mysql );
    CCharacter chars[6];
    unsigned posid = 0;
    for (int j=0;j<15;j++)
    {
        ClearItem( chars[0].eqitems[j] );
        ClearItem( chars[1].eqitems[j] );
        ClearItem( chars[2].eqitems[j] );
        ClearItem( chars[3].eqitems[j] );
    }
    while (row = mysql_fetch_row( result ))
    {
        strcpy( chars[posid].char_name, row[1] );
        chars[posid].Exp = atoi(row[2]);
        chars[posid].Level = atoi(row[3]);
        chars[posid].gold = atoi(row[4]);
        chars[posid].Str = atoi(row[5]);
        chars[posid].Int = atoi(row[6]);
        chars[posid].Dex = atoi(row[7]);
        chars[posid].Con = atoi(row[8]);
        chars[posid].Mobid = atoi(row[9]);
        if(!DoSQL("SELECT slotnum,itemid,add1,add2,add3,addval1,addval2,addval3 FROM char_items WHERE owner='%s'", row[0]))
            return true;
        result2 = mysql_store_result( mysql );
        while(row2 = mysql_fetch_row(result2))
        {
            unsigned itemnum = atoi(row2[0]);
            chars[posid].eqitems[itemnum].itemid = atoi(row2[1]);
            chars[posid].eqitems[itemnum].add1 = atoi(row2[2]);
            chars[posid].eqitems[itemnum].add2 = atoi(row2[3]);
            chars[posid].eqitems[itemnum].add3 = atoi(row2[4]);
            chars[posid].eqitems[itemnum].val1 = atoi(row2[5]);
            chars[posid].eqitems[itemnum].val2 = atoi(row2[6]);
            chars[posid].eqitems[itemnum].val3 = atoi(row2[7]);
        }
        posid++;
    }
    mysql_free_result( result );

    unsigned charnum = 0;
    for (int k=0;k<posid;k++)
    {
        int charpos;
        if(!DoSQL("SELECT posid FROM characters WHERE name='%s'", chars[k].char_name ))
        return false;
        result = mysql_store_result( mysql );
        while ( row = mysql_fetch_row( result ) )
            charpos = atoi(row[0]);
        packet->AddByte( 0x41, (2*charpos)+12 );
        packet->AddByte( 0x08, (2*charpos)+13 );
        packet->AddByte( 0x30, (2*charpos)+20 );
        packet->AddByte( 0x08, (2*charpos)+21 );
        packet->AddWord( chars[k].Level, (28*charpos)+92 ); // char level
        packet->AddStr( chars[k].char_name, (16*charpos)+28 ); // char name
        packet->AddByte( chars[k].Mobid, (128*charpos)+204 ); // mobid
        packet->AddWord( chars[k].Str, (28*charpos)+108 ); // char str
        packet->AddWord( chars[k].Int, (28*charpos)+110 ); // char int
        packet->AddWord( chars[k].Dex, (28*charpos)+112 ); // char dex
        packet->AddWord( chars[k].Con, (28*charpos)+114 ); // char con
        packet->AddDWord( chars[k].gold, (4*charpos)+724 ); // char gold
        packet->AddDWord( chars[k].Exp, (4*charpos)+740 ); // char exp

        unsigned slotnum = 0;
        for (int i=0;i<15;i++)
        {
            packet->AddWord( chars[charnum].eqitems[slotnum].itemid, (8*slotnum)+(128*charpos)+212 );
            packet->AddByte( chars[charnum].eqitems[slotnum].add1, (8*slotnum)+(128*charpos)+214 );
            packet->AddByte( chars[charnum].eqitems[slotnum].val1, (8*slotnum)+(128*charpos)+215 );
            packet->AddByte( chars[charnum].eqitems[slotnum].add2, (8*slotnum)+(128*charpos)+216 );
            packet->AddByte( chars[charnum].eqitems[slotnum].val2, (8*slotnum)+(128*charpos)+217 );
            packet->AddByte( chars[charnum].eqitems[slotnum].add3, (8*slotnum)+(128*charpos)+218 );
            packet->AddByte( chars[charnum].eqitems[slotnum].val3, (8*slotnum)+(128*charpos)+219 );
            slotnum++;
        }
        charnum++;
    }

    // Client Username
    packet->AddStr( thisclient->PlayerSession->username, 1784 );

    // Char List Encryption Keys
    packet->AddStr( (char*)thisclient->KeysLogin, 1800 );
    packet->AddWord( thisclient->PlayerSession->clientid, 120 );
    packet->AddWord( thisclient->PlayerSession->clientid, 122 );
    packet->AddWord( thisclient->PlayerSession->clientid, 124 );
    packet->AddWord( thisclient->PlayerSession->clientid, 126 );

    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 1823, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::CheckLogin( CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    unsigned short cliver;
    unsigned short opcode;
    time_t rtime;
    if ( P[0] + P[1] == 0x74 )
    {
        encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, 116, (unsigned char*)this->CKeys );
        memcpy( this->KeysLogin, &buff[48], 16 );
        memcpy( thisclient->PlayerSession->username, &buff[12], 16 );
        memcpy( thisclient->PlayerSession->password, &buff[28], 12 );
        memcpy( &cliver, &buff[40], 2 );
    }
    if ( P[4] + P[5] == 0x74 )
    {
        encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)&P[4], 116, (unsigned char*)this->CKeys );
        memcpy( this->KeysLogin, &buff[48], 16 );
        memcpy( thisclient->PlayerSession->username, &buff[12], 16 );
        memcpy( thisclient->PlayerSession->password, &buff[28], 12 );
        memcpy( &cliver, &buff[40], 2 );
    }
    Log( MSG_INFO, "User login with account \"%s\", using cliver \"%i\"", thisclient->PlayerSession->username, cliver );
    this->Hash1 = encdec->WYD2_GetHash1( this->KeysLogin, thisclient->recvpkts );
    MYSQL_RES *result;
	MYSQL_ROW row;
    bufwrite *packet = new bufwrite();
    if(cliver >= this->cliversion)
    {
	if(!DoSQL( "SELECT username,password,active,online FROM accounts WHERE username='%s'", thisclient->PlayerSession->username ))
        return false;
    result = mysql_store_result( mysql );
    if (mysql_num_rows( result ) == 0) {
        if ( this->autoacc == 1 )
        {
            if(!DoSQL( "INSERT INTO `accounts` (`username`,`password`,`active`) VALUES ('%s','%s','1')", thisclient->PlayerSession->username, thisclient->PlayerSession->password ))
                return false;
            if(!DoSQL( "SELECT * FROM accounts WHERE username='%s'", thisclient->PlayerSession->username ))
                return false;
            result = mysql_store_result( mysql );
            while ( row = mysql_fetch_row( result ) )
            {
                thisclient->PlayerSession->userid = 40000 + atoi(row[0]);
                thisclient->PlayerSession->accesslevel = atoi(row[6]);
            }
            return SendCharList( thisclient, P );
        }
        else
        {
            return SendServerMsg( thisclient, &lang->id_invalid[0], thisclient->PlayerSession->username );
        }
    }
    else
    {
        if(!DoSQL( "SELECT username,password,active,online FROM accounts WHERE username='%s' AND password='%s'", thisclient->PlayerSession->username, thisclient->PlayerSession->password ))
            return false;
        result = mysql_store_result( mysql );
        if (mysql_num_rows( result ) == 0) {
            return SendServerMsg( thisclient, &lang->pwd_invalid[0] );
        }
        else
        {
            if(!DoSQL( "SELECT username,password,active,online FROM accounts WHERE username='%s' AND active='1'", thisclient->PlayerSession->username ))
                return false;
            result = mysql_store_result( mysql );
            if (mysql_num_rows( result ) == 0) {
                return SendServerMsg( thisclient, &lang->id_notactive[0], thisclient->PlayerSession->username );
            }
            else
            {
                if(!DoSQL( "SELECT id,username,password,active,online FROM accounts WHERE username='%s' AND online='0'", thisclient->PlayerSession->username ))
                    return false;
                result = mysql_store_result( mysql );
                if (mysql_num_rows( result ) == 0) {
                    return SendServerMsg( thisclient, &lang->id_online[0], thisclient->PlayerSession->username );
                }
                else
                {
                    if(!DoSQL( "SELECT id, acesslevel FROM accounts WHERE username='%s'", thisclient->PlayerSession->username ))
                        return false;
                    result = mysql_store_result( mysql );
                    while ( row = mysql_fetch_row( result ) )
                    {
                        // Assign a new id to this person
                        thisclient->PlayerSession->userid = atoi(row[0]);
                        thisclient->PlayerSession->clientid = GetNewClientID();
                        if (thisclient->PlayerSession->clientid <= 0)
                        {
                            Log( MSG_WARNING, "User '%s'(#%i) denied access. Server is full.", thisclient->PlayerSession->username, thisclient->PlayerSession->userid );
                            return false;
                        }
                        Log( MSG_INFO, "User '%s'(#%i) assigned id #%i", thisclient->PlayerSession->username, thisclient->PlayerSession->userid, thisclient->PlayerSession->clientid );
                        thisclient->PlayerSession->isLoggedIn = true;
                        thisclient->PlayerSession->accesslevel = atoi(row[1]);
                    }
                    return SendCharList( thisclient, P );
                }
            }
        }
    }
    }
    else
    {
        SendServerMsg( thisclient, "Wrong Client Version, Download Correct Version." );
    }
	return true;
}

bool CConnServer::SendServerMsg( CConnClient* thisclient ,char* Format, ...)
{
    char Buffer[2000];
	va_list ap; va_start( ap, Format );
	vsprintf( Buffer, Format, ap );
	va_end  ( ap );
	packet->Free();
    packet->AddWord( 108, 0 );
    packet->AddByte( 0x01, 4 );
    packet->AddByte( 0x01, 5 );
    packet->AddStr( Buffer, 12 );
    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 108, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
}

bool CConnServer::SpawnChar( CConnClient* thisclient, CConnClient* otherclient )
{
    packet->Free();
    packet->AddWord( 172, 0 );
    packet->AddWord( 0x0364, 4 );
    packet->AddWord( 30000, 6 );
    // end header

    packet->AddWord( (int)otherclient->PlayerPosition->Cpos.x, 12 ); // pos x
    packet->AddWord( (int)otherclient->PlayerPosition->Cpos.y, 14 ); // pos y
    packet->AddWord( otherclient->PlayerSession->clientid, 16 );
    for (int i=0;i<12;i++)
        packet->AddByte( 0xcc, 18+i );
    packet->AddStr( otherclient->PlayerInfo->char_name, 18 ); // char name
    packet->AddByte( 150, 30 ); // player karma
    packet->AddByte( otherclient->PlayerInfo->mobid, 34 ); // mob id
    for (int i=0;i<15;i++)
    {
        packet->AddWord( otherclient->items[i].itemid, (2*i)+36 );
    }
    packet->AddWord( otherclient->PlayerSession->clientid, 66 );
    packet->AddWord( 00, 69 );//Player Effect
    packet->AddWord( otherclient->PlayerInfo->Level, 100 );
    packet->AddWord( otherclient->PlayerStats->Attack_Power, 102 );
    packet->AddWord( otherclient->PlayerStats->Defense, 104 );
    packet->AddByte( 0, 106 );
    packet->AddByte( 84, 107 );
    packet->AddWord( otherclient->PlayerStats->MaxHP, 108 );//max hp
    packet->AddWord( otherclient->PlayerStats->MaxMP, 110 );//max mana
    packet->AddWord( otherclient->PlayerStats->HP, 112 );//current hp
    packet->AddWord( otherclient->PlayerStats->MP, 114 );//current mana
    packet->AddWord( otherclient->PlayerStats->Str, 116 );
    packet->AddWord( otherclient->PlayerStats->Int, 118 );
    packet->AddWord( otherclient->PlayerStats->Dex, 120 );
    packet->AddWord( otherclient->PlayerStats->Con, 122 );
    packet->AddWord( 2, 128 );
    packet->AddWord( 0, 130 );

    for (int i=0;i<29;i++)
    {
        packet->AddByte( 0xcc, 147+i );
    }
    packet->AddByte( 0x00, 172 );

    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 172, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
}

bool CConnServer::SendNPCSellItems( CConnClient* thisclient, unsigned char* P )
{
    packet->Free();
    packet->AddByte( 236, 0 );
    packet->AddByte( 0x7C, 4 );
    packet->AddByte( 0x01, 5 );
    packet->AddWord( 30000, 6 );
    packet->AddWord( 1, 12 );
    packet->AddWord( 698, 16 );
    packet->AddWord( 699, 22 );
    packet->AddWord( 496, 28 );
    packet->AddWord( 497, 34 );
    packet->AddWord( 410, 48 );
    packet->AddWord( 411, 54 );
    packet->AddWord( 1764, 60 );
    for(int i=0;i<9;i++)
    {
        packet->AddByte( 0xcc, 48+i );
    }
    for(int i=0;i<17;i++)
    {
        packet->AddByte( 0xcc, 88+i );
    }
    for(int i=0;i<9;i++)
    {
        packet->AddByte( 0xcc, 160+i );
    }
    packet->AddByte( 14, 232 );
    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 236, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
}

bool CConnServer::SpawnNPC( CConnClient* thisclient, CNPC* thisnpc )
{
    thisnpc->clientid = GetNewMobID();
    packet->Free();
    packet->AddWord( 172, 0 );
    packet->AddWord( 0x0364, 4 );
    packet->AddWord( 30000, 6 );
    // end header

    packet->AddWord( (int)thisnpc->pos.x, 12 ); // pos x
    packet->AddWord( (int)thisnpc->pos.y, 14 ); // pos y
    packet->AddWord( thisnpc->clientid, 16 );
    for (int i=0;i<12;i++)
        packet->AddByte( 0xcc, 18+i );
    packet->AddStr( thisnpc->npcname, 18 ); // char name
    if(thisnpc->mobcode == NPC_NORMAL)
        packet->AddByte( 150, 30 ); // player karma
    packet->AddByte( thisnpc->mobid, 34 ); // mob id
    for (int i=0;i<15;i++)
    {
        packet->AddWord( thisnpc->inventory[i].itemid, (2*i)+36 );
    }
    packet->AddByte( 1, 66 );
    packet->AddByte( 0, 67 );
    packet->AddWord( 0, 69 );//Player Effect
    packet->AddWord( thisnpc->level, 100 );// level
    packet->AddWord( 3, 102 );
    packet->AddWord( 93, 104 );
    packet->AddByte( thisnpc->mobcode, 106 );
    packet->AddByte( 1, 107 );// pet??
    packet->AddWord( 800, 108 );//max hp
    packet->AddWord( 0, 110 );//max mana
    packet->AddWord( 800, 112 );//current hp
    packet->AddWord( 0, 114 );//current mana
    packet->AddWord( thisnpc->n_str, 116 );//str
    packet->AddWord( thisnpc->n_int, 118 );//int
    packet->AddWord( thisnpc->n_dex, 120 );//dex
    packet->AddWord( thisnpc->n_con, 122 );//con
    packet->AddWord( 2047, 124 );//5
    packet->AddWord( 2047, 126 );//5
    packet->AddWord( 0, 128 );
    packet->AddWord( 0, 130 );

    for (int i=0;i<29;i++)
    {
        packet->AddByte( 0xcc, 147+i );
    }
    packet->AddByte( 0x00, 172 );

    this->curtime = clock();
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 172, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
}
