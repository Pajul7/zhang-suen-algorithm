#include "bmp_utils.h"

BMP_IMG importBMP(const char * filepath){

    FILE * f = fopen(filepath, "rb");
    if (f == NULL){
        fprintf(stderr, "Error : file import failed.");
        exit(1);
    }
    
    unsigned char signature[2];
    fread(signature, 1, 2, f );
    printf("file Signature : %s\n", signature);


    if ( signature[0] != 'B' || signature[1] != 'M'){
        fprintf(stderr, "Error : file not a BMP image.\n");
        exit(1);
    }
    
    fseek(f,0,SEEK_SET);

    unsigned char * fileInfo = malloc(54);
    fread(fileInfo, 1, 54, f );

    
    
    //printf("file Signature : %c%c\n", fileInfo[0],fileInfo[1]);
    
    __uint32_t filesize         = *(__uint32_t *) &fileInfo[2];
    __uint32_t headerSize       = *(__uint32_t *) &fileInfo[14];
    __uint32_t width            = *(__uint32_t *) &fileInfo[18];
    __uint32_t height           = *(__uint32_t *) &fileInfo[22];
    __uint16_t BitsPerPixel     = *(__uint16_t *) &fileInfo[28];
    //__uint16_t BytesPerPixel    = BitsPerPixel >> 3;
    
    
    
    int rowSize = ((BitsPerPixel * width + 31) / 32) * 4;
    int pixArraySize = rowSize * height;
    
    unsigned char * pixArray = malloc(pixArraySize);
    memset(pixArray,0,pixArraySize);
    fread(pixArray, 1,pixArraySize,f);

    fclose(f);

    BMP_IMG res = {
        .pixArray       = pixArray,
        .fileSize       = (int) filesize,
        .width          = (int) width,
        .height         = (int) height,
        .bitsPerPixel   = (int) BitsPerPixel,
        .pixArraySize   = (int) pixArraySize,
        .rowSize        = (int) rowSize
    };

    printf("Header Size : %d\n", headerSize);
    printf("Image dimensions : %d by %d\n", width,height);
    printf("Image size : %d B\n", filesize);
    printf("color depth : %d bpp\n", BitsPerPixel);
    printf("Image row size : %d B\n",rowSize);
    printf("Image array size : %d B\n",pixArraySize);

    printf("size of :%d\n",(int) sizeof(__uint8_t));

    free(fileInfo);
    return res;
}

Color getPixel(BMP_IMG * img, int x, int y){
    unsigned char r,g,b,a;
    int y_offset = (img->height > 0) ? img->height - y - 1 : y;

    if (img->bitsPerPixel == 32){
        __uint32_t pixel = *(__uint32_t *) (img->pixArray + (y_offset) * img->rowSize + x * (img->bitsPerPixel >> 3));

        b = (pixel)         & 0xFF;
        g = (pixel >> 8)    & 0xFF;
        r = (pixel >> 16)   & 0xFF;
        a = (pixel >> 24)   & 0xFF;

    }
    else if (img->bitsPerPixel == 24){
        unsigned char * pixel =  img->pixArray + (y_offset) * img->rowSize + x * 3;

        b = pixel[0];
        g = pixel[1];
        r = pixel[2];
        a = 255;
    }else {
        fprintf(stderr, "Error : Invalid bpp.\n");

        return NULL_COLOR;
    }

    Color res =  {r,g,b,a};
    return res;
}

void putPixel(BMP_IMG * img, int x, int y, Color c){
    int y_offset = (img->height > 0) ? img->height - y - 1 : y;

    unsigned char * pixelRowPtr = (img->pixArray) + (y_offset * img->rowSize);

    if (img->bitsPerPixel == 32) {
        __uint32_t newPix =  (c.a << 24) | (c.r << 16) | (c.g << 8) | (c.b);

        //printf("%X\n", newPix);

        memcpy( pixelRowPtr + x * 4,&newPix , sizeof(__uint32_t));

    } else if (img->bitsPerPixel == 24){

        //printf("putting pixel in a 24bits setting. at x = %d, and y = %d.\n", x, y);

        pixelRowPtr[ x*3 ] = c.b;
        pixelRowPtr[ x*3 + 1] = c.g;
        pixelRowPtr[ x*3 + 2] = c.r;

    }
    //printf("get pixel 0 0 blue : %d\n", getPixel(img,0,0).b );
}

void printBMP(BMP_IMG * img){

    for (int y = 0 ; y < img->height; y++){
        for (int x = 0; x < img->width; x++){

            if ( getPixel(img,x,y).b==255){
                printf(".");
            }else{
                printf("#");
            }

        }
        printf("\n");
    }
}

