#include "game_sockets.h"

CEncDec* encdec = new CEncDec();
bufwrite* packet = new bufwrite();

bool CConnServer::ShopItemBuy( CConnClient* thisclient, unsigned char* P )
{
	if(thisclient->PlayerInfo->Gold<66000)
	{
		this->SendServerMsg(thisclient, "You need more gold");
		return true;
	}
	// Start Send Item Price to Client
	packet->Free();
	for (int i=0;i<24;i++)
		packet->AddByte(P[i], i);
	packet->AddWord(30000, 6);
	packet->AddDWord(66000, 20);
	this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 24, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	// End Send Item Price to Client
	// Start Update Gold
	thisclient->PlayerInfo->Gold = thisclient->PlayerInfo->Gold-66000;
	packet->Free();
	packet->AddWord(16, 0);
	packet->AddWord(0x03af, 4);
	packet->AddWord(thisclient->PlayerSession->clientid, 6);
	packet->AddDWord(thisclient->PlayerInfo->Gold, 12);
	this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 16, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	// End Update Gold
	// Start Add Item to Inventory
	unsigned short itemid;
	memcpy(&itemid, &P[12], 2);
	packet->Free();
    packet->AddWord( 24, 0 );
    packet->AddWord( 0x0182, 4 ); //opcode
    packet->AddWord( thisclient->PlayerSession->clientid, 6 ); //clientid
	packet->AddByte( 1, 12 );
    packet->AddByte( 2, 14 ); //slotnum
    packet->AddWord( itemid, 16 ); //itemid
    packet->AddByte( P[14], 18 ); //add1
	packet->AddByte( P[15], 19 ); //val1
	packet->AddByte( P[16], 20 ); //add2
	packet->AddByte( P[17], 21 ); //val2
	packet->AddByte( P[18], 22 ); //add3
	packet->AddByte( P[19], 23 ); //val3
	this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 24, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	// End Add Item to Inventory
	return true;
}

bool CConnServer::PlayerAttack( CConnClient* thisclient, unsigned char* P )
{
    packet->Free();
    for (int i=0;i<96;i++)
        packet->AddByte( P[i], i );
    SendToVisible( encdec, thisclient, packet, 96, false );
    return true;
}

bool CConnServer::ChangeInventory( CConnClient* thisclient, unsigned char* P )
{
    packet->Free();
    for (int i=0;i<21;i++)
        packet->AddByte( P[i], i );
    this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 20, this->CKeys, this->Hash1, this->curtime );
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
    thisclient->PlayerPosition->current.x = dx;
    thisclient->PlayerPosition->current.y = dy;
    thisclient->PlayerPosition->destiny.x = dx;
    thisclient->PlayerPosition->destiny.y = dy;
	return true;
}

