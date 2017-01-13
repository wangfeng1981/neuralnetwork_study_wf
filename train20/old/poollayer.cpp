#include "poollayer.h"
#include <iostream>
#include <math.h>
#include <assert.h>

using namespace std;

PoolLayer::PoolLayer(int numx , int numy , int depth):NetLayer(numx,numy,depth)
{

}

PoolLayer::~PoolLayer()
{
    cout<<"pool layer out."<<endl;
}

void PoolLayer::computeForewardOutput(const NetLayer* prevLayer ) {
    assert(prevLayer) ;
    int insize = prevLayer->getOutXnum() ;
    int outsize = this->getOutXnum() ;
    assert( outsize*2 == insize ) ;

    this->zeroActiva() ;
    this->zeroOutput() ;

    for(int idep = 0 ; idep < this->getOutDepnum() ; ++ idep ){
        for(int ix=0 ; ix < this->getOutXnum() ; ++ix  ){
            for(int iy=0 ; iy<this->getOutYnum() ; ++iy ) {
                //2x2 box
                double v0 = prevLayer->getOutputAt(ix*2+0,iy*2+0,idep) ;
                double v1 = prevLayer->getOutputAt(ix*2+1,iy*2+0,idep) ;
                double v2 = prevLayer->getOutputAt(ix*2+0,iy*2+1,idep) ;
                double v3 = prevLayer->getOutputAt(ix*2+1,iy*2+1,idep) ;
                double out1 = max(v0,max(v1,max(v2,v3))) ;
                this->setOutputAt(ix,iy,idep,out1);//here out? or acti?
            }
        }
    }
}

void PoolLayer::print(){
    cout<<"Pool Layer print()."<<endl;
}
