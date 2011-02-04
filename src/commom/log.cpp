//// -----------------------------------------------------------------------------------------
// Inclusions
// -----------------------------------------------------------------------------------------
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "log.h"

// -----------------------------------------------------------------------------------------
// Basic colors
// -----------------------------------------------------------------------------------------
typedef enum
{
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
} COLORS;

// -----------------------------------------------------------------------------------------
// Our base colors
// -----------------------------------------------------------------------------------------
static int __BACKGROUND = BLACK;
static int __FOREGROUND = LIGHTGRAY;

// -----------------------------------------------------------------------------------------
// Change console text color
// -----------------------------------------------------------------------------------------
void textcolor(int color)
{
    __FOREGROUND = color;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color + (__BACKGROUND << 4));
}

// -----------------------------------------------------------------------------------------
// This function logs based on flags
// -----------------------------------------------------------------------------------------
void Log( enum msg_type flag, char *Format, ... )
{
	va_list ap;	      // For arguments
	va_start( ap, Format );

	// Timestamp
    time_t rtime;
    time(&rtime);
    char *timestamp = ctime(&rtime);
    timestamp[ strlen(timestamp)-1 ] = ' ';

    if(flag!=MSG_SQL)
    {
        textcolor(WHITE);
        printf("%s", timestamp );
    }

	switch (flag) {
		case MSG_NONE: // direct printf replacement
			textcolor(WHITE);
			vprintf( Format, ap );
			break;
		case MSG_STATUS:
			textcolor(GREEN);
			printf("[STATUS]: ");
			break;
		case MSG_SQL:
			textcolor(LIGHTMAGENTA);
			printf("[SQL]: ");
			break;
		case MSG_INFO:
			textcolor(LIGHTGREEN);
			printf("[INFO]: ");
			break;
		case MSG_NOTICE:
			textcolor(YELLOW);
			printf("[NOTICE]: ");
			break;
		case MSG_WARNING:
			textcolor(LIGHTCYAN);
			printf("[WARNING]: ");
			break;
		case MSG_DEBUG:
			textcolor(LIGHTBLUE);
			printf("[DEBUG]: ");
			break;
		case MSG_ERROR:
			textcolor(LIGHTRED);
			printf("[ERROR]: ");
			break;
		case MSG_FATALERROR:
			textcolor(LIGHTRED);
			printf("[FATAL ERROR]: ");
			break;
		case MSG_HACK:
			textcolor(LIGHTRED);
			printf("[HACK]: ");
			break;
		case MSG_LOAD:
			textcolor(LIGHTRED);
			printf("[LOADING]: ");
			break;
        case MSG_GMACTION:
			textcolor(MAGENTA);
			printf("[GM ACTION]: ");
			break;
	}

	textcolor(LIGHTGRAY);
	vprintf( Format, ap );
	printf( (flag==MSG_LOAD) ? "\r" : "\n" );

	va_end  ( ap );
}

void ShowHeader()
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), LIGHTCYAN + (BLUE << 4));
    printf("#########################################################################\n");
    printf("#\t\t ____              _    _               \t\t#\n");
    printf("#\t\t|  _ \\   ___  ___ | |_ (_) _ __   _   _ \t\t#\n");
    printf("#\t\t| | | | / _ \\/ __|| __|| || '_ \\ | | | |\t\t#\n");
    printf("#\t\t| |_| ||  __/\\__ \\| |_ | || | | || |_| |\t\t#\n");
    printf("#\t\t|____/  \\___||___/ \\__||_||_| |_| \\__, |\t\t#\n");
    printf("#\t\t                                  |___/ \t\t#\n");
    printf("#\t\tHome Page: http://destinyemu.sf.net\t\t\t#\n");
    printf("#########################################################################\n");
    textcolor(LIGHTGRAY);
    printf("\t\n\n");
}

// -----------------------------------------------------------------------------------------


