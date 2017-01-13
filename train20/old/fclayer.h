#include "netlayer.h"
#include "array3d.h"

#ifndef FCLAYER_H
#define FCLAYER_H


class FCLayer: public NetLayer
{
    public:
        FCLayer(const int input1DSize ,const int output1DSize );
        virtual ~FCLayer();
        virtual void computeForewardOutput(const NetLayer* prevLayer ) ;
        void print();

        virtual bool hasWeightsAndBias(){return true;}

        virtual void computeAndSumPartialDerivativeOfWeightsAndBias(const NetLayer* prevlayer,const NetLayer* nextlayer,const int outputTargetIndex) ;
        virtual void updateWeightsAndBiasForBatch(const double studyRate,const double momentum ) ;
        virtual double getWeightsAt(int ix,int iy,int idep) const ;

    protected:
        Array3d* mpWeights ;
        Array3d* mpBias ;

        int mNumberOfSamples ;
        Array3d* mpPartialDerivativeOfWeights ;
        Array3d* mpPartialDerivativeOfBias ;

    private:
};

#endif // FCLAYER_H
