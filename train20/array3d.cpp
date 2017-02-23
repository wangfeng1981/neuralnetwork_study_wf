#include "array3d.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include "wftools.h"
#include <stdio.h>
#include "lodepng.h"
#include <iostream>
#include <vector>

Array3d::Array3d(const int numx ,const int numy ,const int numd ,const bool fillGaussRand)
{
    assert(numx>0);
    assert(numy>0);
    assert(numd>0);

    mNumdep = numd ;
    mNumx = numx ;
    mNumy = numy ;
    mNxy = numx * numy ;

    this->mpData = new double[numx*numy*numd] ;
    memset( mpData , 0 , numx*numy*numd*sizeof(double) ) ;

    int n = mNxy * numd ;
    double sc = sqrt( 1.0 / n ) ;
    if( fillGaussRand ){
        for(int idep =0 ; idep< mNumdep ; ++idep ){
            for(int iy = 0 ; iy<mNumy ; ++ iy ){
                for(int ix = 0 ; ix<mNumx ; ++ix ){
                    mpData[idep*mNxy + iy * numx + ix ] = wft_gaussrand(0.0,sc)  ;
                }
            }
        }
    }
}

Array3d::~Array3d()
{
    if(mpData){
        delete[] mpData ;
        mpData = 0 ;
    }
}

double Array3d::getValueAt(int ix,int iy , int idep ) const {
    return mpData[idep*mNxy + mNumx * iy + ix ] ;
}
void Array3d::setValueAt(int ix,int iy ,int idep, double newValue) {
    mpData[idep*mNxy + mNumx * iy + ix ] = newValue ;
}
 int Array3d::getNumx() const {
    return mNumx ;
}
 int Array3d::getNumy() const{
    return mNumy ;
}
 int Array3d::getNumdep() const{
    return mNumdep ;
}

void Array3d::zero() {
    memset( mpData , 0 , mNumx*mNumy*mNumdep*sizeof(double) ) ;
}
int Array3d::get1DValueNum() const {
    return mNumdep * mNumx * mNumy ;
}
double Array3d::get1DValueAt(int i) const {
    return mpData[i] ;
}

void Array3d::set1DValueAt(int i ,double newval) {
    mpData[i] = newval ;
}

void Array3d::saveToDisk(char* filepath) {
    FILE* pf = fopen( filepath , "w" ) ;
    assert(pf) ;

    fprintf(pf, "%d %d %d\n" , this->getNumx() , this->getNumy() , this->getNumdep() ) ;
    for(int iz = 0 ; iz<this->getNumdep() ; ++iz ){
        for(int iy = 0 ; iy <this->getNumy() ; ++ iy ) {
            for(int ix = 0 ; ix<this->getNumx() ; ++ ix ){
                fprintf(pf,"%8.4f " , this->getValueAt(ix,iy,iz) ) ;
            }
            fprintf(pf,"\n") ;
        }
    }

    fclose(pf) ;
    pf = 0 ;
}

void Array3d::saveToFilePtr(FILE* pf) {
    assert(pf) ;
    fprintf(pf, "%d %d %d\n" , this->getNumx() , this->getNumy() , this->getNumdep() ) ;
    for(int iz = 0 ; iz<this->getNumdep() ; ++iz ){
        for(int iy = 0 ; iy <this->getNumy() ; ++ iy ) {
            for(int ix = 0 ; ix<this->getNumx() ; ++ ix ){
                fprintf(pf,"%8.4f " , this->getValueAt(ix,iy,iz) ) ;
            }
            fprintf(pf,"\n") ;
        }
    }
}

void updateMinMax(double val,double& minval,double& maxval){
    if( val > maxval ){
        maxval = val ;
    }
    if( val < minval ){
        minval = val ;
    }
}

void Array3d::saveToPngFile(const char* filepath) {
    int nband = this->getNumdep() ;

    vector<unsigned char> image ;
    double minval = 99999 ;
    double maxval = -99999 ;
    for(int iy = 0 ; iy < this->getNumy() ; ++ iy ){
        for(int ix = 0 ; ix<this->getNumy() ; ++ ix ){
            double val = this->getValueAt(ix,iy,0) ;
            updateMinMax(val,minval,maxval) ;
            if( nband >=3 ){
                double val1 = this->getValueAt(ix,iy,1) ;
                double val2 = this->getValueAt(ix,iy,2) ;
                updateMinMax(val1,minval,maxval) ;
                updateMinMax(val2,minval,maxval) ;
            }
        }
    }
    double d = maxval - minval ;
    for(int iy = 0 ; iy < this->getNumy() ; ++ iy ){
        for(int ix = 0 ; ix<this->getNumy() ; ++ ix ){
            double val = this->getValueAt(ix,iy,0) ;
            double val1 = val ;
            double val2 = val ;
            if( nband >=3 ){
                val1 = this->getValueAt(ix,iy,1) ;
                val2 = this->getValueAt(ix,iy,2) ;
            }
            unsigned char ival(0),ival1(0),ival2(0) ;
            if(d>0){
                ival = (unsigned char)((val-minval)/d * 255);
                ival1 = (unsigned char)((val1-minval)/d * 255);
                ival2 = (unsigned char)((val2-minval)/d * 255);
            }
            image.push_back(ival) ;
            image.push_back(ival1) ;
            image.push_back(ival2) ;
            image.push_back(255) ;
        }
    }
    int width = this->getNumx() ;
    int height = this->getNumy() ;
    unsigned error = lodepng::encode(filepath, image, width, height);
    //if there's an error, display it
    if(error) std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;

}
