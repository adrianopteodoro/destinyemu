#ifndef __WYDEMU_PACKETWRITER__
#define __WYDEMU_PACKETWRITER__

#include <string>

typedef unsigned char uchar;

class bufwrite
{
    protected:
        unsigned int S;          // size of packet
        uchar D[65355];   // packet data
    public:
        bufwrite();
        bufwrite(uchar d[], int s);
        ~bufwrite();
        int psize();
        unsigned char* buff();
        void SetPSize(int i);
        void AddByte(uchar h, unsigned int offset);
        void AddChar(uchar h, unsigned int ile, unsigned int offset);
        void AddStr(std::string co, unsigned int gdzie);
        void AddWord( unsigned short h, unsigned int offset );
        void AddDWord( unsigned long h, unsigned int offset );
        int GetWord( unsigned char* src, unsigned int offset );
        void Free( );
};

#endif
