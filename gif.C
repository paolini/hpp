/*
// gif.C
//
// Transposition of gif.c by Jim Nelson.
// by Emanuele Paolini.
/*

/*
//
// gif.c
//
// GIF (Graphic Interchange Format) support functions
//
// Copyright (c) 1995-96 Jim Nelson.  Permission to distribute
// granted by the author.  No warranties are made on the fitness of this
// source code.
//
*/

#include <cstring>
#include <iostream>

using namespace std;

typedef short int BOOL;
typedef char BYTE;
typedef int WORD;
const int TRUE=1;
const int FALSE=0;

#define MAKE_WORD(h, l)     (WORD) ((((WORD) (h)) << 8) | (((WORD) (l)) & 0x00FF))

BOOL GifFormatFound(istream &file)
{
    BYTE header[8];

    /* move to BOF */
    if(!file.seekg(0L))
    {
      cerr<<"unable to seek to start of GIF file";
      return FALSE;
    }

    /* read first six bytes, looking for GIF header + version info */
    if(!file.read(header, 6))
    {
      cerr<<"could not read GIF image file header";
      return FALSE;
    }

    /* is this a GIF file? */
    if((memcmp(header, "GIF87a", 6) == 0) || (memcmp(header, "GIF89a", 6) == 0))
    {
        return TRUE;
    }

    /* not a GIF file */
    return FALSE;
}

BOOL GifReadDimensions(istream &file, WORD *height, WORD *width)
{
    BYTE hi;
    BYTE lo;

    /* move to the image size position in the file header */
    if(!file.seekg(6))
    {
      cerr<<"unable to seek to start of GIF file";
      return FALSE;
    }

    /* read the width, byte by byte */
    /* this gets around machine endian problems while retaining the */
    /* fact that GIF uses little-endian notation */
    if(!file.read(&lo, 1))
    {
        return FALSE;
    }

    if(! file.read(&hi,1))
    {
      return FALSE;
    }
    
    *width = MAKE_WORD(hi, lo);
    
    /* read the height, byte by byte */
    if(!file.read(&lo, 1))
    {
        return FALSE;
    }

    if(!file.read(&hi,1))
    {
        return FALSE;
    }

    *height = MAKE_WORD(hi, lo);

    return TRUE;
}

