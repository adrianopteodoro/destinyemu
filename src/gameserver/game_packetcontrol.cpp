#include "game_sockets.h"

bool CConnServer::PacketControl( CConnClient* thisclient, int size, unsigned char* P )
{
    unsigned short opcode;
    unsigned short cid;
    memcpy( &opcode, &P[4], 2 );
    memcpy( &cid, &P[6], 2 );

    switch ( opcode )
    {
		case 0x020d: return CheckLogin( thisclient, P ); 
        case 0x020f: return CharCreate( thisclient, P );
        case 0x0211: return CharDelete( thisclient, P );
        case 0x0213: return SendToWorld( thisclient, P );
        case 0x0366: return ObjectMove( thisclient, P );
        case 0x0369: return true; // Unknow
        case 0x0333: return SendChat( thisclient, P );
        case 0x0334: return packetCommand( thisclient, P );
        case 0x0290: return pakGMTele( thisclient, P, 1047, 1725 ); // teleport
        case 0x0376: return ChangeInventory( thisclient, P );
        case 0x03a0: return true; // Save Request
        case 0x027b: return SendNPCSellItems( thisclient, P ); // NPC Shop Items
        case 0x039d: return PlayerAttack( thisclient, P ); // Player Attack
        case 0x0215: return GameToCharList( thisclient, P ); // gametocharlist
		case 0x0379: return ShopItemBuy( thisclient, P ); // Shop Item Buy
		case 0x4388: return true; // to remove the msg from console.
        default:
            Log( MSG_WARNING, "Received Unknow OPCODE: 0x%04x", opcode );
			FILE *fh = NULL;
			char* file = new char[60];
			sprintf_s(file, 60, ".\\logs\\packet_%04x.bin", opcode);
			fh = fopen( file, "wb" );
			if ( fh != NULL )
			{
				fwrite(P, 1, size, fh);
				fclose( fh );
			}
        break;
    }
	return true;
}
