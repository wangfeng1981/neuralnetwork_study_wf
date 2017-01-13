#include "inputlayer.h"
#include "wImage.h"
#include <assert.h>
#include <iostream>

using namespace std;

InputLayer::InputLayer(const int numx,const int numy,const int numdep)
    :NetLayer(numx,numy,numdep)
{
}

InputLayer::~InputLayer()
{
    cout<<"inputlayer out."<<endl;
}

bool InputLayer::loadFromPng( const std::string& filepath ) {
    wImage image(filepath.c_str()) ;
    assert( this->getOutXnum() == image.getCols() ) ;
    assert( this->getOutYnum() == image.getRows() ) ;
    assert( this->getOutDepnum() ==3 ) ;

    int r,g,b ;
    for(int irow = 0 ; irow < image.getRows() ; ++irow ){
        for(int icol=0 ; icol < image.getCols() ; ++icol ){
            image.getRGB( irow , icol , r,g,b) ;
            this->setOutputAt(icol,irow,0,r) ;// here make sure output ? or activ?
            this->setOutputAt(icol,irow,1,g) ;
            this->setOutputAt(icol,irow,2,b) ;
        }
    }
    return true;
}

void InputLayer::print(){
    cout<<"Input Layer print()."<<endl;
}

void InputLayer::setInputLayerActivationValueAt(int ix,int iy,int idep , double value) {
    this->mpOutputActi->setValueAt(ix,iy,idep,value) ;
}
void InputLayer::setInputLayer1DActivationValueAt(int i ,double value) {
    this->mpOutputActi->set1DValueAt(i,value) ;
}
