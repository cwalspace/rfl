#pragma once

// simple array

typedef struct{
    char **             list;
    unsigned int        size;
} array;


/* TODO: add descriptions */
array * anew( void );

void amove( array * src, array * dst );

int aadd( array * src, char * elem );

int auadd( array * src, char * elem );

void afree( array * src );

char *arand( array * src );
