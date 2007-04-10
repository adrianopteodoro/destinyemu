#ifndef __WYDEMU_PACKETWRITER__
#define __WYDEMU_PACKETWRITER__

#include <string>

typedef unsigned char uchar;

class bufwrite
{
    protected:
        int S;          // size of packet
        uchar D[65355];   // packet data
    public:
        bufwrite();
        bufwrite(uchar d[], int s);
        ~bufwrite();
        int psize();
        unsigned char* buff();
        void SetPSize(int i);
        void AddByte(uchar h, int offset);
        void AddChar(uchar h, int ile, int offset);
        void AddStr(std::string co, int gdzie);
        void AddWord( unsigned short h, int offset );
        void AddDWord( unsigned long h, int offset );
        void Free( );
};

#endif
