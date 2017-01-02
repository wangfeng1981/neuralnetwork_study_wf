
#include "cneuron.h"
#include <vector>


#ifndef CLAYER_H_
#define CLAYER_H_

class CLayer {


public :
	std::vector<CNeuron*> neurons ;

	/// numPrevLayerNeurons eq 0 means input layer. numNeurons must greater than 0.
	CLayer(int numNeur , int numPrevNeur ) ;
	~CLayer() ;
	int getNumOfNeurons() ;
	void printeq() ;
	void printOutput() ;
	void calculateAllNeurons( CLayer* prevLyr );
	void calculateOutputLayerTheta( std::vector<double>& vy ) ;
	void calculateHiddenLayerTheta( CLayer* nextLyr ) ;
	//void recalculateWeightsAndBias(CLayer* prevLayer , double studyRate,double alpha ,double& alldelta) ;


	void calculatePartNeurons(int i) ;//i=0,1,2 for output , 10,11,12 for hidden theta,  20,21,22 for weights and bias

    void calculateAllNeuronsMT( CLayer* prevLyr );
    void calculateHiddenLayerThetaMT( CLayer* nextLyr ) ;
	void recalculateWeightsAndBiasMT(CLayer* prevLayer , double studyRate  ) ;
    int getBestFitIndex( double& poss  ,  double & ttlposs ) ;
private :
    int nOK ;
    CLayer* thePrevLayer ;
    CLayer* theNextLayer ;
    double theStudyRate ;
    std::vector<CNeuron*>::iterator part0 ;
    std::vector<CNeuron*>::iterator part1 ;
    std::vector<CNeuron*>::iterator part2 ;

    int bestFitIndex ;
    double bestFitPoss ;
    double totalPoss ;
} ;
#endif
