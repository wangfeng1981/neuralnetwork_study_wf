#ifndef CONVNETWORK2_H
#define CONVNETWORK2_H
#include "array3d.h"
#include <vector>
#include "wImage.h"

using namespace std;
// IputLayer -> ConvLayer -> PoolLayer -> FullConnLayer -> OutputLayer

enum CNNLayerType{
    CNNLayerTypeNone ,
    CNNLayerTypeInput ,
    CNNLayerTypeConv ,
    CNNLayerTypePool ,
    CNNLayerTypeFullConn ,
    CNNLayerTypeOutput
};

class CNNLayer {
    public:
        CNNLayer(){ mLayerType = CNNLayerTypeNone ; }
        virtual ~CNNLayer() {} ;
        CNNLayerType getLayerType() { return mLayerType; }
        virtual void saveToDisk(char* filepath) {} ;
    protected:
        CNNLayerType mLayerType ;
};


class CNNInputLayer:public CNNLayer {
    public:
        Array3d* mdata3d ;
        CNNInputLayer(const int nx,const int ny,const int nz) ;
        virtual ~CNNInputLayer() ;
        void resetDataFromWImage(wImage& wimg,double scale) ;
        virtual void saveToDisk(char* filepath)   ;
};

class CNNConvLayer:public CNNLayer {
    public:
        vector<Array3d*>  mfilter3dVector ;

        vector<Array3d*> mDeltaFilterWeightsSum3dVector ;
        int      mSampleNumber ;

        Array3d*   moutput3d ;
        Array3d*   mactiv3d ;
        Array3d*   mdelta3d ;
        CNNConvLayer(const int inx,const int iny,const int inz,
                     const int fnx,const int fny,const int nfilter ) ;
        virtual ~CNNConvLayer() ;
        void computeAndSumFilterDeltaWeights(const Array3d* prevLayerActivArray) ;
        void updateFilterWeights(const double studyRate,const double momentum) ;
        virtual void saveToDisk(char* filepath)   ;
};

class CNNPoolLayer:public CNNLayer{
    public:
        Array3d*   minputweight3d ;
        Array3d*   mactiv3d ;
        Array3d*   mdelta3d ;
        CNNPoolLayer(const int inx,const int iny,const int inz) ;
        virtual ~CNNPoolLayer() ;
        virtual void saveToDisk(char* filepath)   ;
};


class CNNFullConnLayer:public CNNLayer {
    public:
        Array3d* mweight2d ;
        Array3d* mDeltaWeightSum2d ;
        Array3d* mDeltaBiasSum1d ;
        int      mSampleNumber ;
        Array3d* mbias1d ;
        Array3d* mactiv1d ;
        Array3d* mdelta1d;
        CNNFullConnLayer(const int insize,const int outsize) ;
        virtual ~CNNFullConnLayer() ;
        void computeAndSumDeltaWeightsAndBias(const Array3d* prevLayerActivArray) ;
        void updateWeightsAndBias(double studyRate,double momentum) ;
        virtual void saveToDisk(char* filepath)  ;
};

class CNNOutputLayer : public CNNFullConnLayer {
    public:
        CNNOutputLayer(const int insize,const int outsize) ;
        virtual ~CNNOutputLayer() ;
        void printBestFit() ;
        int getBestFit() ;
        double computeMSE(int target) ;
};




class ConvNetwork2
{
    public:
        ConvNetwork2();
        virtual ~ConvNetwork2();

        void doforward(CNNInputLayer* inputLayer , CNNConvLayer* convLayer ) ;
        void doforward(CNNInputLayer* inputLayer , CNNFullConnLayer* fcLayer ) ;
        void doforward(CNNConvLayer*  convLayer  , CNNPoolLayer* poolLayer ) ;
        void doforward(CNNPoolLayer*  poolLayer  , CNNFullConnLayer* fcLayer ) ;
        void doforward(CNNFullConnLayer* fcLayer , CNNFullConnLayer* outputLayer) ;
        void doforward(CNNPoolLayer*  poolLayer  , CNNConvLayer* convLayer ) ;

        void doforwardA2B(CNNLayer* layerA,CNNLayer* layerB) ;
        void backpropagate_deltaA2B(CNNLayer* layerA,CNNLayer* layerB);

        void backpropagate_delta(CNNOutputLayer* outputLayer,int targetIndex) ;
        void backpropagate_delta(CNNFullConnLayer* outputLayer, CNNFullConnLayer* fcLayer ) ;
        void backpropagate_delta(CNNFullConnLayer* fcLayer,   CNNPoolLayer* poolLayer ) ;
        void backpropagate_delta(CNNPoolLayer*   poolLayer,   CNNConvLayer* convLayer ) ;
        void backpropagate_delta(CNNConvLayer*   convLayer,   CNNPoolLayer* poolLayer ) ;

        vector<CNNLayer*> mLayerVector ;
        void doforward() ;
        void dobackpropagation(int targetIndex) ;
        void computeAndSumPartialDerivForWeightsAndBias() ;
        void updateWeightsAndBias(const double studyRate ,const double momentum ) ;

        void saveToDisk(const char* filepath) ;

    protected:

    private:
};

#endif // CONVNETWORK2_H
