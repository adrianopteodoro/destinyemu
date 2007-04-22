#include "game_sockets.h"

bufwrite* packet = new bufwrite();

bool CConnServer::ObjectMove( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    int spos_x;
    int spos_y;
    int epos_x;
    int epos_y;
    int unknow1;
    int unknow2;
    int unknow3;
    int unknow4;
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, 52, (unsigned char*)this->CKeys );

    memcpy( &spos_x, &buff[12], 2 );
    memcpy( &spos_y, &buff[14], 2 );
    memcpy( &unknow1, &buff[16], 1 );
    memcpy( &epos_x, &buff[24], 2 );
    memcpy( &epos_y, &buff[26], 2 );
    memcpy( &unknow2, &buff[28], 2 );
    memcpy( &unknow3, &buff[30], 2 );
    memcpy( &unknow4, &buff[32], 2 );


    packet->Free();
    packet->AddWord( 172, 0 );
    packet->AddWord( 868, 4 );
    // end header

    packet->AddWord( spos_x, 12 ); // pos x
    packet->AddWord( spos_y, 14 ); // pos y
    packet->AddWord( 61, 16 );
    packet->AddStr( thisclient->PlayerInfo->char_name, 18 ); // char name
    packet->AddByte( 150, 30 ); // player karma
    packet->AddByte( thisclient->PlayerInfo->mobid, 34 ); // mob id
    packet->AddWord( 1254, 36 ); //helm?
    packet->AddWord( 5362, 38 ); // armor?
    packet->AddByte( 34, 67 );
    packet->AddByte( 05, 68 );
    packet->AddByte( 0x09, 69 );
    packet->AddWord( 28, 100 );
    packet->AddWord( 95, 102 );
    packet->AddWord( 104, 104 );
    packet->AddWord( 106, 106 );
    packet->AddWord( 108, 108 );
    packet->AddWord( 110, 110 );
    packet->AddWord( 112, 112 );
    packet->AddWord( 114, 114 );
    packet->AddWord( 116, 116 );
    packet->AddWord( 118, 118 );
    packet->AddWord( 120, 120 );
    packet->AddWord( 122, 122 );
    packet->SetPSize( 172 );

    SendToVisible( encdec, thisclient, packet, false );
	return true;
}

bool CConnServer::CharDelete( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
    char delcharname[15];
    char delpassword[12];
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, 44, (unsigned char*)this->CKeys );
    memcpy( delcharname, &buff[16], 15 );
    memcpy( delpassword, &buff[32], 12 );

    if(!DoSQL( "SELECT * FROM accounts WHERE username='%s' AND password='%s'", thisclient->PlayerSession->username, delpassword ))
        return false;
    result = mysql_store_result( mysql );
    if (mysql_num_rows( result ) == 1)
    {
        DoSQL("DELETE FROM characters WHERE name='%s'", delcharname);
        DoSQL("DELETE FROM char_items WHERE owner='%s'", delcharname);
        return ResendCharList( encdec, thisclient, P );
    }
    else
    {
        return SendServerMsg( encdec, thisclient, "Invalid password, try again." );
    }
}

bool CConnServer::CharCreate( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
    char newcharname[15];
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, 36, (unsigned char*)this->CKeys );
    memcpy( newcharname, &buff[16], 15 );
    if(!DoSQL( "SELECT name FROM characters WHERE name='%s'", newcharname ))
        return false;
    result = mysql_store_result( mysql );
    if (mysql_num_rows( result ) == 0)
    {
        switch (buff[32])
        {
            //transknight
            case 0:
            DoSQL("INSERT INTO characters (name,uid,mobid,max_hp,max_mp,cstr,cint,cdex,ccon,gold,posid,classid) \
            VALUES ('%s', '%s', 1, 50, 10, 8, 4, 7, 6, 1000, %i, 0)", newcharname, thisclient->PlayerSession->username, buff[12]);
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
            VALUES ('%s', '%s', 11, 40, 30, 5, 8, 5, 5, 1000, %i, 1)", newcharname, thisclient->PlayerSession->username, buff[12]);
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
            VALUES ('%s', '%s', 21, 40, 30, 6, 6, 9, 5, 1000, %i, 2)", newcharname, thisclient->PlayerSession->username, buff[12]);
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
            VALUES ('%s', '%s', 31, 40, 30, 8, 9, 13, 6, 1000, %i, 3)", newcharname, thisclient->PlayerSession->username, buff[12]);
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
        return ResendCharList( encdec, thisclient, P );
    }
    else
    {
        return SendServerMsg( encdec, thisclient, "This character name already in use." );
    }
}

