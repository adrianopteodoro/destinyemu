#ifndef _ENCDEC_H_
#define _ENCDEC_H_
#include <stdio.h>
#include <windows.h>

const int KEYSIZE = 512;

int ReadKeysFromFile( char*filename, unsigned char*Dest );
typedef int (CALLBACK* PtrDecrypt)(unsigned char*, unsigned char*, int, unsigned char*);
typedef int (CALLBACK* PtrEncrypt)(unsigned char*, unsigned char*, unsigned short, unsigned char*, unsigned long, unsigned long);
typedef unsigned long (CALLBACK* PtrGetHash)(unsigned char*, unsigned long);

class CEncDec
{
public:
    CEncDec();
    virtual ~CEncDec();

	HINSTANCE hENCDEC;
	PtrDecrypt WYD2_Decrypt;
	PtrEncrypt WYD2_Encrypt;
	PtrGetHash WYD2_GetHash1;

    // class functions
    int Decrypt( unsigned char* Dest, unsigned char* Src, int Len, unsigned char* ClientKeys );
    int Encrypt( unsigned char* Dest, unsigned char* Src, unsigned short Len, unsigned char* EncDecKeys, unsigned long Hash1, time_t CurTimeStamp );
    unsigned long GetHash1( unsigned char*Keys, unsigned long dwCounter );
};

#endif
