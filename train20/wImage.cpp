#include "wImage.h"
#include <assert.h>
#include "lodepng.h"
#include <iostream>


using namespace std;

wImage::wImage(const char* imagepath):mRows(0),mCols(0)
{
    unsigned error = lodepng::decode(this->data, this->mCols, this->mRows , imagepath );
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

wImage::~wImage()
{
}

void wImage::getRGB(const int row,const int col , int& r,int& g,int& b) {
    assert(row >=0 && row < this->mRows ) ;
    assert(col >=0 && col < this->mCols ) ;

    //image 0,1,2,3 left top [row:0,col:0] pixel red,green,blue,alpha
    //image 4,5,6,7 left top [row:0,col:1] pixel red,green,blue,alpha
    r = this->data[ (row*this->mCols+col)*4 + 0 ] ;
    g = this->data[ (row*this->mCols+col)*4 + 1 ] ;
    b = this->data[ (row*this->mCols+col)*4 + 2 ] ;
}

unsigned int wImage::getRows() {
    return this->mRows ;
}
unsigned int wImage::getCols() {
    return this->mCols;
}