bool CConnServer::SendToWorld( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, 36, (unsigned char*)this->CKeys );
    MYSQL_RES *result;
	MYSQL_ROW row;
    packet->Free();

    if(!DoSQL( "SELECT name FROM characters WHERE uid='%s' AND posid='%i'", thisclient->PlayerSession->username, buff[12] ))
        return false;
    result = mysql_store_result( mysql );

    packet->AddWord( 1244, 0 ); // packet size
    packet->AddWord( 276, 4 ); // packet id
    packet->AddWord( thisclient->PlayerSession->userid, 6 ); //Player ID

    while ( row = mysql_fetch_row( result ) )
    {
        Log( MSG_INFO, "Account \"%s\" has selected the character \"%s\"", thisclient->PlayerSession->username, row[0] );
        strcpy( thisclient->PlayerInfo->char_name, row[0] );
    }
    // Load Char data
    thisclient->loaddata();

    packet->AddWord( thisclient->PlayerPosition->Pos_x, 12 );
    packet->AddWord( thisclient->PlayerPosition->Pos_y, 14 );
    packet->AddStr( thisclient->PlayerInfo->char_name, 16 ); // charname
    packet->AddDWord( thisclient->PlayerInfo->Gold, 40 ); // gold
    packet->AddDWord( thisclient->PlayerInfo->Exp, 44 ); // earned experience
    packet->AddWord( thisclient->PlayerPosition->Pos_x, 48 );
    packet->AddWord( thisclient->PlayerPosition->Pos_y, 50 );
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
    packet->AddByte( 0, 107 ); // aprendiz
    packet->AddByte( 0, 104 ); // aprendiz
    packet->AddByte( 0, 105 ); // aprendiz
    packet->AddByte( 0, 106 ); // aprendiz
    packet->AddByte( thisclient->PlayerInfo->mobid, 108 ); // mobile id (if player class id)
    packet->AddByte( 43, 110 ); // Player Add1
    packet->AddByte( 30, 111 );
    packet->AddByte( 86, 112 ); // Player Add2
    packet->AddByte( 30, 113 );
    packet->AddByte( 28, 114 ); // Player Add3
    packet->AddByte( 30, 115 );
    packet->AddByte( 97, 742 ); // Player Karma
    packet->AddByte( 0x2b, 774 ); // Char Move


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

    SendServerMsg( encdec, thisclient, "Welcome to %s, Powered by Destiny Emulator", this->srvname.c_str() );

	return true;
}

bool CConnServer::ResendCharList( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_RES *result2;
	MYSQL_ROW row2;
    bufwrite* packet = new bufwrite();
    packet->Free();

    // Packet Header
    packet->AddWord( 1824, 0 ); // packet size
    packet->AddWord( 274, 4 ); // packet id
    packet->AddWord( thisclient->PlayerSession->userid, 6 ); //Player ID
    packet->AddWord( thisclient->PlayerSession->userid, 124 );

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
        packet->AddByte( chars[k].Level, (28*charpos)+92 ); // char level
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

bool CConnServer::SendCharList( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
	MYSQL_RES *result2;
	MYSQL_ROW row2;
    bufwrite* packet = new bufwrite();
    packet->Free();

    // Packet Header
    packet->AddWord( 1824, 0 ); // packet size
    packet->AddWord( 270, 4 ); // packet id
    packet->AddWord( thisclient->PlayerSession->userid, 6 ); //Player ID
    packet->AddWord( thisclient->PlayerSession->userid, 124 );

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
        packet->AddByte( chars[k].Level, (28*charpos)+92 ); // char level
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

    this->encsize = encdec->WYD2_Encrypt( this->encbuf, packet->buff(), 1823, this->CKeys, this->Hash1, 0 );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::CheckLogin( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    if ( P[0] + P[1] == 0x74 )
    {
        encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, 116, (unsigned char*)this->CKeys );
        memcpy( this->KeysLogin, &buff[48], 16 );
        memcpy( thisclient->PlayerSession->username, &buff[12], 16 );
        memcpy( thisclient->PlayerSession->password, &buff[28], 12 );
    }
    if ( P[4] + P[5] == 0x74 )
    {
        encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)&P[4], 116, (unsigned char*)this->CKeys );
        memcpy( this->KeysLogin, &buff[48], 16 );
        memcpy( thisclient->PlayerSession->username, &buff[12], 16 );
        memcpy( thisclient->PlayerSession->password, &buff[28], 12 );
    }
    Log( MSG_INFO, "User login with account \"%s\"", thisclient->PlayerSession->username );
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
            return SendCharList( (CEncDec*)encdec, (CConnClient*)thisclient, P );
        }
        else
        {
            return SendServerMsg( encdec, thisclient, "Invalid ID, check your ID." );
        }
    }
    else
    {
        if(!DoSQL( "SELECT username,password,active,online FROM accounts WHERE username='%s' AND password='%s'", thisclient->PlayerSession->username, thisclient->PlayerSession->password ))
            return false;
        result = mysql_store_result( mysql );
        if (mysql_num_rows( result ) == 0) {
            return SendServerMsg( encdec, thisclient, "Invalid Password, please check your Password." );
        }
        else
        {
            if(!DoSQL( "SELECT username,password,active,online FROM accounts WHERE username='%s' AND active='1'", thisclient->PlayerSession->username ))
                return false;
            result = mysql_store_result( mysql );
            if (mysql_num_rows( result ) == 0) {
                return SendServerMsg( encdec, thisclient, "This ID is not active, please check it on website." );
            }
            else
            {
                if(!DoSQL( "SELECT id,username,password,active,online FROM accounts WHERE username='%s' AND online='0'", thisclient->PlayerSession->username ))
                    return false;
                result = mysql_store_result( mysql );
                if (mysql_num_rows( result ) == 0) {
                    return SendServerMsg( encdec, thisclient, "This ID is online, please try other ID." );
                }
                else
                {
                    if(!DoSQL( "SELECT * FROM accounts WHERE username='%s'", thisclient->PlayerSession->username ))
                        return false;
                    result = mysql_store_result( mysql );
                    while ( row = mysql_fetch_row( result ) )
                    {
                        thisclient->PlayerSession->userid = 40000 + atoi(row[0]);
                        thisclient->PlayerSession->accesslevel = atoi(row[6]);
                    }
                    return SendCharList( (CEncDec*)encdec, (CConnClient*)thisclient, P );
                }
            }
        }
    }
	return true;
}

bool CConnServer::SendServerMsg( CEncDec* encdec, CConnClient* thisclient ,char* Format, ...)
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
