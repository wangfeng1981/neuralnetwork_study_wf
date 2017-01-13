#include "fclayer.h"
#include "wftools.h"
#include <assert.h>
#include <iostream>
#include  <iomanip>

using namespace std;

FCLayer::FCLayer(const int input1DSize ,const int output1DSize ):NetLayer(1,1,output1DSize),mNumberOfSamples(0)
{
    this->mpWeights = new Array3d( input1DSize , output1DSize , 1 , true ) ;
    this->mpBias = new Array3d(1,1,output1DSize,true) ;

    this->mNumberOfSamples = 0 ;
    this->mpPartialDerivativeOfBias = new Array3d(1,1,output1DSize,false) ;
    this->mpPartialDerivativeOfWeights = new Array3d(input1DSize,output1DSize,1,false) ;

}

FCLayer::~FCLayer()
{
    if( this->mpWeights ){
        delete this->mpWeights ;
        this->mpWeights = 0 ;
    }
    if( this->mpBias ){
        delete this->mpBias ;
        this->mpBias = 0 ;
    }
    if( mpPartialDerivativeOfBias ){
        delete mpPartialDerivativeOfBias ;
        mpPartialDerivativeOfBias = 0 ;
    }
    if( mpPartialDerivativeOfWeights ){
        delete mpPartialDerivativeOfWeights ;
        mpPartialDerivativeOfWeights = 0 ;
    }
    cout<<"fclayer out."<<endl;
}


void FCLayer::computeForewardOutput(const NetLayer* prevLayer ) {
    int insize0 = this->mpWeights->getNumx() ;
    int insize1 = prevLayer->get1DOutputNum() ;
    assert( insize0 == insize1 ) ;

    int nout = this->getOutDepnum() ;
    for( int iout = 0 ; iout < nout ; ++iout ){
        double out1 = this->mpBias->get1DValueAt(iout) ;
        for( int ii = 0 ; ii<insize0 ; ++ ii ){
            double w = this->mpWeights->getValueAt(ii,iout,0) ;
            double activ0 = prevLayer->get1DActivaAt(ii) ;
            out1 += w * activ0 ;
        }
        double sigout = wft_sigmoid( out1 ) ;
        this->setOutputAt( 0 ,0 ,iout, out1 );
        this->setActivaAt(0,0,iout,sigout) ;
    }
}

void FCLayer::print(){
    cout<<"Full Connect Layer print()."<<endl;
}


void FCLayer::computeAndSumPartialDerivativeOfWeightsAndBias(
                                                            const NetLayer* prevlayer,
                                                            const NetLayer* nextlayer,
                                                            const int outputTargetIndex) {
    assert( prevlayer ) ;
    assert( nextlayer ) ;

    for(int inn = 0 ; inn < this->get1DOutputNum() ; ++ inn ){
        double dfz_i = this->get1DActivaAt(inn) * ( 1.0-this->get1DActivaAt(inn) ) ;// df(z)/dz
        double sum_k = 0.0 ;
        for(int ik = 0 ; ik < nextlayer->get1DOutputNum() ; ++ ik ){
            double delta_ik = nextlayer->get1DDeltaAt(ik) ;
            double w_ik = nextlayer->getWeightsAt(inn , ik , 0) ;
            sum_k += delta_ik * w_ik ;
        }
        double delta_i = dfz_i * sum_k ;
        this->set1DDeltaAt(inn , delta_i) ;

        for(int jw = 0 ; jw < prevlayer->get1DOutputNum() ; ++jw  ){

            double a_j_prevlayer = prevlayer->get1DActivaAt(jw) ;
            double delta_i_currlayer = this->get1DDeltaAt(inn) ;
            double pd_w_ij = a_j_prevlayer * delta_i_currlayer ;
            double oldval = this->mpPartialDerivativeOfWeights->getValueAt(jw,inn,0) ;
            double newval = oldval + pd_w_ij ;
            this->mpPartialDerivativeOfWeights->setValueAt(jw,inn,0,newval) ;

        }

        double old_b_i = this->mpPartialDerivativeOfBias->get1DValueAt(inn) ;
        double new_b_i = old_b_i + this->get1DDeltaAt(inn) ;
        this->mpPartialDerivativeOfBias->set1DValueAt(inn,new_b_i) ;
    }

    ++this->mNumberOfSamples ;
}
void FCLayer::updateWeightsAndBiasForBatch(const double studyRate,const double momentum ) {
    assert(this->mNumberOfSamples>0) ;
    // updating weights and bias.
    for(int inn = 0 ; inn < this->get1DOutputNum() ; ++inn ){

        for(int jw = 0 ; jw < this->mpWeights->getNumx() ; ++ jw  ){
            double pd_wij_sum = this->mpPartialDerivativeOfWeights->getValueAt(jw,inn,0);
            double pd_wij = pd_wij_sum / this->mNumberOfSamples ;
            double old_wij = this->mpWeights->getValueAt(jw,inn,0) ;
            double new_wij = old_wij - studyRate * pd_wij ;
            this->mpWeights->setValueAt(jw,inn,0,new_wij) ;

        }//end for jw

        double pd_bias_sum = this->mpPartialDerivativeOfBias->get1DValueAt(inn) ;
        double pd_bias = pd_bias_sum / this->mNumberOfSamples ;
        double old_bias = this->mpBias->get1DValueAt(inn) ;
        double new_bias = old_bias - studyRate *  pd_bias ; // do not consider momentum.
        this->mpBias->set1DValueAt(inn , new_bias) ;
    }// end for inn
    // reset number of samples.
    this->mNumberOfSamples = 0 ;
    this->mpPartialDerivativeOfBias->zero() ;
    this->mpPartialDerivativeOfWeights->zero() ;
}

double FCLayer::getWeightsAt(int ix,int iy,int idep) const {
    return this->mpWeights->getValueAt(ix,iy,idep) ;
}
