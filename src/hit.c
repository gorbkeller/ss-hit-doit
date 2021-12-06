// hit.c : copy to a predetermined location
// Gordon Keller
// 12/01/21
#include <stdio.h>    
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#define BUF_REF_PATH_LEN 2048
#define BUF_REF_LINE_LEN 4096
#define DATA_COPY_BUF 2048
#define FNAME_BUF 1024
#define BUF_FILE_LINES 1024
#define MAX_CHARS_STATUS 16

/* FUNCTION PROTOTYPES */
int copyFile(char *original_path, char *dest_path);
int parseRefLine( char *ref_line_buf, char *orig_fName, char *dest_fName, char *status );

int main(void) {
    // parse arguments/print use if none supplied
    // perform a copy for the specified file to the referenced symlink
    
    const char *ref_fName = "/tmp/ss-hits-doit/ref.csv";
    FILE *ref_fPtr;
    char orig_fName[BUF_REF_PATH_LEN];
    char dest_fName[BUF_REF_PATH_LEN];
    char status[MAX_CHARS_STATUS];
    char ref_line_buf[BUF_REF_LINE_LEN];

    // open the ref file
    ref_fPtr = fopen( ref_fName, "r" );
    if ( ref_fPtr == NULL ) {
        printf( "Failed to open reference file." );
        exit( EXIT_FAILURE );
    }

    // read csv one line at a time
    while ( fgets( ref_line_buf, BUF_REF_LINE_LEN, ref_fPtr ) )
    {
        // parse the elements from line
        parseRefLine( ref_line_buf, orig_fName, dest_fName, status );
        printf( "%s -> %s (%s)\n", orig_fName, dest_fName, status );

        // remove the alias
        int ret_val = remove(dest_fName);
        if ( ret_val != 0 ) {
            fprintf(stderr, "Failed to delete the alias : %s.", dest_fName );
            exit( EXIT_FAILURE );
        }

        // copy file
        ret_val = copyFile( orig_fName, dest_fName );
        if ( ret_val < 0 )
        {
            fprintf( stderr, "Error copying file." );
            fclose( ref_fPtr );
            exit( EXIT_FAILURE );
        }
    }

    // clean up
    fclose( ref_fPtr );

    return 0;
}

int copyFile(char *orig_path, char *dest_path)
{
    FILE *orig_fPtr;
    FILE *dest_fPtr;
    char *read_buf;

    // open the file to read and copy
    orig_fPtr = fopen( orig_path, "rb" );
    if( orig_fPtr == NULL )
    {
        printf( "ERROR: could not open file to copy.\n" );        
        return -1;
    }    

    // open the file to write to 
    dest_fPtr = fopen( dest_path, "wb" );
    if( dest_fPtr == NULL )
    {
        printf( "ERROR: could not create file pointer to destination.\n" );        
        return -1;
    }    

    // allocate space to read data into
    read_buf = (char *) malloc( sizeof(char) * DATA_COPY_BUF );
    if ( read_buf == NULL )
    {
        fprintf( stderr, "ERROR: could not allocate memory for read buffer" );
        return -1;
    }

    // iteratively copy chunks of data 
    int data_read = 0;
    int iter = 0;
    do {
        //printf("copy loop: %d", iter++);
        data_read = fread( read_buf, 1, DATA_COPY_BUF,  orig_fPtr ); 
        //printf("%d",data_read);
        if ( ferror( orig_fPtr ) ) 
        {
            char err_msg[256];
            sprintf( err_msg, "Failed to read data from %s. Error ", orig_path );
            perror( err_msg );
            return -1;
        }

        fwrite( read_buf, 1, data_read, dest_fPtr ); 
        if ( ferror( dest_fPtr ) ) 
        {
            char err_msg[256];
            sprintf( err_msg, "Failed to write data to %s. Error ", dest_path );
            perror( err_msg );
            return -1;
        }
    } while (data_read == DATA_COPY_BUF);

    // clean up
    fclose(orig_fPtr);
    fclose(dest_fPtr);
    free(read_buf);

    return 0;
}


int parseRefLine( char *ref_line_buf, char *orig_fName, char *dest_fName, char *status )
{
    strcpy( orig_fName, strtok( ref_line_buf, "," ) );
    strcpy( dest_fName, strtok( NULL, "," ) );
    strcpy( status, strtok( NULL, "\n" ) );
    return 0;
}


/* include later
    // check to see if the copy was successful based on the amount of data copied: maybe redundant?
    orig_fSize = fsize(orig_path);

    if ( orig_fSize == -1 )
    {
        fprintf(stderr, "ERROR: could not get the size of the original file to compare");
        return -1;
    }

    if ( data_copied_size != fsize(orig_path) ) 
    {
        fprintf(stderr, "ERROR: copy process ended but resulting file sizes are different");
        return -1;
    }

off_t fsize(const char *filename) {
    struct stat st;

    if ( stat(filename, &st) == 0 )
        return st.st_size;

    fprintf(stderr, "Cannot determine size of %s: %s\n",
            filename, strerror(errno));

    return -1;
}*/
