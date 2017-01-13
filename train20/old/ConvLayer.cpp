#include "ConvLayer.h"
#include <assert.h>
#include <math.h>
#include <string.h>
#include <iostream>

using namespace std;

ConvLayer::ConvLayer(const int filterSize ,const int filterDepth,
                  const int outputXnum ,const int outputYnum ,const int outputDepnum )
                  : NetLayer(outputXnum,outputYnum,outputDepnum)
{
    assert(filterSize>0) ;
    assert(filterDepth>0) ;

    this->mpFilterWeights = new Array3d*[outputDepnum] ;
    for(int ifil = 0 ; ifil < outputDepnum ; ++ifil ){
        this->mpFilterWeights[ifil] = new Array3d(filterSize,filterSize,filterDepth,true) ;
    }
    this->mpBias = new double[outputDepnum] ;
    memset(mpBias , 0 , outputDepnum * sizeof(double) ) ;
}

ConvLayer::~ConvLayer()
{
    if( this->mpBias ){
        delete[] this->mpBias ;
        this->mpBias = 0 ;
    }
    if( this->mpFilterWeights ){
        int nf = this->getFilterNum() ;
        for(int ifil = 0 ; ifil < nf ; ++ifil ){
            delete this->mpFilterWeights[ifil] ;
            this->mpFilterWeights[ifil] = 0 ;
        }
        delete [] this->mpFilterWeights ;
        this->mpFilterWeights = 0 ;
    }
    cout<<"convlayer out."<<endl;
}

double ConvLayer::getWeights(int ix,int iy,int idep,int ifilter) {
    return this->mpFilterWeights[ifilter]->getValueAt(ix,iy,idep);
}
double ConvLayer::getBias(int ifilter) {
    return this->mpBias[ifilter] ;
}

void ConvLayer::computeForewardOutput(const NetLayer* prevLayer ) {
    this->mpOutputActi->zero() ;
    this->mpOutputData->zero() ;

    const int ws = this->mpFilterWeights[0]->getNumx() ;
    const int ds = this->mpFilterWeights[0]->getNumdep() ;
    assert( prevLayer->getOutDepnum() == ds ) ;

    int outsize = ( prevLayer->getOutXnum() - ws)/1 + 1 ;
    assert( this->getOutXnum() == outsize ) ;

    for( int ifil = 0 ; ifil < this->getFilterNum() ; ++ifil ){
        for(int ix = 0 ; ix < this->getOutXnum() ; ++ix ){
            for(int iy = 0 ; iy < this->getOutYnum() ; ++iy ){
                //convolution calculate.
                double out1 = this->mpBias[ifil] ;
                for(int ix1 = 0 ; ix1 < ws ; ++ix1 ){
                    for(int iy1 = 0 ; iy1 < ws ; ++iy1 ){
                        for(int idep = 0 ; idep < ds ; ++idep ){
                            double w = this->mpFilterWeights[ifil]->getValueAt(ix1,iy1,idep) ;
                            double pixel = prevLayer->getOutputAt(ix+ix1,iy+iy1,idep) ;
                            out1 += w*pixel ;
                        }
                    }
                }
                //ReLU.
                out1 = max( 0.0 , out1) ;
                //write to output array3d.
                this->setOutputAt( ix,iy,ifil,out1) ;
            }
        }
    }
}

int ConvLayer::getFilterNum() const {
    return this->getOutDepnum() ;
}

void ConvLayer::print(){
    Array3d* filter = this->mpFilterWeights[0] ;
    cout<<"Conv Layer print().filter size:"
        <<filter->getNumx()<<"x"<<filter->getNumy()<<"x"<<filter->getNumdep()<<endl;
}

void ConvLayer::updateLayerTheta(NetLayer* nextLayer) {
}
