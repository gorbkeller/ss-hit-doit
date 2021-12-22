// ss.c : create and save reference to a symlink 
// Gordon Keller
// 12/01/21
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 10000
#define STR_BUFFER_SIZE 1000

void readFile(FILE * fPtr);
int createLink( const char *origin_str, const char *dest_str );
int generateLinkReference( char *buf, const char *origin_str, const char *dest_str );

int main( int argc, const char *argv[] ) {

    if ( argc < 3 )
    {
        fprintf(stderr, "ERROR: less than 2 arguments provided. Use: ss [file_path] [alias_path]\n");
        exit( EXIT_FAILURE );
    }

    char origin_path[STR_BUFFER_SIZE];
    char copy_path[STR_BUFFER_SIZE];
    const char ref_path[STR_BUFFER_SIZE] = "/tmp/ss-hits-doit/ref.csv";

    strcpy( origin_path, argv[1] );
    strcpy( copy_path, argv[2] );

    FILE *ref_fPtr;
    char *ref_info_buf = malloc(BUFFER_SIZE);
    
    // create the symlink
    int success = symlink(origin_path,copy_path);
    if (success == -1) {
        /* Unable to open file hence exit */
        printf("\nUnable to create the link");
        exit(EXIT_FAILURE);
    }
    
    //construct the string to append as a new line
    success = generateReferenceText(ref_info_buf,origin_path,copy_path);
    if (success == -1) {
        /* Unable to open file hence exit */
        printf("\nUnable to write the data to the buffer");
        printf("\nto-do: give details of how to remedy");
        exit(EXIT_FAILURE);
    }

    // open the ref file to write linkage data to
    ref_fPtr = fopen(ref_path, "a+");
    if (ref_fPtr == NULL) {
        /* Unable to open file hence exit */
        printf("\nUnable to open '%s' file.\n", ref_path);
        printf("Please check whether file exists and you have write privilege.\n");
        exit(EXIT_FAILURE);
    }

    // append to the file
    int ret_val = fputs(ref_info_buf,ref_fPtr);
    if (ret_val == EOF)
    {
        fprintf(stderr,"ERROR: failed to write the link info to reference file.\n");
        fclose(ref_fPtr);
        exit(EXIT_FAILURE);

    }

    // clean up
    fclose(ref_fPtr);
    free(ref_info_buf);

    return 0;

}

// generates the line of reference text as a string
int generateReferenceText(char *buf, const char *origin_str, const char *dest_str)
{
    return sprintf( buf, "%s,%s,ss\n", origin_str,dest_str );
}

// creates the actual symlink
int createLink( const char *full_fName, const char *full_aName )
{
    if( access( full_fName, F_OK ) != 0 ) {
    } 
        // original file doesn't exist
        fprintf( stderr, "ERROR: attempted to make alias for non-existent file %s\n", full_fName );
        remove( test_aName );
        return -1;
    }

    ret_val = symlink( full_fName, full_aName );
    if ( ret_val != 0 )
    {
        // symlink failed
        char err_msg[256];
        sprintf(err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
        return -1;
    }

    return 0;
}
    


