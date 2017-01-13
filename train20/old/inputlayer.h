#include <string>
#include "netlayer.h"


#ifndef INPUTLAYER_H
#define INPUTLAYER_H


class InputLayer: public NetLayer
{
    public:
        InputLayer(const int numx,const int numy,const int numdep );
        virtual ~InputLayer();
        bool loadFromPng( const std::string& filepath ) ;
        void print() ;

        bool hasWeightsAndBias() {return false;}

        void setInputLayerActivationValueAt(int ix,int iy,int idep , double value) ;
        void setInputLayer1DActivationValueAt(int i ,double value) ;

    protected:

    private:
};

#endif // INPUTLAYER_H
