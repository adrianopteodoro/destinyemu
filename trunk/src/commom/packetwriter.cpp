#include "packetwriter.h"
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>
#include <stdio.h>

using std::string;
using std::cout;

// Class Constructor
bufwrite::bufwrite()
{
    S=0;
}

bufwrite::bufwrite(uchar d[], int s)
{
    S=s;
    for(int a=0;a<=s;a++)
    D[a]=d[a];
    D[s+1]=0x00;
}

bufwrite::~bufwrite()
{
}

int bufwrite::psize()
{
   return S;
}

unsigned char* bufwrite::buff()
{
    return D;
}

void bufwrite::SetPSize(int i)
{
    S=i;
}

void bufwrite::AddByte(uchar h, unsigned int offset)
{
     D[offset]=h;
     S++;
}

void bufwrite::AddStr(char* co, unsigned int offset)
{
	memcpy( &D[offset], &co, strlen(co));
}

void bufwrite::AddWord( unsigned short h, unsigned int offset )
{
     memcpy( &D[offset], (int*)&h, 2 );
}

void bufwrite::AddData( unsigned char* h, unsigned int offset, int size )
{
     memcpy( &D[offset], &h, size );
}

int bufwrite::GetWord( unsigned char* src, unsigned int offset )
{
     int value;
     memcpy( &value, &D[offset], 2 );
     return value;
}

void bufwrite::AddDWord( unsigned long h, unsigned int offset )
{
     memcpy( &D[offset], (int*)&h, 4 );
}

void bufwrite::Free( )
{
    ZeroMemory( &D, 65355 );
}
