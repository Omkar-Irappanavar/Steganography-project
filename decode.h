#ifndef DECODE_H
#define DECODE_H

#include "types.h"

/* 
 * Structure to store information required for
 * decoding secret file from output Image
 * Info 
 */

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output File Info */
    char secret_fname[25];
    FILE *fptr_secret;

    /* to store data while decoding*/
    uint length;
    char buf[32];
    char *str;

} DecodeInfo;


/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Perform the Decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_file(DecodeInfo *decInfo);

/* Skip bmp image header */
Status skip_bmp_header(FILE *fptr_stego_image);

/* Retrive Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a LSB into int of image data array */
Status decode_lsb_to_int(uint *len, char *buf);

/* Decode a LSB into byte of image data array */
Status decode_lsb_to_byte(char *data, char *image_buffer);

#endif