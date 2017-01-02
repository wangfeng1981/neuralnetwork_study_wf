
#include <stdio.h>
#include <string>
#include <vector>

#ifndef CNEURON_H_
#define CNEURON_H_

class CLayer ;

class CNeuron {
private:
	int neuronIndexInLayer ;


public :
	std::vector<double> weights;
	double bias ;
	double output_a ;
	double zeta ;
	double theta ;
	int deltaSumCount ;
	double* deltaWeightsSum ;
	double deltaBiasSum ;
	double allDeltaSum ;
	double deltaBias ;
	double* deltaWeights ;

	CNeuron(int numOfWei , int nindex ) ;
	~CNeuron() ;
	void printeq() ;
	double calculateOutput( CLayer * prevLayer ) ;
	void setInputLayerOutputValue( double val ) ;
	int getNumOfWeights() ;
	void calculateOutputLayerTheta( double y ) ;
	void calculateHiddenLayerTheta( CLayer* nextLayer ) ;
	void calculateDerivativeWeightsAndBias( CLayer* prevLayer , double studyRate , double alpha  );
    void updateWeightsAndBias() ;

    double sigmoid(double z) ;
    double sigmoid_prime(double z) ;

} ;
#endif
