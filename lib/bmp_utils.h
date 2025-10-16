#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbit.h>

#ifndef BMP_utils_H
#define BMP_utils_H

typedef struct BMP_IMG {
    unsigned char * pixArray;
    int fileSize;
    int width;
    int height;
    int bitsPerPixel;
    int pixArraySize;
    int rowSize;
} BMP_IMG;

typedef struct Color{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

static const Color NULL_COLOR = {0,0,0,0};
static const Color BLACK = {0  ,0  ,0  ,255};
static const Color WHITE = {255,255,255,255};
static const Color RED =   {255,0  ,0  ,255};
static const Color GREEN = {0  ,255,0  ,255};
static const Color BLUE =  {0  ,0  ,255,255};
static const Color MAGENTA =   {255,0  ,255,255};
static const Color YELLOW =    {255,255,0  ,255};
static const Color CYAN =      {0  ,255,255,255};


BMP_IMG importBMP(const char * filepath);
Color getPixel(BMP_IMG * img, int x, int y);
void putPixel(BMP_IMG * img, int x, int y, Color c);
void printBMP(BMP_IMG * img);
BMP_IMG * newBMP( int height, int width, int bpp );
BMP_IMG * duplicateBMP(BMP_IMG * src);
void destroyBMP(BMP_IMG * img);
void exportBMP(BMP_IMG img, const char * filename);
void clearColor(BMP_IMG * img, Color c);
bool isSameColor(Color a , Color b);


#endif