#include "cneuron.h"
#include <stdlib.h>
#include <assert.h>
#include "clayer.h"
#include <math.h>

extern int global_index_data ;
extern int global_index_layer ;

CNeuron::CNeuron ( int numOfWei , int nindex ){
    this->deltaWeightsSum = NULL ;
    this->deltaWeights = NULL ;
	if( numOfWei > 0 ){
        this->deltaWeightsSum = new double[numOfWei] ;
        this->deltaWeights = new double[numOfWei] ;
		for(int i = 0 ; i<numOfWei ; ++ i ){
            double r = 4 * ( ((double) rand() / (RAND_MAX)) - 0.5 ) ;
			this->weights.push_back(r)  ;
			this->deltaWeightsSum[i] = 0 ;
			this->deltaWeights[i] = 0 ;
		}
        double b = 4 * ( ((double) rand() / (RAND_MAX)) - 0.5 ) ;
		this->bias = b ;

	}else{
		this->bias = 0 ;
	}
	this->neuronIndexInLayer = nindex ;
	this->output_a = 0 ;
	this->zeta = 0 ;
	this->theta = 0 ;
	this->deltaBiasSum = 0 ;
	this->deltaBias = 0 ;
	this->deltaSumCount = 0 ;
}

CNeuron::~CNeuron() {
    if( this->deltaWeightsSum ){
        delete[] this->deltaWeightsSum ;
        this->deltaWeightsSum = NULL ;
    }
    if( this->deltaWeights ){
        delete[] this->deltaWeights ;
        this->deltaWeights = NULL ;
    }
	this->weights.clear() ;
}

void CNeuron::printeq(){
	int n = this->getNumOfWeights() ;
	if( n == 0 ){
		printf(" --input layer-- ") ;
	}else{
		printf(" sig(") ;
		for(int i = 0 ; i<n ; ++ i ){
			printf( " a%02d*%4.2f +" , i , weights[i] ) ;
		}
		printf(" %4.2f )" , bias ) ;
	}
}


int CNeuron::getNumOfWeights() {
	return (int)this->weights.size() ;
}



double CNeuron::calculateOutput( CLayer* prevLayer ) {
	int nw = this->getNumOfWeights() ;
	this->output_a = 0 ;
	this->zeta = 0 ;
	assert( prevLayer != NULL );
    int npn = prevLayer->getNumOfNeurons() ;
    assert(nw == npn) ;
    double z = 0 ;
    for(int i = 0; i<npn ; ++ i ){
        double a0 = prevLayer->neurons[i]->output_a ;
        double w = this->weights[i] ;
        double dz = w*a0 ;
        z += dz ;
    }
    z += this->bias ;
    double oa = this->sigmoid(z) ;
    this->output_a = oa ;
    this->zeta = z ;
    return this->output_a ;
}

void CNeuron::setInputLayerOutputValue( double val ) {
	this->output_a = val ;
}

void CNeuron::calculateOutputLayerTheta( double y ) {
	double c = output_a - y ;
	double a = this->output_a;
    double sp = a*(1.0-a) ;// this->sigmoid_prime(this->zeta) ;
	double t = c * sp ;
	this->theta = t ;//old
	this->theta = c ; //new cross entropy
}

void CNeuron::calculateHiddenLayerTheta( CLayer* nextLayer ) {

	int nw = (int)(nextLayer->neurons[0]->weights.size()) ;
	assert( this->neuronIndexInLayer < nw ) ;
    double a= this->output_a ;
    double sp = a*(1.0-a);// this->sigmoid_prime(this->zeta) ;
	double sumtw = 0 ;
	for( std::vector<CNeuron*>::iterator it = nextLayer->neurons.begin() ;
		it < nextLayer->neurons.end() ; ++ it ){
		double tlp1 = (*it)->theta ;
		double wlp1 = (*it)->weights[this->neuronIndexInLayer] ;
		sumtw += tlp1 * wlp1 ;
	}
	this->theta = sp * sumtw ;
}




void CNeuron::calculateDerivativeWeightsAndBias( CLayer* prevLayer , double studyRate , double alpha) {
	//bias
	++ this->deltaSumCount ;

	double t = this->theta ;
	double b = this->bias ;
	this->deltaBias =  -studyRate *t + alpha * this->deltaBias ;
	this->deltaBiasSum += this->deltaBias ;
	this->allDeltaSum = this->deltaBias ;
	//weights
	for( int i = 0 ; i<this->getNumOfWeights() ; ++ i ){
		double w = this->weights[i] ;
		CNeuron* neuronI = prevLayer->neurons[i] ;
		double prevLayerAi = neuronI->output_a ;
		this->deltaWeights[i] = - studyRate * prevLayerAi * this->theta + alpha * this->deltaWeights[i] ;
        this->deltaWeightsSum[i] += this->deltaWeights[i] ;
		this->allDeltaSum += this->deltaWeights[i] ;
	}
}

void CNeuron::updateWeightsAndBias() {
    assert( this->deltaSumCount > 0 ) ;
    int nw = this->getNumOfWeights() ;
    double tc = 1.0/this->deltaSumCount ;
    this->bias = this->bias + this->deltaBiasSum * tc ;
    this->deltaBiasSum = 0 ;

    for(int i = 0 ; i< nw ; ++i ){
        this->weights[i] = this->weights[i] + this->deltaWeightsSum[i] * tc ;
        this->deltaWeightsSum[i] = 0 ;
    }
    this->deltaSumCount = 0 ;
}


double CNeuron::sigmoid(double z) {
    return 1.0/(1.0+exp(-z))  ;
}
double CNeuron::sigmoid_prime(double z) {
    return this->sigmoid(z) * (1.0-this->sigmoid(z)) ;
}



