#include "game_sockets.h"

// Map Process
PVOID MapProcess( PVOID TS )
{
    while(GServer->ServerOnline)
    {
        pthread_mutex_lock( &PlayerMutex );
        // Player
        pthread_mutex_unlock( &PlayerMutex );
        pthread_mutex_lock( &MonsterMutex );
        // Monster
        pthread_mutex_unlock( &MonsterMutex );
        #ifdef _WIN32
        Sleep(10);
        #else
        usleep(10);
        #endif
    }
}

// Visibility Process
PVOID VisibilityProcess(PVOID TS)
{
    while(GServer->ServerOnline)
    {
        pthread_mutex_lock( &DropMutex );
        pthread_mutex_lock( &MonsterMutex );
        pthread_mutex_lock( &PlayerMutex );

        pthread_mutex_unlock( &PlayerMutex );
        pthread_mutex_unlock( &MonsterMutex );
        pthread_mutex_unlock( &DropMutex );
        #ifdef _WIN32
        Sleep(200);
        #else
        usleep(200);
        #endif
    }
    pthread_exit(NULL);
}

// World Process
PVOID WorldProcess( PVOID TS )
{
    while (GServer->ServerOnline)
    {
        pthread_mutex_lock( &DropMutex );
        pthread_mutex_lock( &MonsterMutex );
        pthread_mutex_lock( &PlayerMutex );
        pthread_mutex_lock( &MainMutex );

        pthread_mutex_unlock( &MainMutex );
        pthread_mutex_unlock( &PlayerMutex );
        pthread_mutex_unlock( &MonsterMutex );
        pthread_mutex_unlock( &DropMutex );
        #ifdef _WIN32
        Sleep(700);
        #else
        usleep(700);
        #endif
    }
    pthread_exit(NULL);
}

// Shutdown Server Process
PVOID ShutdownServer(PVOID TS)
{
    //TODO: use pthread_join to wait until all the threads are closed
    int minutes = (int)TS;
    #ifdef _WIN32
    Sleep(minutes*60000);
    #else
    usleep(minutes*60000);
    #endif
    Log( MSG_INFO, "Saving User Information... " );
    GServer->DisconnectAll();
    Log( MSG_INFO, "Waiting Process to ShutDown... " );
    GServer->ServerOnline = false;
    int status,res;
    res = pthread_join( GServer->WorldThread[0], (PVOID*)&status );
    if(res)
    {
        Log( MSG_WARNING, "World thread can't be joined" );
    }
    else
    {
        Log( MSG_INFO, "World thread closed." );
    }
    res = pthread_join( GServer->WorldThread[1], (PVOID*)&status );
    if(res)
    {
        Log( MSG_WARNING, "Visibility thread can't be joined" );
    }
    else
    {
        Log( MSG_INFO, "Visibility thread closed." );
    }
    res = pthread_join( GServer->MapThread[0], (PVOID*)&status );
    if(res)
    {
        Log( MSG_WARNING, "Map thread can't be joined" );
    }
    else
    {
        Log( MSG_INFO, "Map thread closed." );
    }
    Log( MSG_INFO, "All Threads Closed." );
    Log( MSG_INFO, "Closing Server." );
    GServer->isActive = false;
    pthread_exit(NULL);
}
