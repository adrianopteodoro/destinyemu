
#ifndef __WYDEMU_CONFIG_FUNCTIONS__
#define __WYDEMU_CONFIG_FUNCTIONS__

// -----------------------------------------------------------------------------------------
// Config load functions
// -----------------------------------------------------------------------------------------
char *ConfigGetString( char *pcFile, char *pcName, char *pcDefault );
unsigned ConfigGetInt( char *pcFile, char *pcName, unsigned uDefault );

// -----------------------------------------------------------------------------------------

#endif

