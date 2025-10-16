#include "ZhangSuen.h"

bool isBorder(BMP_IMG * img, int x , int y){
    int ly = y;

    if(x>=img->width || y>= img->height){
        fprintf(stderr,"Warning : Border check out of bounds.\n");
    }

    if (img->height < 0){
        ly = -y;
    }

    return (x==0 || ly == 0 || x >= img->width-1 || ly >= img->height-1 );
}

Color zhang_suenPixel(BMP_IMG * source, int x, int y, bool * hasPixelChanged, bool isFirstPass){
    Color pixColor = getPixel(source, x,y);
    int blackNeighbors = 0;
    int whiteBlackTransitions = 0;

    bool cond1 = (!isBorder(source, x, y)) && isSameColor(pixColor,BLACK);
    //printf("%d %d\n", x, y);
    
    if(cond1){
        //printf("isBorder(source, x, y) : true\n");
    }else{
        //printf("NOT A BORDER\n");
    }
    
    if (cond1){

        Color sequence[9] = {
            getPixel(source, x   , y+1), // P2
            getPixel(source, x+1, y+1), // P3
            getPixel(source, x+1, y  ), // P4
            getPixel(source, x+1, y-1), // P5
            getPixel(source, x  , y-1), // P6
            getPixel(source, x-1, y-1), // P7
            getPixel(source, x-1, y  ), // P8
            getPixel(source, x-1, y+1), // P9
            getPixel(source, x  , y+1), // P2
        };

        Color precColor = sequence[0];
        for(int i = 0; i < 9; i++){
            if (isSameColor(sequence[i], BLACK)){
                blackNeighbors++;
                if (isSameColor(precColor, WHITE)){
                    whiteBlackTransitions++;
                    precColor = BLACK;
                }
            }
            else {
                precColor = WHITE;
            }
        }
        
        bool cond2 = blackNeighbors >=2 && blackNeighbors <= 6;
        bool cond3 = whiteBlackTransitions == 1;
        bool cond4 = false;
        bool cond5 = false;


        if (isFirstPass) {
            cond4 = isSameColor(sequence[0],WHITE) || isSameColor(sequence[2],WHITE) || isSameColor(sequence[4],WHITE);
            cond5 = isSameColor(sequence[2],WHITE) || isSameColor(sequence[4],WHITE) || isSameColor(sequence[6],WHITE);
        } else {
            cond4 = isSameColor(sequence[0],WHITE) || isSameColor(sequence[2],WHITE) || isSameColor(sequence[6],WHITE);
            cond5 = isSameColor(sequence[0],WHITE) || isSameColor(sequence[4],WHITE) || isSameColor(sequence[6],WHITE);
        }




        if ( cond2 && cond3 && cond4 && cond5){
            //printf("conditions ok , white\n");
            *hasPixelChanged = true;
            return WHITE;
        } else {
            //printf("default case\n");
            *hasPixelChanged = false;
            return pixColor;
        }
    }
    else{
        //printf("border, skip.\n");
        *hasPixelChanged = false;
        return pixColor;
    }
        
}




BMP_IMG * zhang_suen(BMP_IMG * source, int maxIter){

    BMP_IMG * src = duplicateBMP(source);

    BMP_IMG * res = newBMP(source->height, source->width, source->bitsPerPixel);
    BMP_IMG * tmp = NULL;

    const int MAX_ITERATIONS = maxIter;

    int nAffected1 = -1;
    int nAffected2 = -1;
    bool hasPixelChanged;

    int i = 0;


    
    while (nAffected1 + nAffected2 != 0 && i < MAX_ITERATIONS){
        
        printf("ZS Iteration %d...\n", i);
        printf("Writing from %X to %X\n",src, res);
        nAffected2 = 0;
        nAffected1 = 0;

        hasPixelChanged = false;

        for (int y = 0; y < res->height; y++){
            for (int x = 0; x < res->width; x++){
                Color c = zhang_suenPixel(src, x, y, &hasPixelChanged, true);
                if (hasPixelChanged) {
                    nAffected1++;
                }
                putPixel(res, x, y , c );
            }
        }

        // SECOND PART

        for (int y = 0; y < res->height; y++){
            for (int x = 0; x < res->width; x++){
                Color c = zhang_suenPixel(src, x, y, &hasPixelChanged, false);
                if (hasPixelChanged) {
                    nAffected2++;
                }
                putPixel(res, x, y , c );
            }
        }
        
        i++;

        if (nAffected1 + nAffected2 != 0 && i < MAX_ITERATIONS){
            tmp = res;
            res = src;
            src = tmp;
        }

        printf("ZS algo pass finished. %d pixels affected. Part 1 : %d, Part 2 : %d.\n", nAffected1+nAffected2, nAffected1, nAffected2);
    }
    printf("Returning buffer %X.\n",res);
    destroyBMP(src);
    return res;
}



int main(){
    
    bool c = isSameColor(BLACK, BLACK);
    if (c){
        printf("True\n");
    }else{
        printf("False\n");
    }

    BMP_IMG source = importBMP("smile.bmp");

    printf("STARTING ZS\n");
    BMP_IMG * res = zhang_suen(&source, 10);

    //BMP_IMG * res2 = zhang_suen(res, 5);

    exportBMP(*res, "res.bmp");
    //exportBMP(*res2, "res2.bmp");



    destroyBMP(&source);
    destroyBMP(res);
    //destroyBMP(res2);

    //printBMP(res);

    return EXIT_SUCCESS;
}