BMP_IMG * newBMP( int height, int width, int bpp ){

    if (bpp != 32 && bpp != 24){
        fprintf(stderr, "Error : bpp formats other than 32b and 24b are not supported.\n");
        exit(EXIT_FAILURE);
    }
    int rowSize = ((bpp * width + 31) / 32) * 4;

    int pixArraySize =  rowSize * height;

    unsigned char * pixArray = malloc(pixArraySize); 
    memset(pixArray,0,pixArraySize);

    BMP_IMG * res = malloc(sizeof(BMP_IMG));

    (*res).pixArray       = pixArray;
    (*res).fileSize       = (int) pixArraySize + 54;
    (*res).width          = (int) width;
    (*res).height         = (int) height;
    (*res).bitsPerPixel   = (int) bpp;
    (*res).pixArraySize   = (int) pixArraySize;
    (*res).rowSize        = (int) rowSize;

    return res;
}

void destroyBMP(BMP_IMG * img){
    free(img->pixArray);
    img->pixArray = NULL;
}

void exportBMP(BMP_IMG img, const char * filename){

    // HEADER (14B)
    char sig[2] = "BM";
    __uint32_t fileSize = img.fileSize;
    __uint16_t reserved1 = 0;
    __uint16_t reserved2 = 0;
    __uint32_t pixArrayOffset = 54;

    // Windows BITMAPINFOHEADER (40B)

    __uint32_t headerSize = 40;
    __int32_t width = img.width;
    __int32_t height = img.height;
    __uint16_t colorPlanes = 1;
    __uint16_t bpp = img.bitsPerPixel; // 24 or 32
    __uint32_t compression = 0;
    __uint32_t imgSizeDummy = 0;
    __uint32_t hppm = 1; // DUMMY
    __uint32_t vppm = 1; // DUMMY
    __uint32_t ncolor = 0;
    __uint32_t importantColors = 0;

    FILE * f = fopen(filename, "wb+");

    printf("File size is :%d\n", fileSize);

    fwrite(&sig,             sizeof(char)        , 2,f); // 2 B
    fwrite(&fileSize,        sizeof(__uint32_t)  , 1,f); // 6 B
    fwrite(&reserved1,       sizeof(__uint16_t)  , 1,f); // 8 B
    fwrite(&reserved2,       sizeof(__uint16_t)  , 1,f); // 10 B
    fwrite(&pixArrayOffset,  sizeof(__uint32_t)  , 1,f); // 14 B

    fwrite(&headerSize,      sizeof(__uint32_t)  , 1,f); // 18 B
    fwrite(&width,           sizeof(__int32_t)   , 1,f); // 22 B
    fwrite(&height,          sizeof(__int32_t)   , 1,f); // 26 B
    fwrite(&colorPlanes,     sizeof(__uint16_t)  , 1,f); // 28 B
    fwrite(&bpp,             sizeof(__uint16_t)  , 1,f); // 30 B
    fwrite(&compression,     sizeof(__uint32_t)  , 1,f); // 34 B
    fwrite(&imgSizeDummy,    sizeof(__uint32_t)  , 1,f); // 38 B
    fwrite(&hppm,            sizeof(__uint32_t)  , 1,f); // 42 B
    fwrite(&vppm,            sizeof(__uint32_t)  , 1,f); // 46 B
    fwrite(&ncolor,          sizeof(__uint32_t)  , 1,f); // 50 B
    fwrite(&importantColors, sizeof(__uint32_t)  , 1,f); // 54 B
    fwrite(img.pixArray,     img.pixArraySize    , 1,f);

    fclose(f);
}

void clearColor(BMP_IMG * img, Color c){
    int Bpp = (img->bitsPerPixel == 32) ? 4 : 3;
    
    unsigned char insideColor[Bpp];
    insideColor[0] = c.b;
    insideColor[1] = c.g;
    insideColor[2] = c.r;
    if (Bpp == 4) {
        insideColor[3] = c.a;
    }

    unsigned char * row = malloc(img->rowSize);
    memset(row,0xFF,img->rowSize);

    for (int x = 0 ; x < img->width; x++){
        printf("writing color at x = %d\n", x);
        printf("rox index : %d\n",row + (x * sizeof(unsigned char) * Bpp));
        printf("pixel has size : %d\n",sizeof(unsigned char)*Bpp);
        memcpy(row + (x * sizeof(unsigned char) * Bpp),insideColor, sizeof(unsigned char)*Bpp );
    }
    for (int y = 0 ; y < img->height; y++){

        int y_offset = (img->height > 0) ? img->height - y - 1 : y;
        
        memcpy(img->pixArray + y_offset *img->rowSize , row ,img->rowSize );
    }
    free(row);

}

bool isSameColor(Color a , Color b){
    return (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a);
}

BMP_IMG * duplicateBMP(BMP_IMG * src){

    BMP_IMG * res = malloc(sizeof(BMP_IMG));
    
    (*res).pixArray       = src->pixArray;
    (*res).fileSize       = (int) src->pixArraySize + 54;
    (*res).width          = (int) src->width;
    (*res).height         = (int) src->height;
    (*res).bitsPerPixel   = (int) src->bitsPerPixel;
    (*res).pixArraySize   = (int) src->pixArraySize;
    (*res).rowSize        = (int) src->rowSize;
    (*res).rowSize        = (int) src->rowSize;
    
    unsigned char * pixArray = malloc(src->pixArraySize); 
    memcpy(pixArray,src->pixArray, src->pixArraySize);

    (*res).pixArray = pixArray;

    return res;
}