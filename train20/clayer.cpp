#include "clayer.h"
#include <assert.h>
#include <thread>


CLayer::CLayer(int numNeur , int numPrevNeur) {
	assert(numNeur>0) ;
	for(int i = 0 ;i< numNeur ; ++ i ){
		CNeuron* neu = new CNeuron( numPrevNeur , i ) ;
		this->neurons.push_back(neu) ;
		neu = NULL ;
	}
    int p1 = (int)(numNeur * 0.333) ;
    int p2 = (int)(numNeur * 0.667) ;
    this->part0 = this->neurons.begin() ;
    this->part1 = this->part0 + p1 ;
    this->part2 = this->part0 + p2 ;

}


CLayer::~CLayer() {
	for( std::vector<CNeuron*>::iterator it = this->neurons.begin() ;
		it < this->neurons.end() ; ++ it )
	{
		/* code */
		delete *it ;
		*it = NULL ;
	}
	this->neurons.clear() ;
	this->thePrevLayer = NULL ;
	this->theNextLayer = NULL ;
}

int CLayer::getNumOfNeurons()  {
	return (int)this->neurons.size() ;
}

void CLayer::printeq(){
	for( std::vector<CNeuron*>::iterator it = neurons.begin() ;
		it < neurons.end() ; ++ it ){
		(*it)->printeq() ;
		printf( "\n" ) ;
	}
}

void CLayer::printOutput(){
	for( std::vector<CNeuron*>::iterator it = neurons.begin() ;
		it < neurons.end() ; ++ it ){
		printf( "%6.2f\n", (*it)->output_a ) ;
	}
}

void CLayer::calculatePartNeurons( int i ){
    printf("\n Error calculatePartNeurons() deprecated.\n ") ;
 /*
    std::vector<CNeuron*>::iterator it0 = this->part0 ;
    std::vector<CNeuron*>::iterator it1 = this->part1 ;
    if( i==1 || i==11 || i== 21 ){
        it0 = this->part1 ;
        it1 = this->part2 ;
    }else if(i==2 || i==12 || i==22 ){
        it0 = this->part2 ;
        it1 = this->neurons.end() ;
    }
    if( i<10 ){
        for( std::vector<CNeuron*>::iterator it = it0 ; it<it1 ; ++ it ){
            CNeuron* pn = *it ;
            pn->calculateOutput( this->thePrevLayer ) ;
        }
    }else if(i<20) {
        for( std::vector<CNeuron*>::iterator it = it0 ; it<it1 ; ++ it ){
            CNeuron* pn = *it ;
            pn->calculateHiddenLayerTheta( this->theNextLayer ) ;
        }
    }else if(i<30 ){
        for( std::vector<CNeuron*>::iterator it = it0 ; it<it1 ; ++ it ){
            CNeuron* pn = *it ;
            pn->recalculateWeightsAndBias( this->thePrevLayer , this->theStudyRate ) ;
        }
    }
*/
}

void computePart0( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(0) ;
}
void computePart1( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(1) ;
}
void computePart2( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(2) ;
}

void computePart10( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(10) ;
}
void computePart11( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(11) ;
}
void computePart12( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(12) ;
}

void computePart20( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(20) ;
}
void computePart21( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(21) ;
}
void computePart22( CLayer* layer )
{
  // do stuff...
    layer->calculatePartNeurons(22) ;
}

void CLayer::calculateAllNeuronsMT( CLayer* prevLyr ) {
    this->thePrevLayer = prevLyr ;

  std::thread thread0(computePart0,this);
  std::thread thread1(computePart1,this);
  std::thread thread2(computePart2,this);
  thread0.join();
  thread1.join();
  thread2.join();
}

void CLayer::calculateAllNeurons( CLayer* prevLyr ) {
    this->bestFitIndex = 0 ;
    this->bestFitPoss = -1 ;
    this->totalPoss = 0 ;
    int i = 0;
	for( std::vector<CNeuron*>::iterator it = neurons.begin() ;
		it < neurons.end() ; ++ it )
	{
		CNeuron* pn = *it ;
		pn->calculateOutput( prevLyr ) ;
        if( pn->output_a > this->bestFitPoss ){
            this->bestFitIndex = i ;
            this->bestFitPoss = pn->output_a ;
        }
        this->totalPoss = pn->output_a ;
		++ i ;
	}
}

void CLayer::calculateOutputLayerTheta( std::vector<double>& vy ) {
	int ny = (int)vy.size() ;
	int nn = (int)neurons.size() ;
	assert( ny == nn ) ;
	for(int i = 0 ; i<nn ; ++ i ){
		CNeuron* pn = this->neurons[i] ;
		pn->calculateOutputLayerTheta( vy[i] ) ;
	}
}
void CLayer::calculateHiddenLayerTheta( CLayer* nextLyr ) {
	int nn = (int)neurons.size() ;
	for(int i = 0 ; i<nn ; ++ i ){
		CNeuron* pn = this->neurons[i] ;
		pn->calculateHiddenLayerTheta( nextLyr ) ;
	}
}

void CLayer::calculateHiddenLayerThetaMT( CLayer* nextLyr ) {
    this->theNextLayer = nextLyr ;
	  std::thread thread0(computePart10,this);
      std::thread thread1(computePart11,this);
      std::thread thread2(computePart12,this);
      thread0.join();
      thread1.join();
      thread2.join();
}


//void CLayer::recalculateWeightsAndBias(CLayer* prevLayer , double studyRate , double alpha,double & alldelta ) {
//	for( std::vector<CNeuron*>::iterator it = this->neurons.begin() ;
//		it < this->neurons.end() ; ++ it ){
//		(*it)->recalculateWeightsAndBias( prevLayer , studyRate , alpha  ) ;
//		alldelta += (*it)->allDeltaSum ;
//	}
//
//}

void CLayer::recalculateWeightsAndBiasMT(CLayer* prevLayer , double studyRate ) {
    this->thePrevLayer = prevLayer ;
    this->theStudyRate = studyRate ;
	  std::thread thread0(computePart20,this);
      std::thread thread1(computePart21,this);
      std::thread thread2(computePart22,this);
      thread0.join();
      thread1.join();
      thread2.join();
}

int CLayer::getBestFitIndex( double& poss,  double & ttlposs ) {
    poss = this->bestFitPoss ;
    ttlposs = this->totalPoss ;
    return this->bestFitIndex;
}

