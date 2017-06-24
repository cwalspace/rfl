#include "global.h"

pthread_mutex_t         jobs_tracker = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t          threads_cap = PTHREAD_COND_INITIALIZER;

int                     workers;
long                    successful_jobs;


