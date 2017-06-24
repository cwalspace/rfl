#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <locale.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define LASTCHAR( x )   ( x[strlen( x ) - 1] )

#define ARGS_COUNT      4
#define ARG_TMAX        1
#define ARG_HOSTS       2
#define ARG_REFS        3

#define TMP_SM          0x400
#define TMP_MD          0xC00

#define AGENTS_FILE     "agents.txt"

// global vars

typedef struct{
    char *              host;
    char *              ref;
    char *              agent;
} triple;

extern pthread_mutex_t         jobs_tracker;
extern pthread_cond_t          threads_cap;

extern int                     workers;
extern long                    successful_jobs;


