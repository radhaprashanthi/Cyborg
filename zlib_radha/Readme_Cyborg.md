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
    1. run `./configure`
    2. Next, run `make` to build all required essentials from zlib 
    3. `make run file_comp_dcomp` which will create object file for our program to run with required arguments
    4. `./file_comp_dcomp <file_name> <mode>` where file_name is the file which needs to be compressed or decompressed & mode is whether it needs to be compressed or decompressed

## Example output:
    ```

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
