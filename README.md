# Cyborg Assignment
    Build a simple compression and decompression tool using Zlib library using C programming language.

## Steps to follow:
    1. Clone the zlib repo from github
    2. Build the zlib library
    3. Use zlib.h and built zlib library to create a compressor/decompressor, which will take a `filename` and `mode(compressor/decompressor)` as arguments.

## System Properties:
    1. Linux mint OS
    2. gcc compiler


## Execution Details:
    1. cd zlib_radha/
    2. run `./configure`
    3. Next, run `make` to build all required essentials from zlib.
    4. `gcc -o file_comp_decomp file_comp_decomp.c -lz` which will generate executable out file.
    4. `./file_comp_decomp <file_name> <mode>` where file_name is the file which needs to be compressed or decompressed & mode is whether it needs to be compressed or decompressed
    5. Created `test_files/` for testing various files using the above compression and decompression.
    6. Myself tested for all various extensions of files. BUT, on github I am pushing only for the files of example run below.

## Example Run:
    ```
    gcc -o file_comp_decomp file_comp_decomp.c -lz

    Compresssion:
    ./file_comp_decomp test_files/WP.csv comp
    
    output:
        output_filename = tset_files/WP_out.csv
        actual_file_size = 109394
        compressed_file_size = 38644

    Decompression:
    ./file_comp_decomp test_files/WP_out.csv decomp

    output:
        output_filename = test_files/WP_out_out.csv
        actual_file_size = 38644
        decompressed_file_size = 109394

    ```


## Implementation Details:
    1. A file called `file_comp_decomp.c` exists in the repo which is main file for our functionality
    2. This file used `zlib.h` from zlib library
    3. Two functions called `def() and inf()` are created which will compress and decompress files respectively.
    4. `def()` will compress the file source to dest until EOF on source and it returns Z_OK on success, Z_MEM_ERROR if memory couldnt be allocated for processing, Z_STREAM_ERROR if an invalid compression level is supplied, Z_VERSION_ERROR if the version of zlib.h anf the version of the library linked donot match, or Z_ERRNO if there is an error reading or writing files
    5. `inf()` will decompress from file source to file dest util stream ends or EOF and returns Z_OK on sucess, Z_MEM_ERROR if memory couldnt be allocated for processing, Z_DATA_ERROR if the deflate data is invalid or incomplete, Z_ERRNO if there is an error reading or writing files.
    6. Above errors mentioned for both `def() and inf()` are referred from `zerr()` function.
    7. `main()` function to call the functions to compress or decompress from input arguments.
    8. `def()` compression function uses `Z_BEST_COMPRESSON` among the available compression levels from zlib.h like Z_BEST_SPEED, Z_DEFAULT_COMPRESSION which performed better compared to others.

## Handling edge cases:
    1. Filename and mode not passed in arguments
        `file_comp_decomp usage: file_comp_decomp <filename> <mode>`

    2. Incorrect filename passed
        `Error: file <filename> does not exist! Enter correct filename!`

    3. Folder name passed instead of filename
        `Error: <filename> is a folder! Enter filename and not folder name!`
