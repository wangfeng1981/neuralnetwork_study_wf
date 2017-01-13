#include"netlayer.h"
#include <iostream>

#ifndef POOLLAYER_H
#define POOLLAYER_H


class PoolLayer:public NetLayer
{
    public:
        PoolLayer(int numx , int numy , int depth );
        virtual ~PoolLayer();
        virtual void computeForewardOutput(const NetLayer* prevLayer ) ;
        void print() ;

        bool hasWeightsAndBias(){return false;}
    protected:

    private:
};

#endif // POOLLAYER_H
