//神经网络学习代码 20161214 jfwf@yeah.net

#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "convnetwork2.h"


using namespace std;


//卷积神经网络处理32x32图像
int main() {
	printf("Conv Network Demo 2017-01. \n") ;
	srand(time(NULL)) ;

	CNNInputLayer*    layer0    = new CNNInputLayer(32,32,3) ;
	CNNConvLayer*     convlayer1 = new CNNConvLayer(32,32,3,5,5,10) ;
	int csize1 = convlayer1->mactiv3d->getNumx() ;
	CNNPoolLayer*     poollayer1 = new CNNPoolLayer(csize1,csize1,10) ;
	int psize1 = poollayer1->mactiv3d->getNumx() ;
	CNNFullConnLayer* fclayer1  = new CNNFullConnLayer(psize1*psize1*10,6) ;
	CNNOutputLayer*   outlayer2 = new CNNOutputLayer(6,3) ;

	ConvNetwork2 net2 ;
	net2.mLayerVector.push_back(layer0) ;
	net2.mLayerVector.push_back(convlayer1) ;
	net2.mLayerVector.push_back(poollayer1) ;
	net2.mLayerVector.push_back(fclayer1) ;
	net2.mLayerVector.push_back(outlayer2) ;

    double traindata[] = {
        1,2,0 ,
        2,4,0 ,
        11,10,1,
        12,15,1,
        20,24,2,
        21,22,2,
        35,33,3,
        34,34,3,
        41,40,4,
        45,41,4 } ;

    wImage img0("img32-car0.png") ;
    wImage img1("img32-fox1.png") ;
    wImage img2("img32-flower2.png") ;

    double mse_sum= 0 ;
    for(int i=0 ; i<100000; ++ i ){

        int ri = wft_randFromZeroToN(3) ;
        int target = ri ;
        if(ri==0){
            layer0->resetDataFromWImage(img0) ;
        }else if( ri==1 ){
            layer0->resetDataFromWImage(img1) ;
        }else{
            layer0->resetDataFromWImage(img2) ;
        }

        net2.doforward() ;
        net2.dobackpropagation(target) ;
        net2.computeAndSumPartialDerivForWeightsAndBias() ;

        double mse1 = outlayer2->computeMSE(target) ;
        mse_sum += mse1 ;
        if( i>0 && i%9==0 ){
            net2.updateWeightsAndBias( 0.1 ,0.9) ;
            cout<<"MSE: "<<mse_sum/10.0<<endl ;
            if( mse_sum/10.0 < 0.01 ){
                break;
            }
            mse_sum = 0 ;
        }
    }

    cout<<"Training finished."<<endl ;

    do{
        double in0 , in1 ;
        cout<<"Input two value to test. value must greater than 0."<<endl;
        cin>>in0>>in1 ;
        if(in0<0 || in1<0 ){
            cout<<"in0 or in1 less than zero. program out."<<endl;
            break ;
        }
        layer0->mdata3d->set1DValueAt(0,in0/40.0) ;
        layer0->mdata3d->set1DValueAt(1,in1/40.0) ;
        net2.doforward() ;
        outlayer2->printBestFit() ;

    }while(1) ;

	printf("press any key to quit.\n") ;
    getchar() ;
    printf("press any key to quit.\n") ;
    getchar() ;
    printf("press any key to quit.\n") ;
    getchar() ;

	return true ;
}


