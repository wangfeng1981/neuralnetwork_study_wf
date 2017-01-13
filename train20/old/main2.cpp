//神经网络学习代码 20161214 jfwf@yeah.net
//build 1

#include <stdio.h>
#include <string>
#include <vector>
#include "cneuron.h"
#include <stdlib.h>
#include "clayer.h"
#include "cnn.h"
#include <iostream>
#include <time.h>
#include <assert.h>
#include <string.h>
#include "netlayer.h"
#include "poollayer.h"

#include "wsampleone.h"

int global_index_data = 0 ;
int global_index_layer = 0 ;

//hiddenlayer 的层数增加 那么studyrate要响应减小
//输入数据必要归一化
//每次对训练样本循环之前都要重新洗牌，保证每次训练样本的出现顺序随机分布，否则会造成训练往复运动不会收敛。

using namespace std;


extern bool readImages( std::vector<WData>& datas ) ;
//普通神经网络分 遥感20x20数据
int main2() {
	printf("hello jfwf\n") ;
	srand(time(NULL)) ;

    NetLayer* tempLayer0 = new PoolLayer( 10 ,10 , 3) ;
    delete tempLayer0 ;
    tempLayer0 = 0 ;
    return 1;

    cout<<"0. a new train mode; 1. continue train mode ; 2. classify mode."<<endl ;
    int theMode = 0 ;
    cin>>theMode ;
    cout<<"You select:"<<theMode<<endl ;

    if(theMode==0 ){
        std::vector<int> vlayers ;
        int nlyr = 0 ;
        cout<<"input number of layers:" ;
        cin>>nlyr ;
        if( nlyr < 3 ){
            cout<<"invalid number of layers."<<endl ;
            return 1 ;
        }
        for(int i = 0 ; i<nlyr ; ++ i ){
            double nn1 = 0 ;
            cin>>nn1 ;
            cout<<"layer"<<i<<":"<<nn1<<endl ;
            vlayers.push_back(nn1) ;
        }
        double studyRate = 1 ;
        cout<<"input study rate:" ;
        cin>>studyRate ;
        cout<<"study rate is "<<studyRate<<endl ;
        int repeat = 10 ;
        cout<<"input repeat count:" ;
        cin>>repeat ;
        while(repeat==0){
            cin>>repeat ;
        }
        cout<<"repeat count is "<<repeat<<endl ;
        CNn network ( vlayers) ;
        std::vector<WData> data2 ;
        readImages(data2) ;
        network.train( data2 , repeat  , studyRate ) ;
        printf("\ntrained ok.\n") ;
    }else if( theMode==1 ){
        cout<<"Input weights and bias file path."<<endl;
        string wbfilepath ;
        cin>>wbfilepath;
        cout<<"Your input:"<<wbfilepath<<endl ;
        double studyRate = 1 ;
        cout<<"input study rate:" ;
        cin>>studyRate ;
        cout<<"study rate is "<<studyRate<<endl ;
        int repeat = 10 ;
        cout<<"input repeat count:" ;
        cin>>repeat ;
        cout<<"repeat count is "<<repeat<<endl ;

        CNn network1( wbfilepath.c_str() ) ;
        std::vector<WData> data1 ;
        readImages(data1) ;
        network1.train( data1 , repeat  , studyRate ) ;

        cout<<"Continue train mode finished."<<endl;
    }else if(theMode==2 ){

        cout<<"Input weights and bias file path."<<endl;
        string wbfilepath ;
        cin>>wbfilepath;
        cout<<"Your input:"<<wbfilepath<<endl ;

        cout<<"Input image path."<<endl;
        string imgpath ;
        cin>>imgpath ;
        cout<<"image path : "<<imgpath <<endl;

        wSampleOne sample( imgpath.c_str() , wbfilepath.c_str());
        sample.startClassify() ;

        cout<<"classify finished."<<endl;
    }
    printf("\nPress any key to quit.\n") ;
    getchar() ;
    printf("\nPress any key to quit.\n") ;
    getchar() ;
    printf("\nPress any key to quit.\n") ;
    getchar() ;
	return 0 ;
}



bool readImages(   std::vector<WData>& datas ){

    cout<<"input num of class"<<endl;
    int numclass (0) ;
    numclass = 5 ;//cin>>numclass ;
    cout<<"You have "<<numclass<<" class"<<endl;
    for(int ic = 0 ; ic<numclass ; ++ ic ){
        int i0 (0) , i1(0) ;
        cout<<"input i0 - i1 for class "<<ic<<endl ;
        if(ic==0){
            i1 = 12 ;
        }else if(ic==1 ){
            i1 = 82 ;
        }else if( ic==2 ){
            i1 = 37;
        }else if( ic==3 ){
            i1 = 5 ;
        }else {
            i1 = 5 ;
        }
        //cin>>i0>>i1 ;
        char buff[100] ;
        for( int ifile = i0 ; ifile <=i1 ; ++ifile ){
            sprintf(buff , "../train_samples/%d_%d.png" , ic , ifile ) ;
            wImage img0(buff) ;
            WData d1( &img0 , (double)ic , 10000.0 , numclass ) ;
            datas.push_back(d1) ;
        }
    }
	return true ;
}


