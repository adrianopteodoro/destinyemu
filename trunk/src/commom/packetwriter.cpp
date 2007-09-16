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

void bufwrite::AddChar(uchar h, unsigned int ile, unsigned int offset)
{
     if(S<ile+offset)S=ile+offset;
     for (unsigned int i =0;i<=ile;i++)
     D[i+offset]=h;
}

void bufwrite::AddStr(std::string co, unsigned int gdzie)
{
    if(gdzie+co.size()>S)S=gdzie+(unsigned int)co.size();
    for (unsigned int i =0;i<=co.size();i++)
    D[i+gdzie]=co[i];
}

void bufwrite::AddWord( unsigned short h, unsigned int offset )
{
     memcpy( (char*)&D[offset], (int*)&h, 2 );
}

int bufwrite::GetWord( unsigned char* src, unsigned int offset )
{
     int value;
     memcpy( &value, &D[offset], 2 );
     return value;
}

void bufwrite::AddDWord( unsigned long h, unsigned int offset )
{
     memcpy( (char*)&D[offset], (int*)&h, 4 );
}

void bufwrite::Free( )
{
    ZeroMemory( &D, 65355 );
}
