#include "wImage.h"
#include <assert.h>
#include "lodepng.h"
#include <iostream>
#include <cmath>
#include <cstdlib>

using namespace std;

wImage::wImage(const char* imagepath):mRows(0),mCols(0)
{
    unsigned int trow(0) , tcol(0) ;
    unsigned error = lodepng::decode(this->data, tcol , trow , imagepath );
    this->mRows = (int)trow ;
    this->mCols = (int)tcol ;
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

wImage::wImage(int nrow,int ncol):mRows(nrow),mCols(ncol)
{
    this->data = vector<unsigned char>( mRows * mCols * 4 , 0 ) ;
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
void wImage::getRGB1d(const int index1d , int& r,int& g,int& b) {
    assert( index1d >=0 && index1d < this->mCols * this->mRows ) ;
    r = this->data[ index1d*4 + 0 ] ;
    g = this->data[ index1d*4 + 1 ] ;
    b = this->data[ index1d*4 + 2 ] ;
}
void wImage::setRGB(const int row,const int col , int r,int g , int b) {
    assert(row >=0 && row < this->mRows ) ;
    assert(col >=0 && col < this->mCols ) ;

    r = min(r,255) ;
    g = min(g,255) ;
    b = min(b,255) ;

    //image 0,1,2,3 left top [row:0,col:0] pixel red,green,blue,alpha
    //image 4,5,6,7 left top [row:0,col:1] pixel red,green,blue,alpha
    this->data[ (row*this->mCols+col)*4 + 0 ] = (unsigned char)r ;
    this->data[ (row*this->mCols+col)*4 + 1 ] = (unsigned char)g ;
    this->data[ (row*this->mCols+col)*4 + 2 ] = (unsigned char)b ;
    this->data[ (row*this->mCols+col)*4 + 3 ] = 255 ;
}
void wImage::setRGB1d(const int index1d , int r,int g , int b) {

    r = min(r,255) ;
    g = min(g,255) ;
    b = min(b,255) ;

    this->data[ index1d*4 + 0 ] = (unsigned char)r ;
    this->data[ index1d*4 + 1 ] = (unsigned char)g ;
    this->data[ index1d*4 + 2 ] = (unsigned char)b ;
    this->data[ index1d*4 + 3 ] = 255 ;
}

int wImage::getRows() {
    return this->mRows ;
}
int wImage::getCols() {
    return this->mCols;
}

wImage wImage::gaussBlur( float sig ) {
    wImage newImage( this->mRows , this->mCols ) ;
    int kernelradius = (int)ceil(3*sig) ;
    int ksize = kernelradius * 2 + 1;
    vector<float> kernel( ksize*ksize , 0 ) ;
    float fm = 0 ;
    for(int i = 0 ; i< ksize ; ++ i ){
        for(int j = 0 ; j<ksize ; ++ j ){
            int ii = i-kernelradius ;
            int jj = j-kernelradius ;
            kernel[i*ksize+j] = 1.f/(2.f*3.1416f*sig*sig)*exp(-(ii*ii+jj*jj) / 2.f / (sig*sig) ) ;
            fm += kernel[i*ksize+j] ;
        }
    }
    for(int i = 0 ; i< ksize ; ++ i ){
        for(int j = 0 ; j<ksize ; ++ j ){
            kernel[i*ksize+j] = kernel[i*ksize+j]/fm ;
        }
    }

    for(int irow = 0 ; irow < this->getRows() ; ++irow ){
        for(int icol = 0 ; icol <this->getCols() ; ++ icol ){
            int r0,g0,b0 ;
            float newr(0) , newg(0) , newb(0) ;
            this->getRGB( irow , icol , r0,g0,b0) ;
            for(int ii = -kernelradius ; ii<= kernelradius ; ++ ii ){
                for( int jj = -kernelradius ; jj<=kernelradius ; ++ jj ){
                    int newrow = ii + irow ;
                    int newcol = jj + icol ;
                    int r,g,b ;
                    if( newrow >= 0 && newrow < this->getRows()
                       && newcol >=0 && newcol < this->getCols() ){
                        this->getRGB( newrow , newcol , r,g,b ) ;
                    }else{
                        r = r0 ;
                        g = g0 ;
                        b = b0 ;
                    }
                    int ikernel = (ii+kernelradius) * ksize + (jj+kernelradius ) ;
                    newr += kernel[ikernel] * r ;
                    newg += kernel[ikernel] * g ;
                    newb += kernel[ikernel] * b ;
                }
            }
            newImage.setRGB( irow , icol , round(newr) , round(newg) , round(newb) ) ;
        }
    }
    return newImage ;
}

void wImage::saveToFile(const string filepath ) {
    unsigned error = lodepng::encode( filepath , this->data , this->mCols , this->mRows );
    if(error) std::cout << "encode error " << error << ": " << lodepng_error_text(error) << std::endl;
}


//============================================




wImaged::wImaged(const char* imagepath):mRows(0),mCols(0)
{
    unsigned int trow(0) , tcol(0) ;
    vector<unsigned char> tdata ;
    unsigned error = lodepng::decode( tdata , tcol , trow , imagepath );
    if(error){
         std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
         exit(1) ;
    }
    this->mRows = (int)trow ;
    this->mCols = (int)tcol ;
    for(size_t i = 0 ; i<tdata.size() ; ++ i ){
        this->data.push_back( static_cast<double>(tdata[i]) ) ;
    }

}

wImaged::wImaged(int nrow,int ncol):mRows(nrow),mCols(ncol)
{
    this->data = vector<double>( mRows * mCols * 4 , 0 ) ;
}


wImaged::~wImaged()
{
}
void wImaged::getRGB(const int row,const int col , float& r,float& g,float& b){
    double r1,g1,b1 ;
    this->getRGB(row,col , r1,g1,b1) ;
    r = (float)r1 ;
    g = (float)g1 ;
    b = (float)b1 ;
}
void wImaged::getRGB(const int row,const int col , double& r,double& g,double& b) {
    assert(row >=0 && row < this->mRows ) ;
    assert(col >=0 && col < this->mCols ) ;

    //image 0,1,2,3 left top [row:0,col:0] pixel red,green,blue,alpha
    //image 4,5,6,7 left top [row:0,col:1] pixel red,green,blue,alpha
    r = this->data[ (row*this->mCols+col)*4 + 0 ] ;
    g = this->data[ (row*this->mCols+col)*4 + 1 ] ;
    b = this->data[ (row*this->mCols+col)*4 + 2 ] ;
}
void wImaged::getRGB1d(const int index1d , double& r,double& g,double& b) {
    assert( index1d >=0 && index1d < this->mCols * this->mRows ) ;
    r = this->data[ index1d*4 + 0 ] ;
    g = this->data[ index1d*4 + 1 ] ;
    b = this->data[ index1d*4 + 2 ] ;
}
void wImaged::setRGB(const int row,const int col , double r,double g , double b) {
    assert(row >=0 && row < this->mRows ) ;
    assert(col >=0 && col < this->mCols ) ;

    //image 0,1,2,3 left top [row:0,col:0] pixel red,green,blue,alpha
    //image 4,5,6,7 left top [row:0,col:1] pixel red,green,blue,alpha
    this->data[ (row*this->mCols+col)*4 + 0 ] =  r ;
    this->data[ (row*this->mCols+col)*4 + 1 ] =  g ;
    this->data[ (row*this->mCols+col)*4 + 2 ] =  b ;
    this->data[ (row*this->mCols+col)*4 + 3 ] = 255 ;
}
void wImaged::setRGB1d(const int index1d , double  r,double g , double  b) {

    this->data[ index1d*4 + 0 ] =  r ;
    this->data[ index1d*4 + 1 ] =  g ;
    this->data[ index1d*4 + 2 ] =  b ;
    this->data[ index1d*4 + 3 ] = 255 ;
}

int wImaged::getRows() {
    return this->mRows ;
}
int wImaged::getCols() {
    return this->mCols;
}

wImaged wImaged::gaussBlur( double sig ) {
    wImaged newImage( this->mRows , this->mCols ) ;
    int kernelradius = (int)ceil(3*sig) ;
    int ksize = kernelradius * 2 + 1;
    vector<double> kernel( ksize*ksize , 0 ) ;
    double fm = 0 ;
    for(int i = 0 ; i< ksize ; ++ i ){
        for(int j = 0 ; j<ksize ; ++ j ){
            int ii = i-kernelradius ;
            int jj = j-kernelradius ;
            kernel[i*ksize+j] = 1./(2.*3.1416*sig*sig)*exp(-(ii*ii+jj*jj) / 2. / (sig*sig) ) ;
            fm += kernel[i*ksize+j] ;
        }
    }
    for(int i = 0 ; i< ksize ; ++ i ){
        for(int j = 0 ; j<ksize ; ++ j ){

            kernel[i*ksize+j] = kernel[i*ksize+j]/fm ;
        }
    }

    for(int irow = 0 ; irow < this->getRows() ; ++irow ){
        for(int icol = 0 ; icol <this->getCols() ; ++ icol ){
            double r0,g0,b0 ;
            double newr(0) , newg(0) , newb(0) ;
            this->getRGB( irow , icol , r0,g0,b0) ;
            for(int ii = -kernelradius ; ii<= kernelradius ; ++ ii ){
                for( int jj = -kernelradius ; jj<=kernelradius ; ++ jj ){
                    int newrow = ii + irow ;
                    int newcol = jj + icol ;
                    double r,g,b ;
                    if( newrow >= 0 && newrow < this->getRows()
                       && newcol >=0 && newcol < this->getCols() ){
                        this->getRGB( newrow , newcol , r,g,b ) ;
                    }else{
                        r = r0 ;
                        g = g0 ;
                        b = b0 ;
                    }
                    int ikernel = (ii+kernelradius) * ksize + (jj+kernelradius ) ;
                    newr += kernel[ikernel] * r ;
                    newg += kernel[ikernel] * g ;
                    newb += kernel[ikernel] * b ;
                }
            }
            newImage.setRGB( irow , icol , newr , newg , newb ) ;
        }
    }
    return newImage ;
}

void wImaged::saveToFile(const string filepath ) {
    vector<unsigned char> tdata ;
    for(size_t i = 0 ; i<this->data.size() ; ++ i ){
        tdata.push_back( static_cast<unsigned char>(this->data[i]) ) ;
    }
    unsigned error = lodepng::encode( filepath , tdata , this->mCols , this->mRows );
    if(error) std::cout << "encode error " << error << ": " << lodepng_error_text(error) << std::endl;
}
