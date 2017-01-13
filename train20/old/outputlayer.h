#include "fclayer.h"
#include "array3d.h"

#ifndef OUTPUTLAYER_H
#define OUTPUTLAYER_H


class OutputLayer:public FCLayer
{
    public:
        OutputLayer(const int input1DSize ,const int output1DSize);
        virtual ~OutputLayer();
        virtual void computeAndSumPartialDerivativeOfWeightsAndBias(
                                                                    const NetLayer* prevlayer,
                                                                    const NetLayer* nextlayer,
                                                                    const int outputTargetIndex) ;
        virtual bool hasWeightsAndBias(){return true;}
        virtual void print() ;
        void printMSECost(int targetIndex) ;
        double computeMSE(int targetIndex) ;
    protected:


    private:
};

#endif // OUTPUTLAYER_H
