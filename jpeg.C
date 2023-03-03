/*
// jpeg.C
//
// Translate from jpeg.c by Jim Nelson
// by Emanuele Paolini
// 
*/


/*
//
// jpeg.c
//
// JPEG FIF (File Interchange Format) support functions
//
// Copyright (c) 1995-96 Jim Nelson.  Permission to distribute
// granted by the author.  No warranties are made on the fitness of this
// source code.
//
*/

/*
// a great deal of this code was ripped off wholesale from the Independent
// JPEG Group's sample source code, obtainable from any SimTel mirror under
// msdos/graphics/jpegsrc6.zip
*/

/*
// JPEG markers
*/
#define M_SOF0  0xC0        /* Start Of Frame N */
#define M_SOF1  0xC1		/* N indicates which compression process */
#define M_SOF2  0xC2		/* Only SOF0-SOF2 are now in common use */
#define M_SOF3  0xC3
#define M_SOF5  0xC5		/* NB: codes C4 and CC are NOT SOF markers */
#define M_SOF6  0xC6
#define M_SOF7  0xC7
#define M_SOF9  0xC9
#define M_SOF10 0xCA
#define M_SOF11 0xCB
#define M_SOF13 0xCD
#define M_SOF14 0xCE
#define M_SOF15 0xCF
#define M_SOI   0xD8		/* Start Of Image (beginning of datastream) */
#define M_EOI   0xD9		/* End Of Image (end of datastream) */
#define M_SOS   0xDA		/* Start Of Scan (begins compressed data) */
#define M_APP0  0xE0
#define M_COM   0xFE		/* COMment */

#include <cstring>
#include <iostream>

using namespace std;

typedef short int BOOL;
typedef int WORD;
typedef char BYTE;
const int TRUE=1;
const int FALSE=0;

#define MAKE_WORD(h, l)     (WORD) ((((WORD) (h)) << 8) | (((WORD) (l)) & 0x00FF))


BOOL JpegReadByte(istream &file, BYTE *b)
{
    int i;

    if(!file.get(*b))
    {
      cerr<<"unable to read byte from JFIF file";
      return FALSE;
    }
    return TRUE;
}

BOOL JpegReadWord(istream &file, WORD *w)
{
    int hi;
    int lo;

    /* words are kept in MSB format */
    if(!file.get((char&)hi))
    {
      cerr<<"unable to read high byte from JFIF file";
      return FALSE;
    }

    if(!file.get((char&)lo))
    {
      cerr<<"unable to read low byte from JFIF file";
      return FALSE;
    }
    
    *w = MAKE_WORD(hi, lo);

    return TRUE;
}

BOOL JpegFirstMarker(istream &file)
{
    unsigned char flag;
    unsigned char marker;

    /* move to the beginning of the file */
    if(!file.seekg(0))
    {
      cerr<<"unable to seek to start of JFIF file";
      return FALSE;
    }
    /* look for the start of image marker */
    if(JpegReadByte(file, (char *)&flag) == FALSE)
    {
        return FALSE;
    }

    if(JpegReadByte(file, (char *)&marker) == FALSE)
    {
        return FALSE;
    }

    /* start of image? */
    if((flag != 0xFF) || (marker != M_SOI))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL JpegNextMarker(istream &file, unsigned char *marker)
{
    unsigned char flag;
    BOOL ok;

    /* move file pointer to next 0xFF flag */
    while((ok = JpegReadByte(file, (char *)&flag)) == TRUE)
    {
        if(flag == 0xFF)
        {
            break;
        }
    }

    if(ok == FALSE)
    {
        return FALSE;
    }

    /* extra 0xFF flags are legal as padding, so move past them */
    while((ok = JpegReadByte(file, (char *)marker)) == TRUE)
    {
        if(*marker != 0xFF)
        {
            break;
        }
    }

    /* exit condition really depends if a good marker was found */
    return ok;
}

BOOL JpegFormatFound(istream &file)
{
    unsigned char marker;
    char signature[8];

    if(JpegFirstMarker(file) == FALSE)
    {
        return FALSE;
    }

    if(JpegNextMarker(file, &marker) == FALSE)
    {
        return FALSE;
    }

    /* should see an APP0 marker */
    if(marker != M_APP0)
    {
        return FALSE;
    }

    /* file format is now pointing to JFIF header ... skip two bytes and */
    /* look for the signature */
    if(!file.seekg(2, istream::cur))
      {
	cerr<<"unable to seek to start of JFIF file";
        return FALSE;
      }
    
    if(!file.read(signature, 5))
      {
        cerr<<"unable to read JFIF signature from file";
        return FALSE;
      }

    /* it all comes down to the signature being present */
    return (strcmp(signature, "JFIF") == 0) ? TRUE : FALSE;
}

BOOL JpegReadDimensions(istream &file, WORD *height, WORD *width)
{
    unsigned char marker;

    /* make sure we can find the first marker */
    if(JpegFirstMarker(file) == FALSE)
    {
        return FALSE;
    }

    /* read file looking for SOF (start of frame) ... when it or */
    /* or SOS (start of scan, the compressed data) is reached, stop */
    while(JpegNextMarker(file, &marker) == TRUE)
    {
        /* if SOS, stop */
        if(marker == M_SOS)
        {
	  cerr<<"JFIF SOS marker found before SOF marker";
	  break;
        }

        /* if not SOF, continue */
        if((marker < M_SOF0) || (marker > M_SOF15))
        {
            continue;
        }

        /* start of frame found ... process the dimension information */
        /* seek past the next three bytes, useless for this application */
        if(!file.seekg(3, istream::cur))
        {
            return FALSE;
        }

        /* read the height and width and get outta here */
        if(JpegReadWord(file, height) == FALSE)
        {
            return FALSE;
        }

        if(JpegReadWord(file, width) == FALSE)
        {
            return FALSE;
        }

        return TRUE;
    }

    cerr<<"JFIF SOF marker not found";

    /* didn't find the SOF or found the SOS */
    return FALSE;
}

