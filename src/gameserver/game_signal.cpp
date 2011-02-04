#include "game_sockets.h"

void StartSignal( )
{
    signal( SIGINT, HandleSignal );
    signal( SIGILL, HandleSignal );
    signal( SIGFPE, HandleSignal );
    signal( SIGSEGV, HandleSignal );
    signal( SIGTERM, HandleSignal );
    signal( SIGBREAK, HandleSignal );
    signal( SIGABRT, HandleSignal );
}

void StopSignal( )
{
    signal( SIGINT, 0 );
    signal( SIGILL, 0 );
    signal( SIGFPE, 0 );
    signal( SIGSEGV, 0 );
    signal( SIGTERM, 0 );
    signal( SIGBREAK, 0 );
    signal( SIGABRT, 0 );
}

void HandleSignal( int num )
{
    switch(num)
    {
        case SIGINT:/* Interactive attention */
            Log( MSG_ERROR, "Signal received: SIGINT" );
    	    #ifdef _WIN32
            Sleep(1000);
    	    #else
    	    usleep(1000);
    	    #endif
            raise(num);
        break;
        case SIGILL:/* Illegal instruction */
            Log( MSG_ERROR, "Signal received: SIGILL, Server will be closed, Trying to save..." );
            GServer->DisconnectAll();
    	    #ifdef _WIN32
                Sleep(1000);
    	    #else
    	    usleep(1000);
    	    #endif
            raise(num);
        break;
        case SIGFPE:/* Floating point error */
            Log( MSG_ERROR, "Signal received: SIGFPE, Server will be closed, Trying to save..." );
            GServer->DisconnectAll();

            raise(num);
        break;
        case SIGSEGV:/* Segmentation violation */
            Log( MSG_ERROR, "Signal received: SIGSEGV, Server will be closed, Trying to save..." );
            GServer->DisconnectAll();
    	    #ifdef _WIN32
                Sleep(1000);
    	    #else
    	    usleep(1000);
    	    #endif
            raise(num);
        break;

        case SIGTERM:/* Termination request */
            Log( MSG_INFO, "Signal received: SIGTERM, Server will be closed" );
            pthread_create( &GServer->WorldThread[SHUTDOWN_THREAD], NULL, ShutdownServer, (PVOID)0);
        break;
    	#ifdef _WIN32
        case SIGBREAK:/* Control-break */
            Log( MSG_INFO, "Signal received: SIGBREAK, Server will be closed" );
            pthread_create( &GServer->WorldThread[SHUTDOWN_THREAD], NULL, ShutdownServer, (PVOID)0);
        break;
    	#endif
        default:
            std::cout << "Unknown Signal Number: " << num << std::endl;
            raise(num);
    }
}
