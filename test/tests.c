// tests.c -- unit tests for ss, hit, and doit
// Gordon Keller
// 12/05/21

/* -- Expectations for ss functionality -- */
/*
 * 1. To create a symlink for a file (explicitly or implicitly) OR a directory.
 * 2. To add a reference line to the link reference file stored in /tmp/ss-hit-doit of the form:
 *      " [original file], [symlink to file], [status ((l)inked, (t)ransferred, or (d)eleted] "
 */
 
 // Based on the functionality of symlink(), a single alias cannot refer to multiple
 // original files. In keeping with this, checks to the reference file will be made
 // to ensure that a new link created has a unique alias assigned to it. Additionally, 
 // functionality should be included which allows the user to query the link reference 
 // file to avoid needlessly creating separate aliases for a single file.

/*
 * ss tests
 *  -> performing symlink
 *      a. for single file successfully against explicit path (with filename)
 *      b. for single file unsuccessfully against explicit path (with filename) due to:
 *          1. attempting to save over an alias that is taken with the same original filename
 *          2. attempting to save for a file that doesn't exist
 *              -> symlink succeeds without a check
 *              -> apply a check and verify that it fails
 *          3. attempting to save in a directory that doesn't exist 
 *          4. attempting to save over a preexisting alias with a different original file
 *      c. for single file successfully against implicit path (without filename)
 *      d. for single file unsuccessfully against implicit path (without filename) due to:
 *          1. attempting to save over an alias that is taken (i.e., has the symbolic name at link but not passed explicitly) 
 *          2. attempting to save for a file that doesn't exist
 *          3. attempting to save in a directory that doesn't exist 
 *      d. for multiple files
 *      e. for directory
 *      f. for multiple directories
 *  -> saving reference
 *      for single file
 *      for multiple files
 *      for directory
 *      for multiple directories
 *  -> delete symlink and reference (" ss -d ")
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


typedef enum PathType
{
    FILEPATH = 0,
    DIRPATH = 1,
    UNKPATH = 2,
    ERRPATH = -1
} PathType;

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

void print_test_result( int test_result, const char *func_name, const char *test_name )
{
    if ( test_result == PASSED )
    {
        char result[] = "\033[1;32mPASSED\033[0m";
        printf( "%s (%s): %s\n", func_name, test_name, result );
    }
    else if ( test_result == FAILED ) 
    {
        char result[] = "\033[1;31mFAILED\033[0m";
        printf( "%s (%s): %s\n", func_name, test_name, result );
    }
}

PathType identify_path_type(char *path)
{
    // this function determines if the path provided is a file or a directory
    struct stat s;
    if( stat(path,&s) == 0 )
    {
        if( s.st_mode & S_IFDIR )
        {
            //it's a directory
            return DIRPATH;
        }
        else if( s.st_mode & S_IFREG )
        {
            //it's a file
            return FILEPATH;
        }
        else
        {
            //something else
            return UNKPATH;
        }
    }
    else
    {
        fprintf(stderr, "ERROR:  %s is not a valid path name.\n", name);
        
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

int test_ss_symlink_single_f_explicit()
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

/* -- a. for single file successfully against explicit path (with filename) -- */
 
    // link test file to alias
    int ret_val = symlink( full_fName, full_aName );
    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf(err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
    }
        
    int test_result = assert_success( ret_val );
    print_test_result( test_result , __func__ , "a1" );

/* end of subtest a. */

/* -- b. for single file unsuccessfully against explicit path (with filename) due to: -- */

// 1. attempting to save over an alias that is taken with the same original filename
    // link test file to alias again (expect to fail)
    ret_val = symlink( full_fName, full_aName );
    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf(err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
    }
        
    test_result = assert_failure( ret_val );
    print_test_result( test_result , __func__, "b1" );

    // delete test text file and alias
    remove( test_fName );
    remove( test_aName );

// 2. attempting to save for a file that doesn't exist
    // link test file to alias again (will succeed even though we have deleted original file)
    ret_val = symlink( full_fName, full_aName );
    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf( err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
    }
        
    test_result = assert_success( ret_val );
    print_test_result( test_result , __func__ , "b2" );

    // create the newly created alias which points to nothing
    remove( test_aName );

// 3. attempting to save in a directory that doesn't exist 
    // link test file to alias again (expect to fail)
    if( access( full_fName, F_OK ) == 0 ) {
        // original file exists
        ret_val = symlink( full_fName, full_aName );
    } else {
        // file doesn't exist
        fprintf( stderr, "ERROR: attempted to make alias for non-existent file %s\n", full_fName );
        remove( test_aName );
        ret_val = -1;
    }

    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf( err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aName );
        perror( err_msg );
    }

    test_result = assert_failure( ret_val );
    print_test_result( test_result , __func__ , "b3" );

// 4. attempting to save over a preexisting alias with a different original file


    /* end of subtest b. */
    return 0;
}


int test_ss_symlink_single_f_implicit(void)
{
    // get the current working directory -- assume that the absolute path
    // will be provided by default in the actual code
    char path[MAX_PATH_BUF];
    getcwd( path, MAX_PATH_BUF );
    const char test_fName[] = "temp/test.txt";
    const char test_aDir[] = "temp/links/";
    char full_fName[256];
    char full_aDir[256];

    sprintf( full_fName, "%s/%s", path, test_fName );
    sprintf( full_aDir, "%s/%s", path, test_aDir );

    // create test file
    make_dummy_file( full_fName );

    // determine if the provided original path is a directory or a file
    PathType fName_path_type = identify_path_type( full_fName );

    // currently, we can't process directories as the origin. need to implement later.
    if ( fName_path_type == DIRPATH )
    {
        fprintf( stderr, "ERROR: cannot use path type of directory for linking. Argument passed: %s", full_fName );
        remove( full_fName );
        return -1;
    } else if ( fName_path_type == ERRPATH )
    {
        fprintf( stderr, "ERROR: The path passed throws an error. Argument passed: %s", full_fName );
        remove( full_fName );
        return -1;
    } else if ( fName_path_type == UNKPATH )
    {
        fprintf( stderr, "ERROR: The path passed is of an unknown type -- only files are allowed. Argument passed: %s", full_fName );
        remove( full_fName );
        return -1;
    }


    // determine if the provided original path is a directory or a file
    aName_path_type = identify_path_type(full_aName);

    // if a directory is passed, append the name of the original file to the end 
    if ( fName_path_type == DIRPATH )
    {
        int ret_val = alias_dir_to_file( full_aName, full_fName );
        remove( full_fName );
        return -1;
    } else if ( fName_path_type == ERRPATH )
    {
        fprintf( stderr, "ERROR: The path passed throws an error. Argument passed: %s", full_fName );
        remove( full_fName );
        return -1;
    } else if ( fName_path_type == UNKPATH )
    {
        fprintf( stderr, "ERROR: The path passed is of an unknown type -- only files are allowed. Argument passed: %s", full_fName );
        remove( full_fName );
        return -1;
    }

    // link test file to alias
    int ret_val = symlink( full_fName, full_aDir );
    if ( ret_val != 0 )
    {
        char err_msg[256];
        sprintf(err_msg, "ERROR: could not create symlink %s -> %s\n", full_fName, full_aDir );
        perror( err_msg );
    }
        
    int test_result = assert_success( ret_val );
    print_test_result( test_result , __func__ , "a1" );

    return 0;

}

int main( void )
{
    // run tests
    //test_ss_symlink_single_f_explicit();
    test_ss_symlink_single_f_implicit();

    return 0;

}
