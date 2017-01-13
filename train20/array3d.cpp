#include "array3d.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include "wftools.h"

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
