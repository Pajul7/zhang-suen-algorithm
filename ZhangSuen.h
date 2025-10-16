#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "lib/bmp_utils.h"



bool isBorder(BMP_IMG * img, int x , int y);

Color zhang_suenPixel(BMP_IMG * source, int x, int y, bool * hasPixelChanged, bool isFirstPass);

BMP_IMG * zhang_suen(BMP_IMG * source, int maxIter);