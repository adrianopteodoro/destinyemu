#ifndef __WYDEMU_LOGGING_FUNCTIONS__
#define __WYDEMU_LOGGING_FUNCTIONS__

// -----------------------------------------------------------------------------------------
// Error Types
// -----------------------------------------------------------------------------------------
enum msg_type {
	MSG_NONE,
	MSG_STATUS,
	MSG_SQL,
	MSG_INFO,
	MSG_NOTICE,
	MSG_WARNING,
	MSG_DEBUG,
	MSG_ERROR,
	MSG_FATALERROR,
	MSG_HACK,
	MSG_LOAD,
	MSG_SDEBUG,
	MSG_GMACTION
};

// -----------------------------------------------------------------------------------------
// Logging function
// -----------------------------------------------------------------------------------------
void Log( msg_type flag, char* Format, ... );
void textcolor(int color);
void ShowHeader();

// -----------------------------------------------------------------------------------------

#endif
