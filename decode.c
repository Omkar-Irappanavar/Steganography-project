/*
    Name: Omkar Irappanavar
    Date: 18/09/2024
    Description: Steganography Project
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"


/* Get File pointers for i/p and o/p files */
Status open_file(DecodeInfo *decInfo)
{
    printf("INFO: Opening required files\n");

    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r"); // opening the source file in read mode
    if (decInfo->fptr_stego_image == NULL)      // to check the file is opened or not
    {
        printf(RED"INFO: Error : Image file not opened\n"RESET);
        return e_failure;
    }

    return e_success;
}


Status do_decoding(DecodeInfo *decInfo)
{
    printf(MAGENTA"INFO: ## Decoding Procedure Started ##\n"RESET);

    // open file
    if (open_file(decInfo) == e_success)    // calling function to open the files for r/w operations
    {
        printf("INFO: Opened %s\n",decInfo->stego_image_fname);
    }
    else
    {
        printf(RED"INFO: Error : Open file Failure\n"RESET);    // if file not opened printing error message
        exit(0);
    }

    /* Skip bmp image header */
    if( skip_bmp_header(decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    /* Retrive Magic String */
    if( decode_magic_string(decInfo) == e_failure)
    {
        return e_failure;
    }

    /* Decode secret file extenstion */
    if( decode_secret_file_extn(decInfo) == e_failure)
    {
        return e_failure;
    }

    /* Decode secret file size */
    if( decode_secret_file_size(decInfo) == e_failure)
    {
        return e_failure;
    }

    /* Decode secret file data */
    if( decode_secret_file_data(decInfo) == e_failure)
    {
        return e_failure;
    }

    printf(MAGENTA"INFO: ## Decoding Done Successfully ##\n"RESET);
}

/* Skip bmp image header */
Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image, 54, SEEK_SET);  // moving cursor to 55th byte(to skip first 54 bytes)
    return e_success;
}

/* Decode a LSB into int of image data array */
Status decode_lsb_to_int(uint *len,char *buf)
{
    for(int i=0; i<32; i++)     // getting lsb bit of each byte and storing in a variable(MSB to LSB)
    {
        *len = *len << 1;
        *len = *len | (buf[i] & 1);
    }

    return e_success;
}

/* Decode a LSB into byte of image data array */
Status decode_lsb_to_byte(char *data, char *image_buffer)
{
    for(int i=0; i<8; i++)      // getting lsb bit of each byte and storing in a variable(MSB to LSB)
    {
        *data = *data << 1;
        *data = *data | (image_buffer[i] & 1);
    }

    return e_success;
}


/* Retrive Magic String */
Status decode_magic_string(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Magic String Signature\n");

    // char buf[32];
    uint magic_len = strlen(MAGIC_STRING);  // calculating length of magic string
    
    fread(decInfo->buf, 32, 1, decInfo->fptr_stego_image);  // reading 32 bytes from source image file to decode magic string length
    decode_lsb_to_int(&decInfo->length,decInfo->buf);       // decoding magic string length

    if(decInfo->length != magic_len)    // to check decoded magic string length matches or not
    {
        printf(RED"INFO: Error: Give Data Encoded file as Input\n"RESET);
        return e_failure;
    }

    // char str[decInfo->length];
    decInfo->str = (char *) malloc(decInfo->length * sizeof(char)); // allocating magic string length bytes of heap memory
    for(int i=0; i<decInfo->length; i++)
    {
        fread(decInfo->buf, 8, 1, decInfo->fptr_stego_image);   // reading 8 bytes to decode the magic string
        decode_lsb_to_byte(decInfo->str+i, decInfo->buf);   // calling function to decode data
        
    }
    decInfo->str[decInfo->length] = '\0';
    // printf("magic string = %s\n",decInfo->str);

    printf(CYAN"\nEnter the magic string : "RESET); // asking user to enter the magic string 
    scanf(" %[^\n]",decInfo->buf);

    if(strcmp(decInfo->buf, decInfo->str)!=0)   // to check user entered magic string and decoded magic string matches or not
    {
        printf(RED"INFO: Error : Mismatch of Magic string\n"RESET);
        exit(0);
    }
    else
    {
        printf(GREEN"INFO: Magic string matched\n"RESET);
    }

    printf("INFO: Done\n");
    return e_success;
}


/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    printf("INFO: Decoding Output File Extension\n");

    // char buf[32];
    // uint extlen=0;

    fread(decInfo->buf, 32, 1, decInfo->fptr_stego_image);  // reading 32 bytes from encoded source image file to decode file extension length
    decode_lsb_to_int(&decInfo->length, decInfo->buf);  // calling function to decode extension length

    // char str[extlen];
    decInfo->str = (char *) malloc(decInfo->length * sizeof(char)); // allocating extension length bytes of heap memory 
    for(int i=0; i<decInfo->length; i++)
    {
        fread(decInfo->buf, 8, 1, decInfo->fptr_stego_image);   // reading 8 bytes to decode extension
        decode_lsb_to_byte(decInfo->str + i, decInfo->buf);     // calling function to decode extension
    }
    decInfo->str[decInfo->length] = '\0';

    strcat(decInfo->secret_fname, decInfo->str);    // concatinating output file name and decoded extension

    free(decInfo->str);     // freeing heap memory
    printf("INFO: Done\n");


    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");   // opening output file in write mode to store the secret data
    if (decInfo->fptr_secret == NULL)
    {
        printf(RED"INFO: Error : Output file not opened\n"RESET);
        return e_failure;
    }

    printf("INFO: Opened %s\n",decInfo->secret_fname);
    printf("INFO: Done. Opened all required files\n");

    return e_success;
}


/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Size\n",decInfo->secret_fname);

    // char buf[32];
    // uint filesize=0;

    fread(decInfo->buf, 32, 1, decInfo->fptr_stego_image);  // reading 32 bytes from encoded source file to decode size of encoded secret data
    decode_lsb_to_int(&decInfo->length, decInfo->buf);      // calling function to decode the size
    
    printf("INFO: Done\n");
    return e_success;
}


/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("INFO: Decoding %s File Data\n",decInfo->secret_fname);

    // char buf[32];
    char ch;
    printf(YELLOW"\nSecret data = "RESET);
    for(int i=0; i<decInfo->length; i++)        // decoding encoded secret data
    {
        fread(decInfo->buf, 8, 1, decInfo->fptr_stego_image);   // reading 8 bytes from encoded source file
        decode_lsb_to_byte(&ch, decInfo->buf);      // calling function to decode secret data(one one character)
        fwrite(&ch, 1, 1, decInfo->fptr_secret);    // writing the decoded data to output file
        printf(CYAN"%c"RESET,ch);               // printing the decoded secret data 
    }

    printf("INFO: Done\n");
    return e_success;
}