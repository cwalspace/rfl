/* calloc, realloc ... */
#include <stdlib.h>
#include <string.h>
#include "array.h"

array * 
anew( void )
{
    array * a = calloc( sizeof( array ), 1 );
    return a;
}

void 
amove( array * src, array * dst )
{
    dst->list = src->list;
    src->list = 0;
    dst->size = src->size;
    src->size = 0;
}

int 
aadd( array * src, char * elem )
{
    int length = strlen( elem );
    void * tmp;

    src->size++;

    tmp = NULL;
    tmp = realloc( src->list, src->size * sizeof( void * ) );
    if ( !tmp ) {
        src->size--;
        return 0;
    }
        
    src->list = tmp;

    tmp = NULL;
    tmp = calloc( length + 1, 1 );
    if ( !tmp ) {
        src->size--;
        return 0;
    }

    src->list[src->size - 1] = tmp;
    strncpy( src->list[src->size - 1], elem, length );    

    return 1;
}

int 
auadd( array * src, char * elem )
{
    int flag = 0;

    for ( int i = 0; i < src->size; i++ ) {
        if ( !strcmp( src->list[i], elem ) ) {
            flag = 1;
            break;
        }
    }

    if ( !flag ) {
        return aadd( src, elem );
    }

    return 0;
}

void 
afree( array * src )
{
    if ( !src || !src->list )
        return;

    for ( int i = 0; i < src->size; i++ ) {
        if ( src->list[i] ) {
            free( src->list[i] );
        }
    }

    free( src->list );
    free( src );
}

char *
arand( array * src )
{
    if ( !src || !src->list || src->size == 0 )
        return ""; 

    return src->list[rand() % ( src->size )];
}
