#ifndef _ENCDEC_H_
#define _ENCDEC_H_

const int KEYSIZE = 512;

int ReadKeysFromFile( char*filename, unsigned char*Dest );

typedef int (CALLBACK* _PDecrypt)(unsigned char*, unsigned char*, int, unsigned char*);
typedef int (CALLBACK* _PEncrypt)(unsigned char*, unsigned char*, unsigned short, unsigned char*, unsigned long, unsigned long);
typedef unsigned long (CALLBACK* _PGetHash)(unsigned char*, unsigned long);

class CEncDec
{
public:
    CEncDec();
    virtual ~CEncDec();

    // class functions
    bool LoadLib();
    int WYD2_Decrypt( unsigned char* Dest, unsigned char* Src, int Len, unsigned char* ClientKeys );
    int WYD2_Encrypt( unsigned char* Dest, unsigned char* Src, unsigned short Len, unsigned char* EncDecKeys, unsigned long Hash1, unsigned long CurTimeStamp );
    unsigned long WYD2_GetHash1( unsigned char*Keys, unsigned long dwCounter );
};

#endif
