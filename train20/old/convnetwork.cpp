#include "convnetwork.h"

ConvNetwork::ConvNetwork()
{

}

ConvNetwork::~ConvNetwork()
{
    for(unsigned int i = 0 ; i<mLayerVector.size() ; ++i ){
        if( mLayerVector[i] ){
            delete mLayerVector[i] ;
            mLayerVector[i] = 0 ;
        }
    }
}

int ConvNetwork::doForward(){
    for(int i = 1 ; i<mLayerVector.size() ; ++ i ){
        NetLayer* prevLayer = mLayerVector[i-1];
        NetLayer* currLayer = mLayerVector[i]  ;
        currLayer->computeForewardOutput(prevLayer);
    }
    NetLayer* outlayer = *(mLayerVector.end()-1) ;
    int nout = outlayer->get1DOutputNum() ;
    double poss = 0 ;
    int bestfit = 0 ;
    for(int i = 0 ; i<nout ; ++i ){
        if( poss < outlayer->get1DOutputAt(i) ){
            poss = outlayer->get1DOutputAt(i) ;
            bestfit = i ;
        }
    }
    return bestfit ;
}

void ConvNetwork::doTrain(const int targetIndex) {
    this->doForward() ;
    NetLayer* nextLayer = 0 ;
    for( vector<NetLayer*>::iterator it = mLayerVector.end()-1 ; it != mLayerVector.begin() ; --it ){
        NetLayer* currLayer = *it ;
        if( currLayer->hasWeightsAndBias() ){
            currLayer->computeAndSumPartialDerivativeOfWeightsAndBias( *(it-1) , nextLayer , targetIndex) ;
            nextLayer = currLayer ;
        }
    }
}

void ConvNetwork::finishBatch() {
    for( vector<NetLayer*>::iterator it = mLayerVector.begin()+1 ; it != mLayerVector.end() ; ++it ){
        NetLayer* currLayer = *it ;
        currLayer->updateWeightsAndBiasForBatch(0.1,0) ;
    }
}



