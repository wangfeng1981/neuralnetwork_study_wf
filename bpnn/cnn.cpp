#include "cnn.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include "wftools.h"
#include <time.h>

extern int global_index_data;
extern int global_index_layer;

CNn::CNn( std::vector<int>& nInLayers ) {
	assert( nInLayers.size() > 2 ) ;

	for( std::vector<int>::iterator it = nInLayers.begin() ; it < nInLayers.end() ; ++ it ){
		int num1 = *it ;

		if( it == nInLayers.begin() ){
			this->layers.push_back( new CLayer(num1 , 0) ) ;
		}else{
			std::vector<int>::iterator prevIt = it-1 ;
			int num0 = *prevIt ;
			this->layers.push_back( new CLayer(num1 ,num0 ) ) ;
		}
	}
}

CNn::CNn( const char* weightsAndBiasFilepath ) {
    std::ifstream infile( weightsAndBiasFilepath ) ;
    int il = 0 ;
    std::string line ;
    std::vector<int> nInLayers ;
    int lstart = 3 ;
    int ilayer = 1 ;
    int ineu = 0 ;
    while( std::getline(infile , line) ){
        if( il==1 ){
            std::cout<<line<<endl;
            nInLayers = wft_split2i(line , ' ') ;
            for( unsigned int i = 0 ; i<nInLayers.size(); ++i ){
                if(i==0){
                    this->layers.push_back(new CLayer(nInLayers[i] , 0)) ;
                }else{
                    this->layers.push_back(new CLayer(nInLayers[i] , nInLayers[i-1] ) ) ;
                }
            }
        }
        if( il >= lstart ){
            if( ilayer >= nInLayers.size() ) break ;
            CLayer* layer = this->layers[ilayer] ;
            if( ineu < layer->neurons.size() ) {
                CNeuron* neu = layer->neurons[ineu] ;
                std::vector<double> wbarr = wft_split2d( line , ' ') ;
                for(int iw = 0 ; iw <wbarr.size()-1 ; ++ iw ){
                    neu->weights[iw] = wbarr[iw] ;
                }
                neu->bias = wbarr[wbarr.size()-1];
                ++ ineu ;
            }else{
                ++ilayer ;
                ineu = 0 ;
            }
        }
        ++ il ;
    }
}

CNn::~CNn() {

	if( this->layers.size() > 0 ){
		for( std::vector<CLayer*>::iterator it = this->layers.begin() ; it < this->layers.end(); ++it ) {
		    delete *it ;
		    *it = NULL ;
		}
		this->layers.clear() ;
	}
}

void CNn::printeq() {
	printf("\n*neur-network:\n") ;
	int ilayer = 0 ;
	for( std::vector<CLayer*>::iterator itlayer = this->layers.begin() ;
		itlayer < this->layers.end() ; ++ itlayer ){
		printf("layer %d :\n" , ilayer ) ;
		(*itlayer)->printeq() ;
		++ ilayer ;
	}
}

void CNn::shuffleInputDatas(std::vector<WData>& data) {
    int num = data.size() ;
    for(int i = 0 ; i<num ; ++i ){
        WData temp = data[i] ;
        int rIndex = (int) (  (double)rand() / RAND_MAX * num )  ;
        if( rIndex >= num ){
            printf("*** *** Exception %d\n" , rIndex) ;
            rIndex = num-1 ;
        }
        assert(rIndex < num) ;
        data[i] = data[rIndex] ;
        data[rIndex] = temp ;
    }
}

