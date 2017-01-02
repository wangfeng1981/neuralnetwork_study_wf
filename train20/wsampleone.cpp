#include "wsampleone.h"
#include "lodepng.h"
#include <assert.h>
#include "wdata.h"
#include <iostream>

using namespace std;

wSampleOne::wSampleOne(const char* imgfile,const char* weightsfile)
{
    //ctor
    this->mpImagedata = new wImage(imgfile) ;
    this->mpNetwork = new CNn(weightsfile);

}

wSampleOne::~wSampleOne()
{
    //dtor
    if(this->mpImagedata){
        delete this->mpImagedata ;
        this->mpImagedata = 0 ;
    }
    if(this->mpNetwork){
        delete this->mpNetwork ;
        this->mpNetwork = 0 ;
    }
}

void wSampleOne::startClassify(){
    assert(this->mpImagedata) ;
    assert(this->mpNetwork) ;

    cout<<"sizeof(float):"<<sizeof(float)<<endl ;

    int nr = this->mpImagedata->getRows() ;
    int nc = this->mpImagedata->getCols() ;

    unsigned char* resc = new unsigned char[nr*nc] ;
    float* resp = new float[nr*nc] ;

    for( int irow = 0 ; irow <this->mpImagedata->getRows() ; irow+= 20 ){
        for(int icol = 0 ; icol <this->mpImagedata->getCols(); icol+= 20 ){
            WData data1( this->mpImagedata , irow,icol,20,20,0,10000,5) ;
            double poss = 0 ;
            int res = this->mpNetwork->classify( data1 , poss ) ;
            for(int iir = irow ; iir<irow+20 ; ++iir ){
                for(int iic = icol ; iic<icol+20 ; ++ iic ){
                    resc[iir*nc+iic] = (unsigned char)res ;
                    resp[iir*nc+iic] = (float)poss ;
                }
            }

        }
    }



    FILE* filec = fopen("out-c.bin" , "wb") ;
    FILE* filep = fopen("out-p.bin" , "wb") ;

    fwrite( resc , 1 , nc*nr , filec) ;
    fwrite( resp , sizeof(float) , nc*nr , filep) ;


    delete[] resc ;
    resc = 0 ;
    delete[] resp ;
    resp = 0 ;

    fclose(filec) ;
    fclose(filep) ;
    filec = 0 ;
    filep = 0 ;

    cout<<"finished."<<endl;
}
