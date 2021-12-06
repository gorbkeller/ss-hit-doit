// tests.c -- unit tests for ss, hit, and doit
// Gordon Keller
// 12/05/21

/*
 * ss tests
 *  -> performing symlink
 *      for single file
 *      for multiple files
 *      for directory
 *      for multiple directories
 *  -> saving reference
 *      for single file
 *      for multiple files
 *      for directory
 *      for multiple directories
 *  -> delete symlink and reference
 *
 * hit tests
 *  -> all existing entries
 *  -> single entry
 *  -> multiple entries
 *  */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#define PASSED 0
#define FAILED -1

#define MAX_PATH_BUF 256

int assert_success( ret_val )
{
    if ( ret_val == 0 )
        { return PASSED; }
    else
        { return FAILED; }
}

int assert_failure( ret_val )
{
    if ( ret_val != 0 )
        { return PASSED; }
    else
        { return FAILED; }
}

void print_test_result( int test_result, const char *func_name )
{
    if ( test_result == PASSED )
    {
        printf( "%s : PASSED\n", func_name );
    }
    else if ( test_result == FAILED ) 
    {
        printf( "%s : FAILED\n", func_name );
    }
}

int make_dummy_file( const char *fName )
{
    FILE *fPtr;

    fPtr = fopen( fName, "w" );
    if ( fPtr == NULL )
    {
        fprintf(stderr,"ERROR: couldn't make dummy file");
        return -1;
    }

    char text[256];
    sprintf( text, "this is a test file called %s", fName );
    fputs( text, fPtr );
    if ( ferror(fPtr) )
    {
        char err_msg[256];
        sprintf( err_msg, "ERROR: couldn't write to dummy file" );
        perror( err_msg );

        fclose(fPtr);
        return -1;
    }
    
    fclose(fPtr);

    return 0;
}

int delete_dummy_file( const char *fName )
{
    int ret_val = remove(fName);
    if ( ret_val != 0 )
    {
        fprintf(stderr, "ERROR: could not delete file %s", fName);
        return -1;
    }

    return 0;
}

int test_ss_symlink_single_f()
{
    // get the current working directory -- assume that the absolute path
    // will be provided by default in the actual code
    char path[MAX_PATH_BUF];
    getcwd( path, MAX_PATH_BUF );
    const char test_fName[] = "temp/test.txt";
    const char test_aName[] = "temp/links/test_alias.txt";
    char full_fName[256];
    char full_aName[256];

    sprintf( full_fName, "%s/%s", path, test_fName );
    sprintf( full_aName, "%s/%s", path, test_aName );

    // create test file
    make_dummy_file( full_fName );

    // link test file to alias
    int ret_val = symlink( full_fName, full_aName );
    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf(err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
    }
        
    int test_result = assert_success( ret_val );
    print_test_result( test_result , __func__ );

    // link test file to alias again (expect to fail)
    ret_val = symlink( full_fName, full_aName );
    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf(err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
    }
        
    test_result = assert_failure( ret_val );
    print_test_result( test_result , __func__ );

    // delete test text file and alias
    remove( test_fName );
    remove( test_aName );

    // link test file to alias again (will succeed even though we have deleted original file)
    ret_val = symlink( full_fName, full_aName );
    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf(err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
    }
        
    test_result = assert_success( ret_val );
    print_test_result( test_result , __func__ );

    // create the newly created alias which points to nothing
    remove( test_aName );

    // link test file to alias again (expect to fail)
    if( access( full_fName, F_OK ) == 0 ) {
        // original file exists
        ret_val = symlink( full_fName, full_aName );
    } else {
        // file doesn't exist
        fprintf(stderr, "ERROR: attempted to make alias for non-existent file %s\n", full_fName);
        remove( test_aName );
        ret_val = -1;
    }

    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf(err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
    }

    test_result = assert_failure( ret_val );
    print_test_result( test_result , __func__ );

    return 0;
}

int main( void )
{
    // run tests
    test_ss_symlink_single_f();

    return 0;

}
