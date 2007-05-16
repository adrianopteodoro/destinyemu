#include "game_sockets.h"

CEncDec* encdec = new CEncDec();
bufwrite* packet = new bufwrite();

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
    char delcharname[15];
    char delpassword[12];
    memcpy( delcharname, &P[16], 15 );
    memcpy( delpassword, &P[32], 12 );

    if(!DoSQL( "SELECT * FROM accounts WHERE username='%s' AND password='%s'", thisclient->PlayerSession->username, delpassword ))
        return false;
    result = mysql_store_result( mysql );
    if (mysql_num_rows( result ) == 1)
    {
        DoSQL("DELETE FROM characters WHERE name='%s'", delcharname);
        DoSQL("DELETE FROM char_items WHERE owner='%s'", delcharname);
        return ResendCharList( thisclient, P );
    }
    else
    {
        return SendServerMsg( thisclient, "Invalid password, try again." );
    }
}

bool CConnServer::CharCreate( CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
    char newcharname[15];
    memcpy( newcharname, &P[16], 15 );
    if(!DoSQL( "SELECT name FROM characters WHERE name='%s'", newcharname ))
        return false;
    result = mysql_store_result( mysql );
    if (mysql_num_rows( result ) == 0)
    {
        switch (P[32])
        {
            //transknight
            case 0:
            DoSQL("INSERT INTO characters (name,uid,mobid,max_hp,max_mp,cstr,cint,cdex,ccon,gold,posid,classid) \
            VALUES ('%s', '%s', 1, 50, 10, 8, 4, 7, 6, 1000, %i, 0)", newcharname, thisclient->PlayerSession->username, P[12]);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 0, 1101)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 1, 1113)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 2, 1125)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 3, 1137)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid,add1,addval1) VALUES ('%s',4,1149,29,3)", newcharname);
            for (int i=0;i<4;i++)
                DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', '%i', 400)", newcharname, 15+i);
            for (int i=0;i<4;i++)
                DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', '%i', 405)", newcharname, 19+i);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 23, 861)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 24, 917)", newcharname);
            break;
            //foema
            case 1:
            DoSQL("INSERT INTO characters (name,uid,mobid,max_hp,max_mp,cstr,cint,cdex,ccon,gold,posid,classid) \
            VALUES ('%s', '%s', 11, 40, 30, 5, 8, 5, 5, 1000, %i, 1)", newcharname, thisclient->PlayerSession->username, P[12]);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 0, 1254)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 1, 1266)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 2, 1278)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 3, 1290)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid,add1,addval1) VALUES ('%s', 4, 1302,29,3)", newcharname);
            for (int i=0;i<4;i++)
                DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', '%i', 400)", newcharname, 15+i);
            for (int i=0;i<4;i++)
                DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', '%i', 405)", newcharname, 19+i);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 23, 891)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 24, 918)", newcharname);
            break;
            // beastmaster
            case 2:
            DoSQL("INSERT INTO characters (name,uid,mobid,max_hp,max_mp,cstr,cint,cdex,ccon,gold,posid,classid) \
            VALUES ('%s', '%s', 21, 40, 30, 6, 6, 9, 5, 1000, %i, 2)", newcharname, thisclient->PlayerSession->username, P[12]);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 0, 1416)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 1, 1419)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 2, 1422)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 3, 1425)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid,add1,addval1) VALUES ('%s', 4, 1428,29,3)", newcharname);
            for (int i=0;i<4;i++)
                DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', '%i', 400)", newcharname, 15+i);
            for (int i=0;i<4;i++)
                DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', '%i', 405)", newcharname, 19+i);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 23, 861)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 24, 917)", newcharname);
            break;
            // hunter
            case 3:
            DoSQL("INSERT INTO characters (name,uid,mobid,max_hp,max_mp,cstr,cint,cdex,ccon,gold,posid,classid) \
            VALUES ('%s', '%s', 31, 40, 30, 8, 9, 13, 6, 1000, %i, 3)", newcharname, thisclient->PlayerSession->username, P[12]);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 0, 1566)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 1, 1569)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 2, 1572)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 3, 1575)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid,add1,addval1) VALUES ('%s', 4, 1578,29,3)", newcharname);
            for (int i=0;i<4;i++)
                DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', '%i', 400)", newcharname, 15+i);
            for (int i=0;i<4;i++)
                DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', '%i', 405)", newcharname, 19+i);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 23, 816)", newcharname);
            DoSQL("INSERT INTO char_items (owner,slotnum,itemid) VALUES ('%s', 24, 923)", newcharname);
            break;
        }
        return ResendCharList( thisclient, P );
    }
    else
    {
        return SendServerMsg( thisclient, "This character name already in use." );
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

    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 1244, this->CKeys, this->Hash1, 0 );
    thisclient->SendPacket( this->encbuf, this->encsize );
    SendServerMsg( thisclient, "Welcome to %s, Powered by Destiny Emulator", this->srvname.c_str() );

    thisclient->PlayerSession->inGame = true;
    thisclient->ready = true;

    packet->Free();
    packet->AddWord( 172, 0 );
    packet->AddByte( 0x64, 4 );
    packet->AddByte( 0x03, 5 );
    packet->AddWord( 30000, 6 );
    // end header

    packet->AddWord( 2177, 12 ); // pos x
    packet->AddWord( 2114, 14 ); // pos y
    packet->AddWord( 1390, 16 );
    for (int i=0;i<12;i++)
        packet->AddByte( 0xcc, 18+i );
    packet->AddStr( "Meio_Orc", 18 ); // char name
    packet->AddWord( 75, 30 ); // chaos point
    packet->AddByte( 208, 34 ); // mob id
    packet->AddWord( 705, 46 );
    packet->AddWord( 2380, 62 );
    packet->AddByte( 0, 66 );//0
    packet->AddByte( 0, 67 );//34
    packet->AddWord( 0, 69 );//player effect
    packet->AddWord( 43, 100 );// Level
    packet->AddWord( 3, 102 );// Defesa
    packet->AddWord( 93, 104 );// Atack
    packet->AddByte( 2, 107 );//84
    //------------------------------------
    packet->AddWord( 800, 108 );//max hp
    packet->AddWord( 0, 110 );//max mana
    packet->AddWord( 200, 112 );//current hp
    packet->AddWord( 0, 114 );//current mana
    //-------------------------------------
    packet->AddWord( 5, 116 );//str
    packet->AddWord( 70, 118 );//int
    packet->AddWord( 30, 120 );//dex
    packet->AddWord( 200, 122 );//con
    packet->AddWord( 2047, 124 );//5
    packet->AddWord( 2047, 126 );//5
    packet->AddWord( 0, 128 );//2
    packet->AddByte( 0, 130 );//43
    for (int i=0;i<29;i++)
    {
        packet->AddByte( 0xcc, 147+i );
    }
    packet->AddByte( 0x00, 172 );

    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 172, this->CKeys, this->Hash1, 0 );
    thisclient->SendPacket( this->encbuf, this->encsize );

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
    if(!DoSQL("SELECT name,cexp,clevel,gold, \
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
        strcpy( chars[posid].char_name, row[0] );
        chars[posid].Exp = atoi(row[1]);
        chars[posid].Level = atoi(row[2]);
        chars[posid].gold = atoi(row[3]);
        chars[posid].Str = atoi(row[4]);
        chars[posid].Int = atoi(row[5]);
        chars[posid].Dex = atoi(row[6]);
        chars[posid].Con = atoi(row[7]);
        chars[posid].Mobid = atoi(row[8]);
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

    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 756, this->CKeys, this->Hash1, 0 );
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
    if(!DoSQL("SELECT name,cexp,clevel,gold, \
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
        strcpy( chars[posid].char_name, row[0] );
        chars[posid].Exp = atoi(row[1]);
        chars[posid].Level = atoi(row[2]);
        chars[posid].gold = atoi(row[3]);
        chars[posid].Str = atoi(row[4]);
        chars[posid].Int = atoi(row[5]);
        chars[posid].Dex = atoi(row[6]);
        chars[posid].Con = atoi(row[7]);
        chars[posid].Mobid = atoi(row[8]);
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

    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 1823, this->CKeys, this->Hash1, 0 );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::CheckLogin( CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    unsigned short cliver;
    unsigned short opcode;
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
            return SendServerMsg( thisclient, "Invalid ID, check your ID." );
        }
    }
    else
    {
        if(!DoSQL( "SELECT username,password,active,online FROM accounts WHERE username='%s' AND password='%s'", thisclient->PlayerSession->username, thisclient->PlayerSession->password ))
            return false;
        result = mysql_store_result( mysql );
        if (mysql_num_rows( result ) == 0) {
            return SendServerMsg( thisclient, "Invalid Password, please check your Password." );
        }
        else
        {
            if(!DoSQL( "SELECT username,password,active,online FROM accounts WHERE username='%s' AND active='1'", thisclient->PlayerSession->username ))
                return false;
            result = mysql_store_result( mysql );
            if (mysql_num_rows( result ) == 0) {
                return SendServerMsg( thisclient, "This ID is not active, please check it on website." );
            }
            else
            {
                if(!DoSQL( "SELECT id,username,password,active,online FROM accounts WHERE username='%s' AND online='0'", thisclient->PlayerSession->username ))
                    return false;
                result = mysql_store_result( mysql );
                if (mysql_num_rows( result ) == 0) {
                    return SendServerMsg( thisclient, "This ID is online, please try other ID." );
                }
                else
                {
                    if(!DoSQL( "SELECT id, acesslevel FROM accounts WHERE username='%s'", thisclient->PlayerSession->username ))
                        return false;
                    result = mysql_store_result( mysql );
                    while ( row = mysql_fetch_row( result ) )
                    {
                        // Assign a new id to this person
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
    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 108, this->CKeys, this->Hash1, 0 );
    thisclient->SendPacket( this->encbuf, this->encsize );
}

bool CConnServer::SpawnChar( CConnClient* thisclient, CConnClient* otherclient )
{
    packet->Free();
    packet->AddWord( 172, 0 );
    packet->AddWord( 868, 4 );
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
    packet->AddWord( 84, 107 );
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

    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 172, this->CKeys, this->Hash1, 0 );
    thisclient->SendPacket( this->encbuf, this->encsize );
}