bool CConnServer::CharDelete( CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
//	MYSQL_ROW row;
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
//	MYSQL_ROW row;
//	MYSQL_RES *result2;
//	MYSQL_ROW row2;
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

    packet->AddWord( (int)thisclient->PlayerPosition->current.x, 12 );
    packet->AddWord( (int)thisclient->PlayerPosition->current.y, 14 );
    packet->AddStr( thisclient->PlayerInfo->char_name, 16 ); // charname
	packet->AddByte( 150, 28 );
    packet->AddDWord( thisclient->PlayerInfo->Gold, 40 ); // gold
    packet->AddDWord( thisclient->PlayerInfo->Exp, 44 ); // earned experience
    packet->AddWord( (int)thisclient->PlayerPosition->current.x, 48 );
    packet->AddWord( (int)thisclient->PlayerPosition->current.y, 50 );
    packet->AddByte( thisclient->PlayerInfo->classid, 36 ); // classindent

    packet->AddWord( 1536, 38 ); //
    packet->AddByte( 0x20, 52 ); //
    packet->AddByte( 0x00, 53 ); //
    packet->AddByte( 0x24, 54 ); //
    packet->AddByte( 0x00, 55 ); //
    packet->AddByte( 0x05, 56 ); //
    packet->AddByte( 0x00, 57 ); //
    packet->AddByte( 0x00, 58 ); //
    packet->AddByte( 0x52, 59 ); //
    packet->AddByte( 0x78, 60 ); //
    packet->AddByte( 0x00, 61 ); //
    packet->AddByte( 0xCF, 62 ); //
    packet->AddByte( 0x00, 63 ); //
    packet->AddByte( 0x46, 64 ); //
    packet->AddByte( 0x00, 65 ); //
    packet->AddByte( 0x37, 66 ); //
    packet->AddByte( 0x00, 67 ); //
    packet->AddByte( 0x3D, 68 ); //
    packet->AddByte( 0x00, 69 ); //
    packet->AddByte( 0x32, 70 ); //
    packet->AddByte( 0x00, 71 ); //
    packet->AddByte( 0x33, 72 ); //
    packet->AddByte( 0x00, 73 ); //
    packet->AddByte( 0x0E, 74 ); //
    packet->AddByte( 0x00, 75 ); //
    packet->AddByte( 0x0D, 76 ); //
    packet->AddByte( 0x00, 77 ); //
    packet->AddByte( 0x2C, 78 ); //
    packet->AddByte( 0x01, 79 ); //

    packet->AddWord( thisclient->PlayerInfo->Level, 80 ); // lvl
    packet->AddWord( thisclient->PlayerStats->Defense, 82 ); // defense
    packet->AddWord( thisclient->PlayerStats->Attack_Power, 84 ); // ataque


    //packet->AddByte( 0x80, 86 ); //00
    //packet->AddByte( 0xF4, 87 ); // 72
    packet->AddByte( 4, 87 ); //move speed?

    packet->AddWord( thisclient->PlayerStats->MaxHP, 88 ); // hp total
    packet->AddWord( thisclient->PlayerStats->MaxMP, 90 ); // mp total
    packet->AddWord( thisclient->PlayerStats->HP, 92 ); // hp
    packet->AddWord( thisclient->PlayerStats->MP, 94 ); // mp
    packet->AddWord( thisclient->PlayerStats->Str, 96 ); // str
    packet->AddWord( thisclient->PlayerStats->Int, 98 ); // int
    packet->AddWord( thisclient->PlayerStats->Dex, 100 ); // dex
    packet->AddWord( thisclient->PlayerStats->Con, 102 ); // con
    packet->AddByte( 1, 104 ); // skill master1
    packet->AddByte( 2, 105 ); // skill master2
    packet->AddByte( 3, 106 ); // skill master3
    packet->AddByte( 4, 107 ); // skill master4
    packet->AddByte( thisclient->PlayerInfo->mobid, 108 ); // mobile id (if player class id)
    packet->AddByte( 43, 110 ); // Player Add1
    packet->AddByte( 0, 111 );
    packet->AddByte( 86, 112 ); // Player Add2
    packet->AddByte( 0, 113 );
    packet->AddByte( 28, 114 ); // Player Add3
    //packet->AddByte( 0, 115 );

    packet->AddByte( 1, 115 );//

    /* packet->AddByte( 0x23, 740 ); //
    packet->AddByte( 0x02, 741 ); //
    packet->AddByte( 0x96, 742 ); //
    packet->AddByte( 0x00, 743 ); // */

	//Skills?
    packet->AddByte( 0x4D, 744 ); //00
    packet->AddByte( 0x4D, 745 ); //07
    packet->AddByte( 0x4D, 746 ); //4D
    packet->AddByte( 0x00, 747 ); //00
    packet->AddByte( 0x7F, 748 ); //20
    packet->AddByte( 0x7F, 749 ); //7F
    packet->AddByte( 0x7F, 750 ); //00
    packet->AddByte( 0x00, 751 ); //00

    packet->AddWord( 750, 752 ); //stpoints
    packet->AddWord( 400, 754 ); //skmpoints
    packet->AddWord( 290, 756 ); //skpoints

    /* packet->AddByte( 0x48, 758 ); //
    packet->AddByte( 0x0C, 759 ); // */
    packet->AddByte( 0xFF, 760 ); //quick skill slot1
    packet->AddByte( 0xFF, 761 ); //quick skill slot2
    packet->AddByte( 0xFF, 762 ); //quick skill slot3
    packet->AddByte( 0xFF, 763 ); //quick skill slot4
    /* packet->AddByte( 0x00, 764 ); //
    packet->AddByte( 0x02, 765 ); //
    packet->AddByte( 0x32, 766 ); //
    packet->AddByte( 0x32, 767 ); // */

    packet->AddByte( 18, 768 ); // resist4
    packet->AddByte( 18, 769 ); // resist3
    packet->AddByte( 18, 770 ); // resist1
    packet->AddByte( 18, 771 ); // resist2

    //quick slot(aba1)
    packet->AddByte( 0xFF, 778 ); //quick skill slot5
    packet->AddByte( 0xFF, 779 ); //quick skill slot6
    packet->AddByte( 0xFF, 780 ); //quick skill slot7
    packet->AddByte( 0xFF, 781 ); //quick skill slot8
    packet->AddByte( 0xFF, 782 ); //quick skill slot9
    packet->AddByte( 0xFF, 783 ); //quick skill slot10

    //quick slot(aba2)
    packet->AddByte( 0xFF, 784 ); //quick skill slot1
    packet->AddByte( 0xFF, 785 ); //quick skill slot2
    packet->AddByte( 0xFF, 786 ); //quick skill slot3
    packet->AddByte( 0xFF, 787 ); //quick skill slot4
    packet->AddByte( 0xFF, 788 ); //quick skill slot5
    packet->AddByte( 0xFF, 789 ); //quick skill slot6
    packet->AddByte( 0xFF, 790 ); //quick skill slot7
    packet->AddByte( 0xFF, 791 ); //quick skill slot8
    packet->AddByte( 0xFF, 792 ); //quick skill slot9
    packet->AddByte( 0xFF, 793 ); //quick skill slot10

    packet->AddByte( 0xCC, 794 ); //
    packet->AddByte( 0xCC, 795 ); //
    packet->AddByte( 0x22, 828 ); //
    packet->AddByte( 0x00, 829 ); //
    packet->AddByte( 0x06, 830 ); //
    packet->AddByte( 0x00, 831 ); //
    packet->AddByte( 0x51, 832 ); //
    packet->AddByte( 0x4A, 833 ); //
    packet->AddByte( 0x01, 834 ); //
    packet->AddByte( 0x00, 835 ); //


    /*for (int a=0;a<200;a++)
        packet->AddByte( 150,600+a );*/

    //packet->AddWord( 150, 87 );
    packet->AddWord( thisclient->PlayerSession->clientid, 774 ); // ClientID

    // Load Inventory Items
    for (int i=0;i<78;i++)
    {
        packet->AddWord( thisclient->items[i].itemid, (8*i)+116 );//732
        packet->AddByte( thisclient->items[i].add1, (8*i)+118 );//734
        packet->AddByte( thisclient->items[i].val1, (8*i)+119 );//735
        packet->AddByte( thisclient->items[i].add2, (8*i)+120 );//736
        packet->AddByte( thisclient->items[i].val2, (8*i)+121 );//737
        packet->AddByte( thisclient->items[i].add3, (8*i)+122 );//738
        packet->AddByte( thisclient->items[i].val3, (8*i)+123 );//739
    }

    this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 1244, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );

    thisclient->PlayerSession->inGame = true;
    thisclient->ready = true;

    packet->Free();
    packet->AddWord( 172, 0 );
    packet->AddWord( 0x0364, 4 );
    packet->AddWord( 30000, 6 );
    // end header

    packet->AddWord( (int)thisclient->PlayerPosition->current.x, 12 ); // pos x
    packet->AddWord( (int)thisclient->PlayerPosition->current.y, 14 ); // pos y
    packet->AddWord( thisclient->PlayerSession->clientid, 16 );
    for (int i=0;i<12;i++)
        packet->AddByte( 0xcc, 18+i );
    packet->AddStr( thisclient->PlayerInfo->char_name, 18 ); // char name
    packet->AddByte( 150, 30 ); // player karma
    packet->AddByte( thisclient->PlayerInfo->mobid, 34 ); // mob id
    for (int i=0;i<15;i++)
    {
        packet->AddWord( thisclient->items[i].itemid, (2*i)+36 );
    }
    packet->AddWord( thisclient->PlayerSession->clientid, 66 );
    packet->AddWord( 00, 69 );//Player Effect
    packet->AddWord( thisclient->PlayerInfo->Level, 100 );
    packet->AddWord( thisclient->PlayerStats->Attack_Power, 102 );
    packet->AddWord( thisclient->PlayerStats->Defense, 104 );
    packet->AddByte( 0, 106 );
    packet->AddByte( 84, 107 );
    packet->AddWord( thisclient->PlayerStats->MaxHP, 108 );//max hp
    packet->AddWord( thisclient->PlayerStats->MaxMP, 110 );//max mana
    packet->AddWord( thisclient->PlayerStats->HP, 112 );//current hp
    packet->AddWord( thisclient->PlayerStats->MP, 114 );//current mana
    packet->AddWord( thisclient->PlayerStats->Str, 116 );
    packet->AddWord( thisclient->PlayerStats->Int, 118 );
    packet->AddWord( thisclient->PlayerStats->Dex, 120 );
    packet->AddWord( thisclient->PlayerStats->Con, 122 );
    packet->AddWord( 2, 128 );
    packet->AddWord( 0, 130 );

    /* // test sanc?
    packet->AddByte( 43, 130 ); // helm?
    packet->AddByte( 43, 131 ); // armor?
    packet->AddByte( 43, 132 ); // pants?
    packet->AddByte( 43, 133 ); // gloves?
    packet->AddByte( 43, 134 ); // boots?
    packet->AddByte( 43, 135 ); // hand1?
    packet->AddByte( 43, 136 ); // hand2?
    packet->AddByte( 43, 137 ); // accessories?
    packet->AddByte( 0, 138 ); // amuletos?
    packet->AddByte( 43, 139 ); // pedras e rubis?
    packet->AddByte( 0, 140 ); // medalhas?
    packet->AddByte( 0, 141 ); //
    packet->AddByte( 0, 142 ); //pet?
    packet->AddByte( 0, 143 ); //cape?
    packet->AddByte( 0, 144 ); //???
    packet->AddByte( 43, 145 ); //
    // end test */

    for (int i=0;i<29;i++)
    {
        packet->AddByte( 0xcc, 147+i );
    }
    packet->AddByte( 0x00, 172 );

    this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 172, this->CKeys, this->Hash1, this->curtime );
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
    for (unsigned int k=0;k<posid;k++)
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
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 756, this->CKeys, this->Hash1, this->curtime );
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

	//test for load storageitems
	if(!GServer->DoSQL("SELECT slotnum,itemid,add1,add2,add3,addval1,addval2,addval3 FROM storage WHERE owner='%s'", thisclient->PlayerSession->username))
	   return false;
	result = mysql_store_result(GServer->mysql);
	CItem storageitems[126];
	for(unsigned j=0; j<125; j++)
            ClearItem( storageitems[j] );
	while(row = mysql_fetch_row(result))
    {
		UINT itemnum = atoi(row[0]);
		storageitems[itemnum].itemid = atoi(row[1]);
		storageitems[itemnum].add1 = atoi(row[2]);
		storageitems[itemnum].add2 = atoi(row[3]);
		storageitems[itemnum].add3 = atoi(row[4]);
		storageitems[itemnum].val1 = atoi(row[5]);
		storageitems[itemnum].val2 = atoi(row[6]);
		storageitems[itemnum].val3 = atoi(row[7]);
    }
	mysql_free_result(result);
	//end

    unsigned charnum = 0;
    for (unsigned int k=0;k<posid;k++)
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
    // Load Storage Items
    for (int i=0;i<126;i++)
    {
        packet->AddWord( storageitems[i].itemid, (8*i)+756 );//1756
        packet->AddByte( storageitems[i].add1, (8*i)+758 );//1758
        packet->AddByte( storageitems[i].val1, (8*i)+759 );//1759
        packet->AddByte( storageitems[i].add2, (8*i)+760 );//1760
        packet->AddByte( storageitems[i].val2, (8*i)+761 );//1761
        packet->AddByte( storageitems[i].add3, (8*i)+762 );//1762
        packet->AddByte( storageitems[i].val3, (8*i)+763 );//1763
    }

    packet->AddDWord( 1500000000, 1780 );//Storage Gold


    // Client Username
    packet->AddStr( thisclient->PlayerSession->username, 1784 );

    // Char List Encryption Keys
    packet->AddStr( (char*)thisclient->KeysLogin, 1800 );
    packet->AddWord( thisclient->PlayerSession->clientid, 120 );
    packet->AddWord( thisclient->PlayerSession->clientid, 122 );
    packet->AddWord( thisclient->PlayerSession->clientid, 124 );
    packet->AddWord( thisclient->PlayerSession->clientid, 126 );

    this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 1823, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::CheckLogin( CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    unsigned short cliver;
//    unsigned short opcode;
//    time_t rtime;
    if ( P[0] + P[1] == 0x74 )
    {
        encdec->Decrypt( (unsigned char*)buff, (unsigned char*)P, 116, (unsigned char*)this->CKeys );
        memcpy( this->KeysLogin, &buff[48], 16 );
        memcpy( thisclient->PlayerSession->username, &buff[12], 16 );
        memcpy( thisclient->PlayerSession->password, &buff[28], 12 );
        memcpy( &cliver, &buff[40], 2 );
    }
    if ( P[4] + P[5] == 0x74 )
    {
        encdec->Decrypt( (unsigned char*)buff, (unsigned char*)&P[4], 116, (unsigned char*)this->CKeys );
        memcpy( this->KeysLogin, &buff[48], 16 );
        memcpy( thisclient->PlayerSession->username, &buff[12], 16 );
        memcpy( thisclient->PlayerSession->password, &buff[28], 12 );
        memcpy( &cliver, &buff[40], 2 );
    }
    Log( MSG_INFO, "User login with account \"%s\", using cliver \"%i\"", thisclient->PlayerSession->username, cliver );
    this->Hash1 = encdec->GetHash1( this->KeysLogin, thisclient->recvpkts );
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
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 108, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::SpawnChar( CConnClient* thisclient, CConnClient* otherclient )
{
    packet->Free();
    packet->AddWord( 172, 0 );
    packet->AddWord( 0x0364, 4 );
    packet->AddWord( 30000, 6 );
    // end header

    packet->AddWord( (int)otherclient->PlayerPosition->current.x, 12 ); // pos x
    packet->AddWord( (int)otherclient->PlayerPosition->current.y, 14 ); // pos y
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
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 172, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::SendNPCSellItems( CConnClient* thisclient, unsigned char* P )
{
    unsigned short tmpid;
    memcpy( &tmpid, &P[12], 2 );
    CNPC* thisnpc = GetNPCByID(tmpid);
    packet->Free();
    packet->AddWord( 236, 0 );//packet size
    packet->AddWord( 0x017C, 4 );//opcode
    packet->AddWord( 30000, 6 );//???
    switch(thisnpc->mobcode)
    {
    case 1: //npcshop
        packet->AddWord( 1, 12 );
        for (int i=0;i<26;i++)
        {
            packet->AddWord( thisnpc->shopitems[i].itemid, (8*i)+16 );
            packet->AddByte( thisnpc->shopitems[i].add1, (8*i)+18 );
            packet->AddByte( thisnpc->shopitems[i].val1, (8*i)+19 );
            packet->AddByte( thisnpc->shopitems[i].add2, (8*i)+20 );
            packet->AddByte( thisnpc->shopitems[i].val2, (8*i)+21 );
            packet->AddByte( thisnpc->shopitems[i].add3, (8*i)+22 );
            packet->AddByte( thisnpc->shopitems[i].val3, (8*i)+23 );
        }
    break;
    case 19: //npcskill
        packet->AddWord( 3, 12 );
        for (int i=0;i<26;i++)
        {
            packet->AddWord( thisnpc->shopitems[i].itemid, (8*i)+16 );
            packet->AddByte( thisnpc->shopitems[i].add1, (8*i)+18 );
            packet->AddByte( thisnpc->shopitems[i].val1, (8*i)+19 );
            packet->AddByte( thisnpc->shopitems[i].add2, (8*i)+20 );
            packet->AddByte( thisnpc->shopitems[i].val2, (8*i)+21 );
            packet->AddByte( thisnpc->shopitems[i].add3, (8*i)+22 );
            packet->AddByte( thisnpc->shopitems[i].val3, (8*i)+23 );
        }
    break;
    }
    packet->AddByte( 10, 232 );
    this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 236, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
    return true;
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
    packet->AddWord( thisnpc->mobid, 34 ); // mob id
    for (int i=0;i<15;i++)
    {
        packet->AddWord( thisnpc->inventory[i].itemid, (2*i)+36 );
    }
    packet->AddByte( 0, 66 );
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
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 172, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;
}

bool CConnServer::GameToCharList( CConnClient* thisclient, unsigned char* P )
{

    packet->AddWord( 12, 0 ); // packet size
    packet->AddWord( 0x0116, 4 ); // packet id
    packet->AddWord( thisclient->PlayerSession->clientid, 6 ); // Player ID

    this->curtime = clock();
    this->encsize = encdec->Encrypt( this->encbuf, packet->buff(), 12, this->CKeys, this->Hash1, this->curtime );
    thisclient->SendPacket( this->encbuf, this->encsize );
	return true;

}
