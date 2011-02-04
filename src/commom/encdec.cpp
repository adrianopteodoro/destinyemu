#include "encdec.h"
#include "log.h"

CEncDec::CEncDec()
{
	hENCDEC = LoadLibrary(L".\\encdec.dll");
	if(hENCDEC == NULL) Log(MSG_ERROR, "Failed to load Library encdec.dll");
	WYD2_Decrypt = (PtrDecrypt)GetProcAddress(HMODULE(hENCDEC), "WYD2_Decrypt");
	if(WYD2_Decrypt == NULL) Log(MSG_ERROR, "Failed to load Decrypt Function");
	WYD2_Encrypt = (PtrEncrypt)GetProcAddress(HMODULE(hENCDEC), "WYD2_Encrypt");
	if(WYD2_Decrypt == NULL) Log(MSG_ERROR, "Failed to load Encrypt Function");
	WYD2_GetHash1 = (PtrGetHash)GetProcAddress(HMODULE(hENCDEC), "WYD2_GetHash1");
	if(WYD2_Decrypt == NULL) Log(MSG_ERROR, "Failed to load Hash Function");
}

CEncDec::~CEncDec()
{
	FreeLibrary(hENCDEC);
}

int CEncDec::Decrypt( unsigned char* Dest, unsigned char* Src, int Len, unsigned char* ClientKeys )
{
    return WYD2_Decrypt( Dest, Src, Len, ClientKeys );
}

int CEncDec::Encrypt( unsigned char* Dest, unsigned char* Src, unsigned short Len, unsigned char* EncDecKeys, unsigned long Hash1, time_t CurTimeStamp )
{
    return WYD2_Encrypt( Dest, Src, Len, EncDecKeys, Hash1, (unsigned long)CurTimeStamp );
}

unsigned long CEncDec::GetHash1( unsigned char*Keys, unsigned long dwCounter )
{
    return WYD2_GetHash1( Keys, dwCounter );
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
