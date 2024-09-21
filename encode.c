/*
    Name: Omkar Irappanavar
    Date: 18/09/2024
    Description: Steganography Project
*/

#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

/*
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}
*/

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo)      // opening files to perform some operations(r/w)
{
    printf("INFO: Opening required files\n");

    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r"); // opening file in read mode
    if (encInfo->fptr_src_image == NULL)    // checking file opened or not
    {
        printf(RED"INFO: Image file not opened\n"RESET);
        return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo->src_image_fname);

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");   // opening file in read mode
    if (encInfo->fptr_secret == NULL)
    {
        printf(RED"INFO: Secret file not opened\n"RESET);
        return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo->secret_fname);


    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");     // opening file in write mode
    if (encInfo->fptr_stego_image == NULL)
    {
        printf(RED"INFO: Output file not opened\n"RESET);
        return e_failure;
    }
    printf("INFO: Opened %s\n",encInfo->stego_image_fname);

    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    // open file
    if (open_files(encInfo) == e_success)   // to check all files are opened or not
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf(RED"INFO: Open file Failure\n"RESET);
        
        exit(0);    // terminats the program
    }

    printf(MAGENTA"INFO: ## Encoding Procedure Started ##\n"RESET);

    /* check capacity */
    if( check_capacity(encInfo) == e_failure )
    {
        return e_failure;
    }   

    // copy bmp header
    if( copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    // magic string len and magic string
    if( encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        return e_failure;
    }

    // encode file extension size/length
    // encode file extension
    if( encode_secret_file_extn(encInfo->secret_fname, encInfo) == e_failure)
    {
        return e_failure;
    }

    /* Encode secret file size*/
    if( encode_secret_file_size(get_file_size(encInfo->fptr_secret), encInfo) == e_failure)
    {
        return e_failure;
    }

    /* Encode secret file data */
    if( encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }

    /* Copy remaining image bytes from src to stego image after encoding */
    if( copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    printf(MAGENTA"INFO: ## Encoding Done Successfully ##\n"RESET);
}


/* check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    printf("INFO: Checking for %s size\n",encInfo->secret_fname);
    if(get_file_size(encInfo->fptr_secret) > 0)     // checking the secret file is empty or not
    {
        printf("INFO: Done. Not Empty\n");
        rewind(encInfo->fptr_secret);
    }
    else
    {
        printf(RED"INFO: Secret File is Empty\n"RESET);
        return e_failure;
    }

    printf("INFO: Checking for %s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname);
    if((get_file_size(encInfo->fptr_src_image) - get_file_size(encInfo->fptr_secret)) > 54)     // to check the image file size is larger than secret file size or not
    {
        printf("INFO: Done. Found OK\n");
        rewind(encInfo->fptr_src_image);
        rewind(encInfo->fptr_secret);
    }
    else
    {
        printf(RED"INFO: %s file has larger size than %s size\n"RESET,encInfo->secret_fname, encInfo->src_image_fname);
        return e_failure;
    }

    return e_success;
}


// copy bmp header 
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("INFO: Copying Image Header\n");

    char buf[54]; // to copy first 54 bytes of data from bmp file
    fread(buf, 54, 1, fptr_src_image);  // copying first 54 bytes of data from <source bmp> file
    fwrite(buf, 54, 1, fptr_dest_image);    // storing 54 bytes of data into <output bmp> file
    
    printf("INFO: Done\n");
    return e_success;
}

/* Encode a int into LSB of image data array */
Status encode_int_to_lsb(int len, char *buf)
{
    for (int i = 31; i >= 0; i--)   // storing each bit of integer value(32 bits) into lsb of each byte data(32 bytes)
    {
        if (len & (1 << i))
        {
            buf[31 - i] = buf[31 - i] | 1;
        }
        else
        {
            buf[31 - i] = buf[31 - i] & ~1;
        }
    }

    return e_success;
}

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 7; i >= 0; i--)        // storing each bit of a byte(8 bits) into lsb of a byte(8 bytes)
    {
        if (data & (1 << i))
        {
            image_buffer[7 - i] = image_buffer[7 - i] | 1;
        }
        else
        {
            image_buffer[7 - i] = image_buffer[7 - i] & ~1;
        }
    }

    return e_success;
}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("INFO: Encoding Magic String Signature\n");

    int len = strlen(magic_string);     // calculating length of magic strength

    char buf[32];
    fread(buf, 32, 1, encInfo->fptr_src_image);   // to store the length of magic string, reading 32 bytes of data from source image file
    encode_int_to_lsb(len, buf);                // calling function to encode the length
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);  // storing the encoded data to output image file

    for (int i = 0; i < len; i++)
    {
        fread(buf, 8, 1, encInfo->fptr_src_image);    // to store the magic string, reading 8 bytes data from source image
        encode_byte_to_lsb(magic_string[i], buf);     // calling function to encode 1 byte each time
        fwrite(buf, 8, 1, encInfo->fptr_stego_image);  // writing encoded data to o/p image
    }

    printf("INFO: Done\n");
    return e_success;
}

// to encode secret file extension and length
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Extension\n",encInfo->secret_fname);

    char ext[10];
    char *ptr = strrchr(encInfo->secret_fname, '.');    
    if(ptr == NULL)
    {
        printf(RED"INFO: No file extension found\n"RESET);
    }
    else
    {
        strcpy(ext, ptr);
    }

    int len = strlen(ext);      // calculating the length of secret file extension

    char buf[32];
    fread(buf, 32, 1, encInfo->fptr_src_image);     // to encode the extension length, reading 32 bytes data from source image file
    encode_int_to_lsb(len, buf);                    // calling function to encode extension length
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);  // writing tha encoded data into output image file

    for (int i = 0; i < len; i++)
    {
        fread(buf, 8, 1, encInfo->fptr_src_image);    // to encode secret file extension, reading 8 bytes from source image
        encode_byte_to_lsb(ext[i], buf);               // calling function to encode 1 byte each time
        fwrite(buf, 8, 1, encInfo->fptr_stego_image);  // writing encoded data to o/p image
    }

    printf("INFO: Done\n");

    return e_success;
}


/* Get file size */
uint get_file_size(FILE *fptr)  // to claculate size of file
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}


/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Size\n",encInfo->secret_fname);

    rewind(encInfo->fptr_secret);
    char buf[32];

    fread(buf, 32, 1, encInfo->fptr_src_image); // to encode secret file size, reading 32 bytes of data from source image file
    encode_int_to_lsb(file_size, buf);          // calling function to encode the size
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);  // writing the encoded data into output image file

    printf("INFO: Done\n");
    return e_success;
}


/* Encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("INFO: Encoding %s File Data\n",encInfo->secret_fname);

    rewind(encInfo->fptr_secret);

    char buf[8];
    char ch = fgetc(encInfo->fptr_secret);
    while(ch != EOF)        // reading all data from secret file
    {
        fread(buf, 8, 1, encInfo->fptr_src_image);  // to encode one character, reading 8 bytes from source image file
        encode_byte_to_lsb(ch, buf);                // calling function to encode data
        fwrite(buf, 8, 1, encInfo->fptr_stego_image);   // writing the encoded data into output image file
        ch = fgetc(encInfo->fptr_secret);       // reading a character from secret file
    }

    printf("INFO: Done\n");
    return e_success;
}


/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    printf("INFO: Copying Left Over Data\n");

    char buf[32];
    while(!feof(fptr_src))      // copying all remaining data from source image file to output image file
    {
        fread(buf, 32, 1, fptr_src);
        fwrite(buf, 32, 1, fptr_dest);
    }

    printf("INFO: Done\n");
    return e_success;
}