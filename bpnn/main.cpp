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

int global_index_data = 0 ;
int global_index_layer = 0 ;

//hiddenlayer 的层数增加 那么studyrate要响应减小
//输入数据必要归一化
//每次对训练样本循环之前都要重新洗牌，保证每次训练样本的出现顺序随机分布，否则会造成训练往复运动不会收敛。

using namespace std;


extern bool readBin( std::vector<WData>& datas ) ;
extern bool readHandWrite( std::vector<WData>& datas,const int starti,const int endi ) ;
extern bool readHandWrite2( std::vector<WData>& datas  ) ;

int main() {
	printf("hello jfwf\n") ;
	srand(time(NULL)) ;


    cout<<"0. a new train mode; 1. continue train mode ; 2. classify mode."<<endl ;
    int theMode = 0 ;
    cin>>theMode ;
    cout<<"You select:"<<theMode<<endl ;

    if(theMode==0 ){
        std::vector<int> vlayers ;
        int trainStartIndex , trainEndIndex ;
        cout<<"input training start and end index :" ;
        cin>>trainStartIndex>>trainEndIndex ;
        cout<<"traning number is "<<trainStartIndex<<" -- "<<trainEndIndex<<endl ;
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
        readHandWrite(data2,trainStartIndex,trainEndIndex) ;
        network.train( data2 , repeat  , studyRate ) ;
        printf("\ntrained ok.\n") ;
    }else if( theMode==1 ){
        cout<<"Input weights and bias file path."<<endl;
        string wbfilepath ;
        cin>>wbfilepath;
        cout<<"Your input:"<<wbfilepath<<endl ;
        int trainStartIndex , trainEndIndex ;
        cout<<"input training start and end index :" ;
        cin>>trainStartIndex>>trainEndIndex ;
        cout<<"traning number is "<<trainStartIndex<<" -- "<<trainEndIndex<<endl ;
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
        readHandWrite(data1,trainStartIndex,trainEndIndex) ;
        network1.train( data1 , repeat  , studyRate ) ;

        cout<<"Continue train mode finished."<<endl;
    }else if(theMode==2 ){
        cout<<"Input weights and bias file path."<<endl;
        string wbfilepath ;
        cin>>wbfilepath;
        cout<<"Your input:"<<wbfilepath<<endl ;
        CNn network2( wbfilepath.c_str() ) ;

        std::vector<WData> data2 ;
        readHandWrite2(data2) ;

        int ngood = 0 ;
        int nbad = 0 ;
        int nall = data2.size() ;

        for(int i = 0 ; i<nall ; ++i ){
            WData wd = data2[i] ;
            double poss = 0;
            int iguest = network2.classify(wd , poss ) ;
            if(iguest == wd.getY() ){
                ++ ngood ;
            }else{
                ++ nbad ;
            }
            if(i%100==0){
                cout<<"i,good,bad:"<<i<<","<<ngood<<","<<nbad<<endl ;
            }
        }
        double goodPercent = ngood*1.0/nall ;
        printf("\n finish: good:%d bad:%d goodPer:%f\n" , ngood,nbad,goodPercent) ;
        cout<<"classify mode finished."<<endl;
    }
    printf("\nPress any key to quit.\n") ;
    getchar() ;
    printf("\nPress any key to quit.\n") ;
    getchar() ;
    printf("\nPress any key to quit.\n") ;
    getchar() ;
	return 0 ;
}



bool readBin(   std::vector<WData>& datas ){
	FILE* pf = fopen( "data_batch_1.bin" , "rb") ;
	if( pf == NULL ){
		return false ;
	}
	unsigned char buffer[4096] ;
	for(int i = 0 ;i<10000 ; ++ i ){
		fread(buffer,3073,1,pf);
		double ty = (double) buffer[0] ;
		unsigned char * p1 = & (buffer[1]) ;
		WData d1( p1 , 3072 , ty , 255 , 1 ) ;
		datas.push_back(d1) ;
	}

	fclose(pf) ;
	pf = NULL ;
	return true ;
}

int b4toint(unsigned char* b){
	unsigned char nb[4] ;
	nb[0] = b[3] ;
	nb[1] = b[2] ;
	nb[2] = b[1] ;
	nb[3] = b[0] ;
	int v = 0 ;
	memcpy(&v , nb , 4) ;
	return v ;
}

