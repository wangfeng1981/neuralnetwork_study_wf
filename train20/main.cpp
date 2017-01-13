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
#include "wftools.h"


using namespace std;

void loadTrainSet1(vector<wImage*>& vImages,vector<int>& vLabels) ;
void ReleaseTrainSet1(vector<wImage*>& vImages) ;

//卷积神经网络处理32x32图像
int main() {
	printf("Conv Network Demo 2017-01. \n") ;
	srand(time(NULL)) ;

	CNNInputLayer*    layer0    = new CNNInputLayer(24,24,3) ;
	CNNConvLayer*     convlayer1 = new CNNConvLayer(24,24,3,5,5,4) ;
	int csize1 = convlayer1->mactiv3d->getNumx() ;
	CNNPoolLayer*     poollayer1 = new CNNPoolLayer(csize1,csize1,4) ;
	int psize1 = poollayer1->mactiv3d->getNumx() ;
	CNNConvLayer*     convlayer2 = new CNNConvLayer(psize1,psize1,4,5,5,8) ;
	int csize2 = convlayer2->mactiv3d->getNumx() ;
	CNNPoolLayer*     poollayer2 = new CNNPoolLayer(csize2,csize2,8) ;
	int psize2 = poollayer2->mactiv3d->getNumx() ;
	CNNOutputLayer*  outlayer2  = new CNNOutputLayer(psize2*psize2*8,3) ;

	ConvNetwork2 net2 ;
	net2.mLayerVector.push_back(layer0) ;
	net2.mLayerVector.push_back(convlayer1) ;
	net2.mLayerVector.push_back(poollayer1) ;
	net2.mLayerVector.push_back(convlayer2) ;
	net2.mLayerVector.push_back(poollayer2) ;
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

    wImage img0("img80.png") ;
    wImage img1("img81.png") ;
    wImage img2("img82.png") ;

    vector<wImage*> imageSet1 ;
    vector<int>     labelSet1 ;
    loadTrainSet1(imageSet1,labelSet1) ;
    int numInSet1 =(int) imageSet1.size() ;

    double mse_sum= 0 ;
    for(int i=0 ; i<100000; ++ i ){

        int ri = wft_randFromZeroToN(numInSet1) ;
        int target = labelSet1[ri] ;
        layer0->resetDataFromWImage(*imageSet1[ri],0.01) ;

        net2.doforward() ;
        net2.dobackpropagation(target) ;
        net2.computeAndSumPartialDerivForWeightsAndBias() ;

        double mse1 = outlayer2->computeMSE(target) ;
        mse_sum += mse1 ;
        const int batchn = 5 ;
        if( i>0 && i%batchn==0 ){
            net2.updateWeightsAndBias( 0.01 ,0.9) ;
            double m_mse = mse_sum/batchn ;
            cout<<"Mean-MSE: "<<m_mse<<endl ;
            if( m_mse < 0.01 ){
                break;
            }
            mse_sum = 0 ;
        }
    }

    cout<<"Training finished."<<endl ;
    ReleaseTrainSet1(imageSet1) ;

    do{
        /*
        double in0 , in1 ;
        cout<<"Input two value to test. value must greater than 0."<<endl;
        cin>>in0>>in1 ;
        if(in0<0 || in1<0 ){
            cout<<"in0 or in1 less than zero. program out."<<endl;
            break ;
        }*/
        cout<<"input image.png path to test."<<endl ;
        string testimage ;
        cin>>testimage ;
        wImage image(testimage.c_str()) ;
        layer0->resetDataFromWImage(image,0.01) ;
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

void loadTrainSet1(vector<wImage*>& vImages,vector<int>& vLabels) {
    char buff[100] ;
    //car - 0
    for(int i = 0 ; i<16 ; ++ i ){
        sprintf(buff,"trainset1/cars0/img%d.png" , i ) ;
        wImage* img = new wImage(buff) ;
        vImages.push_back(img) ;
        vLabels.push_back(0) ;
    }

    //men - 1
    for(int i = 0 ; i<12 ; ++ i ){
        sprintf(buff,"trainset1/men1/img%d.png" , i ) ;
        wImage* img = new wImage(buff) ;
        vImages.push_back(img) ;
        vLabels.push_back(1) ;
    }

    //phones - 2
    for(int i = 0 ; i<12 ; ++ i ){
        sprintf(buff,"trainset1/phones2/img%d.png" , i ) ;
        wImage* img = new wImage(buff) ;
        vImages.push_back(img) ;
        vLabels.push_back(2) ;
    }

}
void ReleaseTrainSet1(vector<wImage*>& vImages) {
    for(unsigned int i =0 ; i<vImages.size() ; ++ i ){
        if(  vImages[i]  ){
            delete  vImages[i]  ;
             vImages[i]  = 0 ;
        }
    }
}


