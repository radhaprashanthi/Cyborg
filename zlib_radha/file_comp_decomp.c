/* file_comp_decomp.c: used to compress and decompress a file using zlib's inflate() and deflate() */

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "zlib.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#define CHUNK 16384

/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
int def(FILE *source, FILE *dest, int level)
{
    int ret, flush;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    int actual_fsize = 0;
    int comp_fsize = 0;

    /* allocate deflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    ret = deflateInit(&strm, level);
    if (ret != Z_OK)
        return ret;

    /* compress until end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        actual_fsize += strm.avail_in;
        if (ferror(source)) {
            (void)deflateEnd(&strm);
            return Z_ERRNO;
        }
        flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
        strm.next_in = in;

        /* run deflate() on input until output buffer not full, finish
           compression if all of source has been read in */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = deflate(&strm, flush);    /* no bad return value */
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            have = CHUNK - strm.avail_out;
            comp_fsize += have;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);
        assert(strm.avail_in == 0);     /* all input will be used */

        /* done when last data in file processed */
    } while (flush != Z_FINISH);
    assert(ret == Z_STREAM_END);        /* stream will be complete */

    printf("actual file size = %d\n", actual_fsize);
    printf("compressed file size = %d\n", comp_fsize);

    /* clean up and return */
    (void)deflateEnd(&strm);
    return Z_OK;
}

/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files. */
int inf(FILE *source, FILE *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    int actual_fsize = 0;
    int comp_fsize = 0;

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        actual_fsize += strm.avail_in;
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);
            assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;     /* and fall through */
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    (void)inflateEnd(&strm);
                    return ret;
            }
            have = CHUNK - strm.avail_out;
            comp_fsize += have;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    printf("compressed file size = %d\n", actual_fsize);
    printf("decompressed file size = %d\n", comp_fsize);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

/* report a zlib or i/o error */
void zerr(int ret)
{
    fputs("file_comp_decomp: ", stderr);
    switch (ret) {
        case Z_ERRNO:
            if (ferror(stdin))
                fputs("error reading stdin\n", stderr);
            if (ferror(stdout))
                fputs("error writing stdout\n", stderr);
            break;
        case Z_STREAM_ERROR:
            fputs("invalid compression level\n", stderr);
            break;
        case Z_DATA_ERROR:
            fputs("invalid or incomplete deflate data\n", stderr);
            break;
        case Z_MEM_ERROR:
            fputs("out of memory\n", stderr);
            break;
        case Z_VERSION_ERROR:
            fputs("zlib version mismatch!\n", stderr);
    }
}

/* function to check if the filename is for a file or folder */
int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

/* compress or decompress from stdin to stdout */
int main(int argc, char **argv)
{
    /* otherwise, report usage */
    if (argc != 3) {
        fputs("file_comp_decomp usage: file_comp_decomp <filename> <mode>\n", stderr);
        return 1;
    }

    int ret;
    char *s[10], *p;
    int i = 0;
    char out_filename[100];
    FILE *ptr_in, *ptr_out;

    /* check if file exists or not to open */
    if(access(argv[1], F_OK) != -1) {
        ptr_in = fopen(argv[1],"rb");
    } else {
        printf("Error: file %s does not exist! Enter correct filename!\n", argv[1]);
        return 1;
    }

    /* check if the filename is a folder */
    if(is_regular_file(argv[1]) == 0) {
        printf("Error: %s is a folder! Enter filename and not folder name!\n", argv[1]);
        return 1;
    }

    /* if the file name has any extension split the file name*/
    if (strchr(argv[1], '.') != NULL) {
        p = strtok (argv[1],".");
        while (p!= NULL)
        {
            s[i] = (char *)p;
            i+=1;
            p = strtok (NULL, ".");
        }
        strcpy(out_filename, s[0]);
        strcat(out_filename, "_out.");
        for (int j = 1; j<i; j++){
            strcat(out_filename, s[j]);
            if (j < i-1)
                strcat(out_filename, ".");
        }
    } else {
        strcpy(out_filename, argv[1]);
        strcat(out_filename, "_out");
    }

    printf("output filename = %s\n", out_filename);
    ptr_out = fopen(out_filename,"w");
    free(p);

    if (argc == 3) {
        /* do compression if mode is comp */
        if (strcmp(argv[2], "comp") == 0) {
            ret = def(ptr_in, ptr_out, Z_BEST_COMPRESSION);
            if (ret != Z_OK)
                zerr(ret);
            return ret;
        }

        /* do decompression if mode is decomp */
        if (strcmp(argv[2], "decomp") == 0) {
            ret = inf(ptr_in, ptr_out);
            if (ret != Z_OK)
                zerr(ret);
            return ret;
        }

        else {
            fputs("<mode> usage: mode can be comp/decomp only\n", stderr);
            return 1;
        }
    }
    return 0;
}