bool readHandWrite( std::vector<WData>& datas , const int starti , const int endi ){
	FILE* fileImg = fopen("train-images-idx3-ubyte","rb") ;
	FILE* fileLbl = fopen("train-labels-idx1-ubyte","rb") ;

	assert( fileImg != NULL ) ;
	assert( fileLbl != NULL ) ;

	int magic1 = 0 ;
	int magic2 = 0 ;
	int num1 = 0 ;
	int num2 = 0 ;
	int rows = 0 ;
	int cols = 0 ;

	unsigned char b4[4] ;
	fread( b4 , 4 , 1 , fileImg ) ;
	magic1 = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileLbl ) ;
	magic2 = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileImg ) ;
	num1 = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileLbl ) ;
	num2 = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileImg ) ;
	rows = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileImg ) ;
	cols = b4toint(b4) ;

	printf("\nm1 m2 n1 n2 rows cols: %d %d %d %d %d %d\n" , magic1 , magic2 , num1 , num2 , rows , cols) ;

	int imgSize = rows * cols ;
	unsigned char buff1[1024] ;
	unsigned char buff2[1] ;
	int nsample[10]={0,0,0, 0,0,0, 0,0,0,0} ;
	int usedSamples = 0 ;
	for(int i = 0 ; i<num1 ; ++ i ){
		//label
		fread( buff2 , 1 , 1 , fileLbl ) ;
		//image
		fread( buff1 , imgSize , 1 , fileImg ) ;
		if( i>=starti && i<endi ){
            int y = (int)buff2[0] ;
            WData d1(buff1 , imgSize , (double)y , 1000 , 10) ;
            d1.originIndex = i ;
            datas.push_back(d1) ;
            ++ nsample[y] ;
            ++ usedSamples ;
		}else if( i>= endi ){
            break ;
		}
	}
	fclose(fileImg) ;
	fileImg = NULL ;
	fclose(fileLbl) ;
	fileLbl = NULL ;

	printf("num of samples:\n") ;
	for(int i = 0 ; i<10 ; ++ i ){
        printf("%d %8.2f\n" , i , nsample[i]*1.0/usedSamples ) ;
	}
    printf("\n") ;
	return true ;
}

bool readHandWrite2( std::vector<WData>& datas  ){
	FILE* fileImg = fopen("t10k-images.idx3-ubyte","rb") ;
	FILE* fileLbl = fopen("t10k-labels.idx1-ubyte","rb") ;

	assert( fileImg != NULL ) ;
	assert( fileLbl != NULL ) ;

	int magic1 = 0 ;
	int magic2 = 0 ;
	int num1 = 0 ;
	int num2 = 0 ;
	int rows = 0 ;
	int cols = 0 ;

	printf( "sz int %d\n" , (int) sizeof(int)) ;

	unsigned char b4[4] ;
	fread( b4 , 4 , 1 , fileImg ) ;
	magic1 = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileLbl ) ;
	magic2 = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileImg ) ;
	num1 = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileLbl ) ;
	num2 = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileImg ) ;
	rows = b4toint(b4) ;
	fread( b4 , 4 , 1 , fileImg ) ;
	cols = b4toint(b4) ;

	printf("\nm1 m2 n1 n2 rows cols: %d %d %d %d %d %d\n" , magic1 , magic2 , num1 , num2 , rows , cols) ;

	int imgSize = rows * cols ;
	unsigned char buff1[1024] ;
	unsigned char buff2[1] ;
	int nsample[10]={0,0,0, 0,0,0, 0,0,0,0} ;
	int usedSamples = 0;
	for(int i = 0 ; i<num1 ; ++ i ){
		//label
		fread( buff2 , 1 , 1 , fileLbl ) ;
		//image
		fread( buff1 , imgSize , 1 , fileImg ) ;
        int y = (int)buff2[0] ;
        WData d1(buff1 , imgSize , (double)y ,220 , 10) ;
        datas.push_back(d1) ;
        ++ nsample[y] ;
		++ usedSamples ;
	}
	fclose(fileImg) ;
	fileImg = NULL ;
	fclose(fileLbl) ;
	fileLbl = NULL ;

	printf("num of samples:\n") ;
	for(int i = 0 ; i<10 ; ++ i ){
        printf("%d %8.2f\n" , i , nsample[i]*1.0/usedSamples ) ;
	}
    printf("\n") ;
	return true ;
}

void test1( CNn& nn ){
	std::string testfile ;
	cout<<"input test file :" ;
	cin>>testfile ;
	cout<<"test file:"<<testfile<<endl ;

	int ntest = 0 ;
	cout<<"input test number:" ;
	cin>>ntest ;
	cout<<"test number:"<<ntest<<endl ;
	FILE* pf = fopen( testfile.c_str() , "rb") ;
	if( pf == NULL ){
	}
	int goodEst = 0 ;
	for(int i = 0 ; i<ntest ; ++ i ){
		unsigned char buffer[4096] ;
		fread(buffer,3073,1,pf);
		double ty = (double) buffer[0] ;
		unsigned char * p1 = & (buffer[1]) ;
		WData d1( p1 , 3072 , ty , 255 , 10 ) ;
		printf("test %d \n" , i) ;
		double poss=0;
		int iest = nn.classify( d1 , poss ) ;
		if( iest == (int)ty ){
			++goodEst ;
		}
	}
	fclose(pf) ;
	pf = NULL ;

	printf( "Test %d , Good Est %d , Good/All %4.2f \n" ,ntest , goodEst , goodEst*1.0/ntest ) ;
}



