// -----------------------------------------------------------------------------------------
// Inclusions
// -----------------------------------------------------------------------------------------
#include "sockets.h"

// -----------------------------------------------------------------------------------------
// Base Socket Constructor
// -----------------------------------------------------------------------------------------
CBaseSocket::CBaseSocket( )
{
	sock = INVALID_SOCKET;
	isActive = false;
}

// -----------------------------------------------------------------------------------------
// Base Socket Destructor
// -----------------------------------------------------------------------------------------
CBaseSocket::~CBaseSocket( )
{
}
