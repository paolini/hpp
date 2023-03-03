/*
//
// gif.h
//
// Copyright (c) 1995-96 Jim Nelson.  Permission to distribute
// granted by the author.  No warranties are made on the fitness of this
// source code.
//
*/

#ifndef GIF_H
#define GIF_H

typedef short int BOOL;
typedef int WORD;


/* GIF format detected? */
BOOL GifFormatFound(istream &file);

/* get image dimensions */
BOOL GifReadDimensions(istream &file, WORD *height, WORD *width);

#endif

