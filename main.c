/*
    Name: Omkar Irappanavar
    Date: 18/09/2024
    Description: Steganography Project
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"


int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    if(argc < 3)
    {
        printf(RED"./a.out: Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n"RESET);
        printf(RED"./a.out: Decoding: ./a.out -d <.bmp file> [output file]\n"RESET);
        return e_failure;
    }
    else if (strcmp(argv[1], "-e") == 0) 
    {
        /* Read and validate Encode args from argv */
        if( !(read_and_validate_encode_args(argc, argv, &encInfo)) )
        {
            return e_failure;
        }

        // call encoding function to encode the secret data into image file along with magic string(length, data) and secret file extension(length and file extension)
        do_encoding(&encInfo);

        fclose(encInfo.fptr_secret);    // closing files
        fclose(encInfo.fptr_src_image);
        fclose(encInfo.fptr_stego_image);
        
    }
    else if( strcmp(argv[1], "-d") == 0)
    {
        /* Read and validate Decode args from argv */
        if( !(read_and_validate_decode_args(argc, argv, &decInfo)) )
        {
            return e_failure;
        }

        // call decoding function to decode the encoded data(magic string length, magic string data, file extension length, file extension, secret data size, secret data) from image file
        do_decoding(&decInfo);

    }
    else
    {
        printf(RED"INFO: %s is Unsupported Command\n"RESET,argv[1]);
        return e_failure;
    }


}


/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    encInfo->src_image_fname = strstr(argv[2], ".bmp");
            
    if( (encInfo->src_image_fname == NULL) || (strcmp(encInfo->src_image_fname, ".bmp") != 0))
    {
        printf(RED"./a.out: Encoding: ./a.out -e <.bmp file> <secret file> [output file]\n"RESET);
        return e_failure;
    }
    else
    {
        encInfo->src_image_fname = argv[2];
    }

    if ((argc < 4) )
    {
        printf(RED"./a.out: Encoding: ./a.out -e <.bmp file> <secret file> [output file]\n"RESET);
        return e_failure;
    }
    else if (argc > 4)
    {
        encInfo->stego_image_fname = strstr(argv[4], ".bmp");
        if( (encInfo->stego_image_fname == NULL) || (strcmp(encInfo->stego_image_fname, ".bmp") != 0))
        {
            printf(RED"INFO: Output file extension should be <.bmp file>\n"RESET);
            return e_failure;
        }
        else
        {
            encInfo->stego_image_fname = argv[4];
        }
    }
    else
    {
        printf(YELLOW"INFO: Output File not mentioned. Creating output.bmp as default\n"RESET);
        encInfo->stego_image_fname = "output.bmp";
    }

    encInfo->secret_fname = argv[3];

    return e_success;
}


/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    decInfo->stego_image_fname = strstr(argv[2], ".bmp");
            
    if( (decInfo->stego_image_fname == NULL) || (strcmp(decInfo->stego_image_fname, ".bmp") != 0) )
    {
        printf(RED"./a.out: Decoding: ./a.out -d <.bmp file> [output file]\n"RESET);
        return e_failure;
    }
    else
    {
        decInfo->stego_image_fname = argv[2];
    }

    if (argc > 3)
    {
        char *token = strtok(argv[3], ".\n");
        strcpy(decInfo->secret_fname, token);
    }
    else
    {    
        printf(YELLOW"INFO: Output File not mentioned. Creating 'output' as default\n"RESET);
        strcpy(decInfo->secret_fname, "output");
    }

    return e_success;
}