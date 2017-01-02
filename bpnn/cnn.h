
#include "cneuron.h"
#include "clayer.h"
#include <vector>
#include "wdata.h"

#ifndef CNN_H_
#define CNN_H_



class CNn {

public :
	CNn( std::vector<int>& nInLayers ) ;
	CNn( const char* weightsAndBiasFilepath ) ;
	~CNn() ;
	void printeq() ;
	void train( std::vector<WData>& data , int repeat , double studyRate ) ;
	int classify( WData& data , double & outPossibility) ;

private:
	std::vector<CLayer*> layers ;
    void shuffleInputDatas(std::vector<WData>& data) ;
    void saveWeightsAndBiasToFile(int irepeat , double err , int ngood , int nbad) ;

} ;

#endif