void CNn::train(  std::vector<WData>& data , int repeat , double studyRate ) {
	std::vector<CLayer*>::iterator inlayer =  layers.begin() ;
	std::vector<CLayer*>::iterator outlayer = layers.end()-1 ;
	int outN = (*outlayer)->getNumOfNeurons() ;
    int numGoodFit = 0 ;
    int numBadFit = 0 ;
    int nData = (int)data.size();
    double biggestError = 0 ;
    int biggestErrorDataIndex = -1 ;
    double alldeltaChanged = 0 ;
    int numEachBatch = nData/20 ;
    if(numEachBatch==0){
        numEachBatch = 1 ;
    }
    time_t timeSave = 0 ;
    time(&timeSave) ;

	for( int irepeat = 0 ; irepeat < repeat ; ++ irepeat ){
        double totalErr = 0 ;
        global_index_data = 0 ;
        this->shuffleInputDatas(data) ;
        numGoodFit = 0;
        numBadFit = 0 ;
        biggestError = 0 ;
        biggestErrorDataIndex = -1 ;
        int biggestErrorRealY = 0 ;
        int biggestErrorGuestY = 0 ;
        alldeltaChanged = 0 ;
        int ibatch = 0 ;
        int numbatch = 20 ;
		for( std::vector<WData>::iterator idata = data.begin() ;
			idata < data.end() ; ++ idata  ) {

            int numNeu = (*inlayer)->getNumOfNeurons() ;
            for(int i = 0 ; i<numNeu ; ++ i ){
                CNeuron* pNeu = (*inlayer)->neurons[i] ;
                double invalue = (*idata).getDataAt(i) ;
                pNeu->setInputLayerOutputValue( invalue ) ;
            }
            int numlyr = (int)this->layers.size() ;
            for( int i = 1 ; i<numlyr ; ++ i ){
                CLayer* prevLyr = this->layers[i-1] ;
                CLayer* currLyr = this->layers[i] ;
                //currLyr->calculateAllNeurons( prevLyr ) ;
                global_index_layer = i ;
                currLyr->calculateAllNeurons( prevLyr ) ;
            }
            //total error
            double err1 = 0 ;
            for( int i = 0 ; i<outN ; ++ i ){
                double er =(*outlayer)->neurons[i]->output_a - (*idata).vy[i];
                er = 0.5 * er * er ;
                err1 += er ;
            }

            totalErr += err1 ;
            double fitPoss = 0 ;
            double ttPoss = 0 ;
            int fitIndex = (*outlayer)->getBestFitIndex( fitPoss,ttPoss ) ;
            if(fitIndex == (*idata).getY() ){
                ++ numGoodFit ;
            }else{
                ++ numBadFit ;
            }

            if( err1 > biggestError ){
                biggestError = err1 ;
                biggestErrorDataIndex = (*idata).originIndex ;
                biggestErrorRealY = (*idata).getY() ;
                biggestErrorGuestY = fitIndex  ;
            }

            //back update theta
            (*outlayer)->calculateOutputLayerTheta( (*idata).vy ) ;
            for( std::vector<CLayer*>::iterator iback = outlayer-1 ;
                iback != inlayer ; -- iback  ){
                CLayer* nextlyr = (*(iback+1)) ;
                (*iback)->calculateHiddenLayerTheta(nextlyr) ;
            }

            //biggest error origin data give 10 times study rate.
            double tempStudyRate = studyRate ;
            if( biggestErrorDataIndex == (*idata).originIndex  ){
                //tempStudyRate = studyRate*2 ;
            }

            //reset bias and weights
            for( int i = 1 ; i<numlyr ; ++ i ){
                CLayer* prevLyr = this->layers[i-1] ;
                CLayer* currLyr = this->layers[i] ;
                global_index_layer = i ;
                //currLyr->recalculateWeightsAndBias( prevLyr , tempStudyRate , 0.9 , alldeltaChanged ) ;
                int numn = currLyr->getNumOfNeurons() ;
                for(int ine  = 0 ; ine  <numn ; ++ine  ){
                    CNeuron* neu = currLyr->neurons[ine] ;
                    neu->calculateDerivativeWeightsAndBias(prevLyr , tempStudyRate , 0.9 ) ;
                    alldeltaChanged += neu->allDeltaSum ;
                }
            }

            ++ ibatch ;
            ++ global_index_data ;
            if(ibatch==numbatch ||  global_index_data == nData - 1 ){
                for( int i = 1 ; i<numlyr ; ++ i ){
                    CLayer* currLyr = this->layers[i] ;
                    int numn = currLyr->getNumOfNeurons() ;
                    for(int ine  = 0 ; ine  <numn ; ++ine  ){
                        CNeuron* neu = currLyr->neurons[ine] ;
                        neu->updateWeightsAndBias() ;
                    }
                }
                ibatch = 0 ;
            }

            if( global_index_data == nData-1 ){
                printf("r %d err %8.4f good %d bad %d sumd %8.4f bige %d re %d gu %d\n" , irepeat, totalErr,
                       numGoodFit , numBadFit , alldeltaChanged ,biggestErrorDataIndex
                       ,biggestErrorRealY,biggestErrorGuestY );
            }
		}//idata

		time_t timeUsed = 0 ;
		time(&timeUsed) ;
        time_t timedelta = timeUsed - timeSave ;
        if( timedelta > 600 ){
            timeSave = timeUsed ;
            printf("10 minus for saving ! \n") ;
            this->saveWeightsAndBiasToFile(irepeat,totalErr,numGoodFit,numBadFit) ;
        }else if( numBadFit==0 && totalErr < 0.1 ){
            printf("bad fit = 0 and totalErr < 0.1. Finished! \n") ;
            this->saveWeightsAndBiasToFile(irepeat,totalErr,numGoodFit,numBadFit) ;
            break;
        }else if( irepeat == repeat-1 ){
            printf("All repeat finished! \n") ;
            this->saveWeightsAndBiasToFile(irepeat,totalErr,numGoodFit,numBadFit) ;
            break;
        }
	}//irepeat
}

