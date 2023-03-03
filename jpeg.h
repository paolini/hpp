/*
//
// jpeg.h
//
// Copyright (c) 1995-96 Jim Nelson.  Permission to distribute
// granted by the author.  No warranties are made on the fitness of this
// source code.
//
*/

#ifndef JPEG_H
#define JPEG_H


typedef short int BOOL;
typedef int WORD;


/* is this a JPEG File Interchange Format (JFIF) file? */
BOOL JpegFormatFound(istream &file);

/* one-shot-does-it-all function */
BOOL JpegReadDimensions(istream &file, WORD *height, WORD *width);

#endif

