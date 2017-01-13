#include "array3d.h"
#include "netlayer.h"

#ifndef CONVLAYER_H
#define CONVLAYER_H


class ConvLayer: public NetLayer
{
    public:
        ConvLayer(const int filterSize ,const int filterDepth,
                  const int outputXnum ,const int outputYnum ,const int outputDepnum );
        virtual ~ConvLayer();
        virtual void computeForewardOutput(const NetLayer* prevLayer ) ;
        void print();

        bool hasWeightsAndBias(){return true;}
        void updateLayerTheta(NetLayer* nextLayer) ;

    protected:
        double getWeights(int ix,int iy,int idep,int ifilter) ;
        double getBias(int ifilter) ;
        int getFilterNum() const ;

    private:
        Array3d** mpFilterWeights ;
        double* mpBias ;
};

#endif // CONVLAYER_H