int CNn::classify( WData& data , double & outPossibility ) {
	std::vector<CLayer*>::iterator inlayer =  layers.begin() ;
	std::vector<CLayer*>::iterator outlayer = layers.end()-1 ;
	int numNeu = (*inlayer)->getNumOfNeurons() ;
	for(int i = 0 ; i<numNeu ; ++ i ){
		CNeuron* pNeu = (*inlayer)->neurons[i] ;
		pNeu->setInputLayerOutputValue( data.getDataAt(i) ) ;
	}
	int numlyr = (int)this->layers.size() ;
	for( int i = 1 ; i<numlyr ; ++ i ){
		CLayer* prevLyr = this->layers[i-1] ;
		CLayer* currLyr = this->layers[i] ;
		currLyr->calculateAllNeurons( prevLyr ) ;
	}
	double poss = -1 ;
	int iest = 0 ;
	int outN =(int) (*outlayer)->neurons.size() ;
	for(int i = 0 ;i<outN ; ++ i ){
        double poss1 = (*outlayer)->neurons[i]->output_a ;
        if( poss1 > poss ){
        	poss = poss1 ;
        	iest = i ;
        }
	}
    outPossibility = poss ;
	return iest ;
}


void CNn::saveWeightsAndBiasToFile(int irepeat , double err , int ngood , int nbad ) {
    char path[1024] ;
    sprintf(path , "wb-%d.txt" , irepeat) ;
    FILE* pf = fopen(path , "w") ;
    if(pf==NULL){
        printf("ERROR open output file failed.\n") ;
        return ;
    }
    fprintf(pf,"layers err %f ngood %d nbad %d \n", err , ngood , nbad ) ;
    for(unsigned int i = 0 ; i<this->layers.size() ; ++i ){
        fprintf(pf,"%d " , this->layers[i]->neurons.size()) ;
    }
    fprintf(pf,"\n") ;
    fprintf(pf,"weights and bias begin with layers[1].\n") ;
    for(unsigned int i = 1 ; i<this->layers.size()  ; ++ i ){
        int nn = this->layers[i]->neurons.size() ;
        for(int j = 0 ; j<nn ; ++ j ){
            CNeuron* nu = this->layers[i]->neurons[j] ;
            for(int k = 0 ; k<nu->getNumOfWeights() ; ++ k ){
                fprintf(pf, "%10.5f " , nu->weights[k] ) ;
            }
            fprintf(pf,"%10.5f\n" , nu->bias ) ;
        }
        fprintf(pf,"\n") ;
    }
    fclose(pf) ;
    pf = NULL ;
}









