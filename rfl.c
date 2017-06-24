#include "global.h"
#include "array.h"

/*
 * updating array from the database file
 * if the file was modified during execution
 */
int 
update_list( char *file_name, time_t *prev_mtime, array *list )
{
    struct stat  sb;
    FILE *       rf;
    int          i;
    char *       tmp;
    array *      new_list;


    // check mod timestamp
    stat( file_name, &sb );
    if ( prev_mtime && sb.st_mtime == *prev_mtime )
        return 0;

    rf = fopen( file_name, "r" );
    
    if( !rf )
        return -1;
    
    // creating a new list to replace the old one
    new_list = anew();
    assert( new_list );
    
    // tmp buf
    tmp = ( char * )malloc( TMP_SM );
    assert( tmp );
    
    while ( !feof( rf ) ) {
        if( !fgets( tmp, TMP_SM, rf ) )
            continue;

        // trimming CR LF
        for ( int i = 0; i < 2; i++ ) {
            if ( LASTCHAR( tmp ) == 0x0A || LASTCHAR( tmp ) == 0x0D )
                LASTCHAR( tmp ) = 0;
        }
        
        // trying to add new entry, aadd will create string copy for us
        aadd( new_list, tmp );
    }
    free( tmp );
    
    fclose( rf );

    // terminating all threads to swap lists
    pthread_mutex_lock( &jobs_tracker );
    while ( workers > 0 )
        pthread_cond_wait( &threads_cap, &jobs_tracker );
    afree( list );
    *list = *new_list;
    pthread_mutex_unlock( &jobs_tracker );

    // updating access timestamp
    if (prev_mtime)
        *prev_mtime = sb.st_mtime;

    return 1;
}


/* forming and sending the request */
void *
http_request( void *arg )
{
    triple *          res;
    char *            tmp;
    int               sockfd, rv;
    struct addrinfo   hints, *servinfo, *p;

    memset( &hints, 0, sizeof hints );

    hints.ai_family     = AF_UNSPEC;
    hints.ai_socktype   = SOCK_STREAM;
    
    res                 = ( triple * )arg;

    // info out
    printf( "Host: %s\tRef: %s\n", res->host, res->ref );

    // searching
    if ( ( rv = getaddrinfo( res->host, "http", &hints, &servinfo ) ) != 0 ) {

        // we can't get any address
        printf( "getaddrinfo: %s\n", gai_strerror( rv ) );

        free( res );

        pthread_mutex_lock( &jobs_tracker );
        workers--;
        pthread_cond_signal( &threads_cap );
        pthread_mutex_unlock( &jobs_tracker );
            
        pthread_exit( NULL );
        return 0;
    }

    // loop through all the results and connect to the first one we can reach
    for ( p = servinfo; p != NULL; p = p->ai_next ) {
        if ( ( sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol ) ) == -1 ) {
            perror( "socket" );
            continue;
        }

        if ( connect( sockfd, p->ai_addr, p->ai_addrlen ) == -1 ) {
            perror( "connect" );
            close( sockfd );
            continue;
        }

        // if we get here, we must have connected successfully
        break; 
    }

    // to trash
    freeaddrinfo( servinfo ); 

    if ( p == NULL ) {

        // can't access host
        close( sockfd );
        free( res );
        printf( "failed to connect\n" );

        pthread_mutex_lock( &jobs_tracker );
        workers--;
        pthread_cond_signal( &threads_cap );
        pthread_mutex_unlock( &jobs_tracker );

        pthread_exit( NULL );
        return 0;
    }

    tmp = ( char * )malloc( TMP_MD );
    if ( tmp ) {
        sprintf( 
            tmp, 
            "GET / HTTP/1.1\r\nHost: %s\r\nConnection: Keep-alive\r\nUser-Agent: %s\r\nReferer: %s\r\n\r\n", 
            res->host,
            res->agent,
            res->ref 
        );

        // sending http request
        send( sockfd, tmp, strlen( tmp ), 0 );
        
        free( tmp );    
    }

    close( sockfd );
    free( res );

    // threads / jobs status update
    pthread_mutex_lock( &jobs_tracker );
    successful_jobs++;    
    printf( "Working threads: %d\tTotal number of jobs done: %ld\n", workers, successful_jobs );
    workers--;
    pthread_cond_signal( &threads_cap );
    pthread_mutex_unlock( &jobs_tracker );

    pthread_exit( NULL );
    return 0;
}


int 
main( int argc, char **argv )
{
    printf( "\nCheck out free new hosts for your links on http://cwal.space/runforlinks\n\n" );
    sleep(1);
    
    if ( argc < ARGS_COUNT ) {
        printf( "Usage: ./<this> <max threads> <path to hosts.txt> <path to refs.txt>\nFile data format: <line>\\0x0A\n" );

        return EXIT_SUCCESS;
    }

    int threads_count  = atoi( argv[ARG_TMAX] );
    char * file_hosts  = argv[ARG_HOSTS];
    char * file_refs   = argv[ARG_REFS];

    time_t hosts_mtime = 0;
    array * hosts      = anew();
    time_t refs_mtime  = 0;
    array * refs       = anew();
    array * agents     = anew();
    workers            = 0;
    successful_jobs    = 0;

    if ( !hosts || !refs || !agents ) {
        printf( "memory allocation error\n" );
        return 0;
    }

    srand( time( NULL ) );

    // adding user agents
    update_list( AGENTS_FILE, NULL, agents );

    // main loop
    for ( ;; ) {

        pthread_t thread;
        triple *  res = NULL;
        
        res = malloc( sizeof( triple ) );
        if ( !res ) {
            printf( "memory allocation error, trying again\n" );
            continue;
        }

        // reloading files if needed
        update_list( file_hosts, &hosts_mtime, hosts );
        update_list( file_refs, &refs_mtime, refs );
        
        // threads cap
        pthread_mutex_lock( &jobs_tracker );
        while ( workers >= threads_count )
            pthread_cond_wait( &threads_cap, &jobs_tracker );
        workers++;
        pthread_mutex_unlock( &jobs_tracker );

        // passing random ref/host/agent 
        res->host = arand(hosts);
        res->ref = arand(refs);
        res->agent = arand(agents);

        // launch deattached thread
        pthread_create( &thread, NULL, &http_request, res );
        pthread_detach( thread );
    }

    afree( refs );
    afree( hosts );
    afree( agents );

    pthread_exit( NULL );
    return EXIT_SUCCESS;
}