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
	return true;
}

bool CConnServer::CharDelete( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
}

bool CConnServer::CharCreate( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
}

bool CConnServer::CharProcess( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    unsigned char* buff = (unsigned char*)malloc(thisclient->PSize);
    encdec->WYD2_Decrypt( (unsigned char*)buff, (unsigned char*)P, 36, (unsigned char*)this->CKeys );

    if ( buff[4] == 0x13 && buff[5] == 0x02 )
    {
         return SendToWorld( encdec, thisclient, P );
    }

    if ( buff[4] == 0x0f && buff[5] == 0x02 )
    {
         return CharCreate( encdec, thisclient, P );
    }

    return true;
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
    packet->AddByte( 0, 111 );
    packet->AddByte( 86, 112 ); // Player Add2
    packet->AddByte( 0, 113 );
    packet->AddByte( 28, 114 ); // Player Add3
    packet->AddByte( 0, 115 );
    // char move
    packet->AddByte( 0x2b, 774 );

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
	return true;
}

bool CConnServer::SendCharList( CEncDec* encdec, CConnClient* thisclient, unsigned char* P )
{
    MYSQL_RES *result;
	MYSQL_ROW row;
	if(!DoSQL( "SELECT name FROM characters WHERE uid='%s'", thisclient->PlayerSession->username ))
        return false;
    result = mysql_store_result( mysql );
    bufwrite* packet = new bufwrite();
    packet->Free();

    // Packet Header
    packet->AddWord( 1824, 0 ); // packet size
    packet->AddWord( 270, 4 ); // packet id
    packet->AddWord( thisclient->PlayerSession->userid, 6 ); //Player ID

    // Load Char List
    thisclient->loadcharlist();

    // Char List
    int charid = 0;
    while (row = mysql_fetch_row( result ))
    {
            packet->AddWord( thisclient->PlayerSession->userid, 124 );
            packet->AddByte( 0x41, (2*charid)+12 );
            packet->AddByte( 0x08, (2*charid)+13 );
            packet->AddByte( 0x30, (2*charid)+20 );
            packet->AddByte( 0x08, (2*charid)+21 );
            packet->AddByte( thisclient->chars[charid].Level, (28*charid)+92 );
            packet->AddStr( thisclient->chars[charid].char_name, (16*charid)+28 ); // char name
            packet->AddByte( thisclient->chars[charid].Mobid, (128*charid)+204 );
            packet->AddWord( thisclient->chars[charid].Str, (28*charid)+108 ); // char str
            packet->AddWord( thisclient->chars[charid].Int, (28*charid)+110 ); // char int
            packet->AddWord( thisclient->chars[charid].Dex, (28*charid)+112 ); // char dex
            packet->AddWord( thisclient->chars[charid].Con, (28*charid)+114 ); // char con
            packet->AddDWord( thisclient->chars[charid].gold, (4*charid)+724 ); // char gold
            packet->AddDWord( thisclient->chars[charid].Exp, (4*charid)+740 ); // char exp

            // Increment CharId
            charid++;
    }

    int charnum = 0;
    for (int k=0;k<charid;k++)
    {
        for (int i=0;i<15;i++)
        {
            packet->AddWord( thisclient->chars[charnum].eqitems[i].itemid, (8*i)+(128*k)+212 );
            packet->AddByte( thisclient->chars[charnum].eqitems[i].add1, (8*i)+(128*k)+214 );
            packet->AddByte( thisclient->chars[charnum].eqitems[i].val1, (8*i)+(128*k)+215 );
            packet->AddByte( thisclient->chars[charnum].eqitems[i].add2, (8*i)+(128*k)+216 );
            packet->AddByte( thisclient->chars[charnum].eqitems[i].val2, (8*i)+(128*k)+217 );
            packet->AddByte( thisclient->chars[charnum].eqitems[i].add3, (8*i)+(128*k)+218 );
            packet->AddByte( thisclient->chars[charnum].eqitems[i].val3, (8*i)+(128*k)+219 );
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
        if ( this->autocreateacc.c_str() == "true" )
        {
            if(!DoSQL( "insert into `accounts` (`username`, `password`, `active`) values ('%s', '%s', '1')", thisclient->PlayerSession->username, thisclient->PlayerSession->password )
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
