#include <stdio.h>
#include <windows.h>
#include "encdec.h"
#include "log.h"

_PDecrypt PDecrypt = NULL;
_PEncrypt PEncrypt = NULL;
_PGetHash PGetHash = NULL;

#if _MSC_VER
#	define CSTR LPCSTR
#	define CWSTR LPCWSTR 
#else
#	define CSTR char*
#	define CWSTR char*
#endif

CEncDec::CEncDec()
{
}

CEncDec::~CEncDec()
{
}

bool CEncDec::LoadLib()
{
    HMODULE hDLL = LoadLibrary( (CWSTR)".\\libencdec.dll" );
    PDecrypt = (_PDecrypt)GetProcAddress( hDLL, (CSTR)"WYD2_Decrypt" );
    PEncrypt = (_PEncrypt)GetProcAddress( hDLL, (CSTR)"WYD2_Encrypt" );
    PGetHash = (_PGetHash)GetProcAddress( hDLL, (CSTR)"WYD2_GetHash1" );
    if (!PDecrypt){
      // handle the error
      FreeLibrary( hDLL );
      return false;
	}
    Log( MSG_INFO, "Loaded Enc\\Dec Functions" );
    return true;
}

int CEncDec::WYD2_Decrypt( unsigned char* Dest, unsigned char* Src, int Len, unsigned char* ClientKeys )
{
    return PDecrypt( Dest, Src, Len, ClientKeys );
}

int CEncDec::WYD2_Encrypt( unsigned char* Dest, unsigned char* Src, unsigned short Len, unsigned char* EncDecKeys, unsigned long Hash1, time_t CurTimeStamp )
{
    return PEncrypt( Dest, Src, Len, EncDecKeys, Hash1, (unsigned long)CurTimeStamp );
}

unsigned long CEncDec::WYD2_GetHash1( unsigned char*Keys, unsigned long dwCounter )
{
    return PGetHash( Keys, dwCounter );
}

// Read Client Keys File Function
int ReadKeysFromFile( char*filename, unsigned char*Dest )
{
	unsigned char Buff[KEYSIZE];

	HANDLE hFile=CreateFileA(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if((!hFile) || (GetFileSize(hFile,0)!=KEYSIZE)) {
			return 1;
	}
	unsigned long bytesRead;

	if (SetFilePointer(hFile,0,NULL,FILE_BEGIN)==0xFFFFFFFF) {
		CloseHandle(hFile);
		return 2;
	}

	if(!ReadFile(hFile,Buff,KEYSIZE,&bytesRead,0)) {
		CloseHandle(hFile);
		return 3;
	}
	//save keys to dest
	for(int i=0;i<KEYSIZE;i++) Dest[i]=Buff[i];

	if(!CloseHandle(hFile)){
		return 4;
	}

    return 0;
}
