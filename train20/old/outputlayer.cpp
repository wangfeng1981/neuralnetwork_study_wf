#include "outputlayer.h"
#include <assert.h>
#include  <iomanip>


OutputLayer::OutputLayer(const int input1DSize ,const int output1DSize)
    :FCLayer(input1DSize,output1DSize)
{

}

OutputLayer::~OutputLayer()
{
}


void OutputLayer::computeAndSumPartialDerivativeOfWeightsAndBias(
                                                                 const NetLayer* prevlayer,
                                                                 const NetLayer* nextlayer,
                                                                 const int outputTargetIndex) {
    assert(prevlayer) ;
    assert(nextlayer==0) ;
    assert( prevlayer->get1DOutputNum() == this->mpWeights->getNumx() ) ;

    for(int inn = 0 ; inn < this->get1DOutputNum() ; ++inn ){
        double a_i_currlayer = this->get1DActivaAt(inn) ;
        double y_i = 0.0 ;
        if(inn==outputTargetIndex){
            y_i = 1.0 ;
        }
        /// delta_i = pd(C)/pd(z_i)
        double delta_i = (a_i_currlayer - y_i);
        this->set1DDeltaAt(inn,delta_i) ;

        for(int jw = 0 ; jw < prevlayer->get1DOutputNum() ; ++ jw  ){

            double a_j_prevlayer = prevlayer->get1DActivaAt(jw) ;
            double oldvalue = this->mpPartialDerivativeOfWeights->getValueAt(jw,inn,0) ;
            double newvalue = oldvalue + this->get1DDeltaAt(inn) * a_j_prevlayer ;
            this->mpPartialDerivativeOfWeights->setValueAt(jw,inn,0,newvalue) ;
        }//end for jw

        double old_bias = this->mpPartialDerivativeOfBias->get1DValueAt(inn) ;
        double new_bias = old_bias + this->get1DDeltaAt(inn) ;
        this->mpPartialDerivativeOfBias->set1DValueAt(inn , new_bias) ;

    }// end for inn

    ++ this->mNumberOfSamples ;
}

void OutputLayer::print(){
    cout<<"Output Layer print()."<<endl;
    cout<<"class poss"<<endl ;
    for(int i = 0 ; i< this->get1DOutputNum() ; ++ i ) {
        cout<<i<<"\t"<<setiosflags(ios::fixed)<<setprecision(4)<<this->get1DOutputAt(i)<<endl ;
    }
}

void OutputLayer::printMSECost(int targetIndex) {
    double cost = 0.0 ;
    for(int i = 0 ; i<this->get1DOutputNum() ; ++ i ){
        if( i== targetIndex ){
            double c = 1.0 - this->get1DActivaAt(i)  ;
            cost += c*c ;
        }else {
            double c = 0.0 - this->get1DActivaAt(i)  ;
            cost += c*c ;
        }
    }
    cout<<"mse:"<<cost<<endl ;
}

double OutputLayer::computeMSE(int targetIndex) {
    double cost = 0.0 ;
    for(int i = 0 ; i<this->get1DOutputNum() ; ++ i ){
        if( i== targetIndex ){
            double c = 1.0 - this->get1DActivaAt(i)  ;
            cost += c*c ;
        }else {
            double c = 0.0 - this->get1DActivaAt(i)  ;
            cost += c*c ;
        }
    }
    return cost ;
}
