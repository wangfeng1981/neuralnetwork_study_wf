#include "convnetwork2.h"
#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "wftools.h"

//#define DEBUG_MODE1

using namespace std;

#define SAFE_DELETE(p) if(p){delete p;p=0;}

CNNInputLayer::CNNInputLayer(const int nx,const int ny,const int nz) {
    mLayerType = CNNLayerType::CNNLayerTypeInput ;
    this->mdata3d = new Array3d(nx,ny,nz,false) ;
}
CNNInputLayer::~CNNInputLayer() {
    SAFE_DELETE(mdata3d) ;
}
void CNNInputLayer::saveToDisk(char* filepath) {
    FILE* pf = fopen(filepath , "w") ;
    assert(pf) ;
    fprintf(pf,"CNNInputLayer\n") ;
    fprintf(pf,"onx  ony  onz\n") ;
    fprintf(pf,"%d %d %d\n" , this->mdata3d->getNumx() ,
            this->mdata3d->getNumy() ,
            this->mdata3d->getNumdep() ) ;

    fclose(pf) ;
    pf = 0 ;
}
void CNNInputLayer::resetDataFromWImage(wImage& wimg,double scale) {
    assert(this->mdata3d->getNumx()==(int)wimg.getCols()) ;
    assert(this->mdata3d->getNumy()==(int)wimg.getRows()) ;
    assert(this->mdata3d->getNumdep()==3) ;

    for(int ix = 0 ; ix < mdata3d->getNumx() ; ++ ix ){
        for(int iy = 0 ; iy < mdata3d->getNumy() ; ++ iy ){
            int r,g,b ;
            wimg.getRGB(iy,ix,r,g,b) ;
            mdata3d->setValueAt(ix,iy,0,(double)r*scale) ;
            mdata3d->setValueAt(ix,iy,1,(double)g*scale) ;
            mdata3d->setValueAt(ix,iy,2,(double)b*scale) ;
        }
    }
}

CNNConvLayer::CNNConvLayer(const int inx,const int iny,const int inz,
                     const int fnx,const int fny,const int nfilter )
{
    mLayerType = CNNLayerType::CNNLayerTypeConv ;
    assert(inx==iny) ;
    assert(fnx==fny) ;
    assert(fnx<=inx) ;
    assert(nfilter>0) ;
    int outsize = inx - fnx + 1;
    this->mactiv3d = new Array3d(outsize , outsize , nfilter , false) ;
    this->moutput3d = new Array3d(outsize , outsize , nfilter , false) ;
    this->mdelta3d = new Array3d(outsize , outsize , nfilter , false) ;

    this->mSampleNumber = 0 ;
    for(int i = 0; i<nfilter ; ++ i ){
        Array3d* filter1 = new Array3d(fnx,fny,inz,true) ;
        this->mfilter3dVector.push_back(filter1) ;

        Array3d* fwsum = new Array3d(fnx,fny,inz,false) ;
        this->mDeltaFilterWeightsSum3dVector.push_back(fwsum) ;
    }
}
CNNConvLayer::~CNNConvLayer() {
    SAFE_DELETE(this->mactiv3d) ;
    SAFE_DELETE(this->mdelta3d) ;
    SAFE_DELETE(this->moutput3d) ;
    for(unsigned int i = 0 ; i<this->mfilter3dVector.size() ; ++ i ){
        SAFE_DELETE(this->mfilter3dVector[i]) ;
        SAFE_DELETE(this->mDeltaFilterWeightsSum3dVector[i]) ;
    }
}
void CNNConvLayer::saveToDisk(char* filepath) {
    FILE* pf = fopen(filepath , "w") ;
    assert(pf) ;
    fprintf(pf,"CNNConvLayer\n") ;
    fprintf(pf,"nf fx fy fz ox oy\n") ;
    fprintf(pf,"%d %d %d %d %d %d\n" ,
            this->mfilter3dVector.size() ,
            this->mfilter3dVector[0]->getNumx() ,
            this->mfilter3dVector[1]->getNumy() ,
            this->mfilter3dVector[2]->getNumdep() ,
            this->mactiv3d->getNumx() ,
            this->mactiv3d->getNumy()
            ) ;

    char buff[100] ;
    for(int i = 0 ; i<this->mfilter3dVector.size() ; ++ i ){
        Array3d* arr3d = this->mfilter3dVector[i] ;
        arr3d->saveToFilePtr(pf) ;
        sprintf(buff,"%s-f%d.png" , filepath , i) ;
        arr3d->saveToPngFile(buff) ;
    }

    fclose(pf) ;
    pf = 0 ;
}

void CNNConvLayer::computeAndSumFilterDeltaWeights(const Array3d* prevLayerActivArray) {
    int outnx = this->mactiv3d->getNumx() ;
    int outny = this->mactiv3d->getNumy() ;
    //int outnz = this->mactiv3d->getNumdep() ;

    int knx = this->mfilter3dVector[0]->getNumx() ;
    int kny = this->mfilter3dVector[0]->getNumy() ;
    int knz = this->mfilter3dVector[0]->getNumdep() ;

    int nfilter = (int)this->mfilter3dVector.size() ;

    for(int ifilter = 0 ; ifilter < nfilter ; ++ ifilter ){

        //Array3d* filter = this->mfilter3dVector[ifilter] ;
        Array3d* dweightSum = this->mDeltaFilterWeightsSum3dVector[ifilter] ;
        for(int kz = 0 ; kz < knz ; ++ kz ){
            for(int kx = 0 ; kx < knx ; ++ kx ){
                for(int ky = 0 ; ky < kny ; ++ ky ){
                    double sum1 = 0.0 ;

                    for(int ox = 0 ; ox < outnx ; ++ ox ){
                        for(int oy = 0 ; oy < outny ; ++ oy ){
                            double dxyfr = this->mdelta3d->getValueAt(ox,oy,ifilter) ;
                            double actir = prevLayerActivArray->getValueAt(ox+kx,oy+ky,kz) ;
                            double dw = dxyfr * actir ;
                            sum1 += dw ;
                        }
                    }

                    double oldsum = dweightSum->getValueAt(kx,ky,kz) ;
                    double newsum = oldsum + sum1 ;
                    dweightSum->setValueAt(kx,ky,kz,newsum) ;

                }
            }
        }
    }
    ++ mSampleNumber ;
}

void CNNConvLayer::updateFilterWeights(const double studyRate,const double momentum) {
    double rate = studyRate/this->mSampleNumber ;
    int knx = this->mfilter3dVector[0]->getNumx() ;
    int kny = this->mfilter3dVector[0]->getNumy() ;
    int knz = this->mfilter3dVector[0]->getNumdep() ;

    int nfilter = (int)this->mfilter3dVector.size() ;

    #ifdef DEBUG_MODE
    cout<<"Convolution dw"<<endl;
    #endif // DEBUG_MODE
    for(int ifilter = 0 ; ifilter < nfilter ; ++ ifilter ){
        #ifdef DEBUG_MODE
        cout<<"filter:"<<ifilter<<endl;
        #endif // DEBUG_MODE
        Array3d* filter = this->mfilter3dVector[ifilter] ;
        Array3d* dweightSum = this->mDeltaFilterWeightsSum3dVector[ifilter] ;
        for(int kz = 0 ; kz < knz ; ++ kz ){
            for(int ky = 0 ; ky < kny ; ++ ky ){
                for(int kx = 0 ; kx < knx ; ++ kx ){
                    double dwsum = dweightSum->getValueAt(kx,ky,kz) ;
                    double dw = dwsum * rate ;

                    double oldw = filter->getValueAt(kx,ky,kz) ;
                    double neww = oldw - dw ;
                    filter->setValueAt(kx,ky,kz,neww) ;
                    #ifdef DEBUG_MODE
                    printf("%5.4f " , dw) ;
                    #endif // DEBUG_MODE
                }
                #ifdef DEBUG_MODE
                cout<<endl;
                #endif // DEBUG_MODE
            }
            #ifdef DEBUG_MODE
            cout<<endl;
            #endif // DEBUG_MODE
        }
        dweightSum->zero() ;
    }
    this->mSampleNumber = 0 ;
}


CNNPoolLayer::CNNPoolLayer(const int inx,const int iny,const int inz) {
    mLayerType = CNNLayerType::CNNLayerTypePool ;
    assert(inx>1) ;
    assert(inx==iny) ;
    assert(inz>0) ;
    int outsize = (int)inx/2 ;
    this->mactiv3d = new Array3d(outsize , outsize , inz , false ) ;
    this->mdelta3d = new Array3d(outsize , outsize , inz , false ) ;
    this->minputweight3d = new Array3d(inx,iny,inz,false) ;
}
CNNPoolLayer::~CNNPoolLayer() {
    SAFE_DELETE(this->mactiv3d) ;
    SAFE_DELETE(this->mdelta3d) ;
    SAFE_DELETE(this->minputweight3d) ;
}
void CNNPoolLayer::saveToDisk(char* filepath) {
    FILE* pf = fopen(filepath , "w") ;
    assert(pf) ;
    fprintf(pf,"CNNPoolLayer\n") ;
    fprintf(pf,"inx iny inz\n") ;
    fprintf(pf,"%d  %d  %d \n" , this->minputweight3d->getNumx(),
            this->minputweight3d->getNumy(),
            this->minputweight3d->getNumdep()) ;
    fprintf(pf,"onx ony onz\n") ;
    fprintf(pf,"%d  %d  %d \n" , this->mactiv3d->getNumx(),
            this->mactiv3d->getNumy(),
            this->mactiv3d->getNumdep()) ;

    fclose(pf) ;
    pf = 0 ;
}



CNNFullConnLayer::CNNFullConnLayer(const int insize,const int outsize) {
    mLayerType = CNNLayerType::CNNLayerTypeFullConn ;
    assert(insize>0) ;
    assert(outsize>0) ;

    this->mactiv1d = new Array3d(1,1,outsize,false) ;
    this->mdelta1d = new Array3d(1,1,outsize,false) ;

    this->mbias1d   = new Array3d(1,1,outsize,true) ;
    this->mweight2d = new Array3d(insize,outsize,1,true) ;

    this->mDeltaWeightSum2d = new Array3d(insize,outsize,1,false) ;
    this->mDeltaBiasSum1d = new Array3d(1,1,outsize,false) ;
    this->mSampleNumber =0 ;
}
CNNFullConnLayer::~CNNFullConnLayer() {
    SAFE_DELETE(this->mactiv1d) ;
    SAFE_DELETE(this->mbias1d) ;
    SAFE_DELETE(this->mdelta1d) ;
    SAFE_DELETE(this->mweight2d) ;
    SAFE_DELETE(this->mDeltaWeightSum2d) ;
    SAFE_DELETE(this->mDeltaBiasSum1d) ;
}

void CNNFullConnLayer::saveToDisk(char* filepath) {
    FILE* pf = fopen(filepath , "w") ;
    assert(pf) ;
    fprintf(pf,"CNNFullConnLayer\n") ;
    this->mweight2d->saveToFilePtr(pf) ;
    this->mbias1d->saveToFilePtr(pf) ;

    fclose(pf) ;
    pf = 0 ;
}


void CNNFullConnLayer::computeAndSumDeltaWeightsAndBias(const Array3d* prevLayerActivArray) {
    int nneu = this->mactiv1d->get1DValueNum() ;
    int nw = this->mweight2d->getNumx() ;

    for(int inn = 0 ; inn < nneu ; ++ inn ){
        for(int iw = 0 ; iw < nw ; ++iw ){
            double prev_acti_k = prevLayerActivArray->get1DValueAt(iw) ;
            double curr_delta_i = this->mdelta1d->get1DValueAt(inn) ;
            double dw = prev_acti_k*curr_delta_i ;
            double oldsum = this->mDeltaWeightSum2d->getValueAt(iw,inn,0) ;
            double newsum = oldsum + dw ;
            this->mDeltaWeightSum2d->setValueAt(iw,inn,0,newsum) ;
        }
        double db =  this->mdelta1d->get1DValueAt(inn);
        double oldsumb = this->mDeltaBiasSum1d->get1DValueAt(inn) ;
        double newsumb = oldsumb + db ;
        this->mDeltaBiasSum1d->set1DValueAt(inn,newsumb) ;

    }
    ++ mSampleNumber ;

}
void CNNFullConnLayer::updateWeightsAndBias(double studyRate,double momentum) {
    double rate = studyRate/this->mSampleNumber ;
    #ifdef DEBUG_MODE
    cout<<"full connect dw"<<endl;
    #endif // DEBUG_MODE
    int nw = this->mweight2d->getNumx() ;
    int nneu = this->mweight2d->getNumy() ;
    for(int inn = 0 ; inn < nneu ; ++ inn ){
        for(int iw = 0 ; iw < nw ; ++iw ){
            double dwsum = this->mDeltaWeightSum2d->getValueAt(iw,inn,0) ;
            double dw = dwsum * rate ;
            double oldw = this->mweight2d->getValueAt(iw,inn,0) ;
            double neww = oldw - dw ;
            this->mweight2d->setValueAt(iw,inn,0,neww) ;
            #ifdef DEBUG_MODE
            printf("%.02f " , dw) ;
            #endif // DEBUG_MODE
        }
        double dbsum = this->mDeltaBiasSum1d->get1DValueAt(inn) ;
        double db = dbsum * rate ;
        double oldb = this->mbias1d->get1DValueAt(inn) ;
        double newb = oldb - db ;
        this->mbias1d->set1DValueAt(inn,newb) ;
        #ifdef DEBUG_MODE
        printf("%.03f " , db) ;
        cout<<endl;
        #endif // DEBUG_MODE
    }

    this->mDeltaWeightSum2d->zero() ;
    this->mDeltaBiasSum1d->zero() ;
    this->mSampleNumber = 0 ;

}

CNNOutputLayer::CNNOutputLayer(const int insize,const int outsize):CNNFullConnLayer(insize,outsize) {
    mLayerType = CNNLayerType::CNNLayerTypeOutput ;
}
CNNOutputLayer::~CNNOutputLayer() {

}
void CNNOutputLayer::printBestFit() {
    int bestfit = -1 ;
    double poss = -9999.99;
    for(int i = 0 ; i<this->mactiv1d->get1DValueNum(); ++ i){
        double a = this->mactiv1d->get1DValueAt(i) ;
        if( a > poss ){
            bestfit = i ;
            poss = a ;
        }
    }
    cout<<"Best fit:"<<bestfit<<" possibility:"<<poss<<endl;
}
int CNNOutputLayer::getBestFit() {
    int bestfit = -1 ;
    double poss = -9999.99;
    for(int i = 0 ; i<this->mactiv1d->get1DValueNum(); ++ i){
        double a = this->mactiv1d->get1DValueAt(i) ;
        if( a > poss ){
            bestfit = i ;
            poss = a ;
        }
    }
    return bestfit ;
}
double CNNOutputLayer::computeMSE(int target) {
    double mse = 0.0 ;
    for(int i = 0 ; i<this->mactiv1d->get1DValueNum(); ++ i){
        double a = this->mactiv1d->get1DValueAt(i) ;
        double y = 0.0 ;
        if( i== target ){
            y = 1.0 ;
        }
        double e1 = a-y ;
        mse += e1*e1 ;
    }
    mse = mse/2.0 ;
    return mse ;
}



ConvNetwork2::ConvNetwork2()
{
}

ConvNetwork2::~ConvNetwork2()
{
    for( unsigned int i = 0 ; i<mLayerVector.size() ; ++ i ){
        SAFE_DELETE(mLayerVector[i]) ;
    }
}

void ConvNetwork2::doforward(CNNInputLayer* inputLayer , CNNConvLayer* convLayer ) {
    //int insize = inputLayer->mdata3d->getNumx() ;
    int nband = inputLayer->mdata3d->getNumdep() ;

    Array3d* filter0 = convLayer->mfilter3dVector[0] ;
    int filtersize = filter0->getNumx();
    int nbandf = filter0->getNumdep() ;
    assert(nband==nbandf) ;

    int outsize = convLayer->mactiv3d->getNumx() ;
    #ifdef DEBUG_MODE
    wft_printArray3d(inputLayer->mdata3d , "input layer data:") ;
    #endif // DEBUG_MODE

    for(unsigned int ifilter = 0 ; ifilter < convLayer->mfilter3dVector.size() ; ++ ifilter ){
        Array3d* currfilter = convLayer->mfilter3dVector[ifilter] ;
        #ifdef DEBUG_MODE
        wft_printArray3d(currfilter , "filter weights:") ;
        #endif // DEBUG_MODE

        for(int outx = 0 ; outx < outsize ; ++outx ){

            for( int outy = 0 ; outy <outsize ; ++outy ){
                //each output pixel

                double out1 = 0.0 ;
                for(int fx = 0 ; fx < filtersize ;   ++ fx ){
                    for(int fy = 0 ; fy<filtersize ;   ++ fy ) {
                        for(int iband = 0 ; iband<nband   ;     ++ iband ){
                            double inval = inputLayer->mdata3d->getValueAt(outx+fx,outy+fy,iband) ;
                            double weight = currfilter->getValueAt(fx,fy,iband) ;
                            out1 += inval * weight ;
                        }//iband
                    }//fy
                }//fx
                convLayer->moutput3d->setValueAt(outx,outy,ifilter,out1) ;
                //ReLU
                double relu_acti = max(0.0,out1) ;
                convLayer->mactiv3d->setValueAt(outx,outy,ifilter,relu_acti) ;
            }//end outy
        }//end outx
    }//end ifilter
    #ifdef DEBUG_MODE
    wft_printArray3d(convLayer->mactiv3d , "feature map:") ;
    #endif // DEBUG_MODE
}

void ConvNetwork2::doforward(CNNInputLayer* inputLayer , CNNFullConnLayer* fcLayer ) {
    int nin = inputLayer->mdata3d->get1DValueNum() ;
    int nw = fcLayer->mweight2d->getNumx() ;
    int nout = fcLayer->mactiv1d->get1DValueNum() ;
    assert(nin==nw) ;

    for(int i = 0 ; i<nout ; ++ i ){
        double sum1 = fcLayer->mbias1d->get1DValueAt(i) ;

        for(int iw = 0 ; iw<nw ; ++ iw ){
            double wei = fcLayer->mweight2d->getValueAt(iw,i,0) ;
            double acti = inputLayer->mdata3d->get1DValueAt(iw) ;
            sum1 += wei*acti ;

        }
        double sigout = wft_sigmoid(sum1) ;
        fcLayer->mactiv1d->set1DValueAt(i,sigout) ;

    }
}

void ConvNetwork2::doforward(CNNConvLayer*  convLayer  , CNNPoolLayer* poolLayer ) {
    int insize = convLayer->mactiv3d->getNumx() ;
    int outsize = poolLayer->mactiv3d->getNumx() ;
    int nband = convLayer->mactiv3d->getNumdep() ;
    int nband1 = poolLayer->mactiv3d->getNumdep() ;
    assert(nband==nband1) ;
    int outsize1 = insize/2 ;
    assert(outsize1==outsize) ;

    for(int iband = 0 ; iband < nband ; ++ iband ){
        for(int x = 0 ; x < outsize ; ++x ){
            for(int y = 0 ; y<outsize ; ++ y ){

                double biggest_acti = 0.0 ;
                int x1s = 0 ;
                int y1s = 0 ;
                for(int x1 = 0 ; x1<2 ; ++ x1 ){
                    for(int y1 = 0 ; y1<2 ; ++ y1 ){
                        poolLayer->minputweight3d->setValueAt(x*2+x1,y*2+y1,iband,0.0) ;
                        double acti = convLayer->mactiv3d->getValueAt(x*2+x1,y*2+y1,iband) ;
                        if( acti > biggest_acti ){
                            biggest_acti = acti ;
                            x1s = x1 ;
                            y1s = y1 ;
                        }
                    }//y1
                }//x1
                poolLayer->minputweight3d->setValueAt(x*2+x1s,y*2+y1s,iband,1.0) ;
                //max pooling
                poolLayer->mactiv3d->setValueAt(x,y,iband,biggest_acti) ;
            }
        }
    }
    #ifdef DEBUG_MODE
    wft_printArray3d(poolLayer->mactiv3d,"Pool layer.");
    #endif // DEBUG_MODE
}
void ConvNetwork2::doforward(CNNPoolLayer*  poolLayer  , CNNFullConnLayer* fcLayer ) {
    int n1dpool = poolLayer->mactiv3d->get1DValueNum() ;
    int nw = fcLayer->mweight2d->getNumx() ;
    assert(n1dpool==nw) ;
    #ifdef DEBUG_MODE
    wft_printArray3d(fcLayer->mweight2d,"output weights") ;
    wft_printArray3d(fcLayer->mbias1d,"output bias") ;
    #endif // DEBUG_MODE
    for(int i = 0 ; i<fcLayer->mactiv1d->get1DValueNum() ; ++ i ){
        double out1 = fcLayer->mbias1d->get1DValueAt(i) ;
        for( int iw = 0 ; iw < nw ; ++ iw ){
            double wei = fcLayer->mweight2d->getValueAt(iw,i,0) ;
            double actipool = poolLayer->mactiv3d->get1DValueAt(iw) ;
            out1 += wei * actipool ;
        }
        //sigmoid
        double sigout = wft_sigmoid(out1) ;
        fcLayer->mactiv1d->set1DValueAt(i , sigout) ;
    }
    #ifdef DEBUG_MODE
    wft_printArray3d(fcLayer->mactiv1d,"output activ") ;
    #endif // DEBUG_MODE
}
void ConvNetwork2::doforward(CNNFullConnLayer* fcLayer , CNNFullConnLayer* outputLayer) {
    int insize = fcLayer->mactiv1d->get1DValueNum() ;
    int nw =     outputLayer->mweight2d->getNumx() ;
    assert(insize==nw) ;

    for(int i = 0 ; i<outputLayer->mactiv1d->get1DValueNum() ; ++ i ){
        double out1 = outputLayer->mbias1d->get1DValueAt(i) ;

        for( int iw = 0 ; iw < nw ; ++ iw ){
            double wei = outputLayer->mweight2d->getValueAt(iw,i,0) ;
            double actifc = fcLayer->mactiv1d->get1DValueAt(iw) ;
            out1 += wei * actifc ;

        }
        //sigmoid
        double sigout = wft_sigmoid(out1) ;
        outputLayer->mactiv1d->set1DValueAt(i , sigout) ;
    }
}

void ConvNetwork2::doforward(CNNPoolLayer*  poolLayer  , CNNConvLayer* convLayer ) {
    int pnx = poolLayer->mactiv3d->getNumx() ;
    int pny = poolLayer->mactiv3d->getNumy() ;
    int pnz = poolLayer->mactiv3d->getNumdep() ;

    int cnx = convLayer->mactiv3d->getNumx() ;
    int cny = convLayer->mactiv3d->getNumy() ;
    //int cnz = convLayer->mactiv3d->getNumdep() ;

    int knx = convLayer->mfilter3dVector[0]->getNumx() ;
    int kny = convLayer->mfilter3dVector[0]->getNumy() ;
    int knz = convLayer->mfilter3dVector[0]->getNumdep() ;

    int nfilter = (int) convLayer->mfilter3dVector.size() ;

    assert(pnz == knz) ;
    assert(pnx==pny) ;
    assert(cnx==cny) ;
    assert(knx==kny) ;
    assert(cnx == pnx-knx+1 ) ;

    for(int ifilter = 0 ; ifilter < nfilter ; ++ ifilter ){
        Array3d* filter = convLayer->mfilter3dVector[ifilter] ;
        for(int cx = 0 ; cx <cnx ; ++ cx ){
            for(int cy = 0 ; cy <cny ; ++cy ){

                double out1 = 0.0 ;
                for(int kx = 0 ; kx < knx ; ++kx ) {
                    for(int ky = 0 ; ky < kny ; ++ ky ) {
                        for(int kz = 0 ; kz < knz ; ++ kz ){
                            double wei = filter->getValueAt(kx,ky,kz) ;
                            double acti = poolLayer->mactiv3d->getValueAt(cx+kx,cy+ky,kz) ;
                            out1 += wei*acti ;
                        }
                    }
                }
                convLayer->moutput3d->setValueAt(cx,cy,ifilter,out1) ;
                //ReLU
                double relu_acti = max(0.0,out1) ;
                convLayer->mactiv3d->setValueAt(cx,cy,ifilter,relu_acti) ;
            }
        }
    }


}


void ConvNetwork2::doforwardA2B(CNNLayer* layerA,CNNLayer* layerB) {
    if( layerA->getLayerType()==CNNLayerType::CNNLayerTypeInput ){
        CNNInputLayer* inputLayer = (CNNInputLayer*)layerA ;
        if(layerB->getLayerType()==CNNLayerType::CNNLayerTypeConv){
            CNNConvLayer* convLayer = (CNNConvLayer*)layerB ;
            this->doforward(inputLayer,convLayer) ;
        }else if( layerB->getLayerType()==CNNLayerType::CNNLayerTypeFullConn ){
            CNNFullConnLayer* fcLayer = (CNNFullConnLayer*)layerB ;
            this->doforward(inputLayer,fcLayer) ;
        }else{
            cout<<"Error: 1 bad forward pair A2B."<<endl;
        }
    }else if( layerA->getLayerType()==CNNLayerType::CNNLayerTypeConv ){
        if(layerB->getLayerType()==CNNLayerType::CNNLayerTypePool){
            CNNConvLayer* convLayer = (CNNConvLayer*)layerA ;
            CNNPoolLayer* poolLayer = (CNNPoolLayer*)layerB ;
            this->doforward(convLayer,poolLayer) ;
        }else{
            cout<<"Error: 2 bad forward pair A2B."<<endl;
        }
    }else if( layerA->getLayerType()==CNNLayerType::CNNLayerTypeFullConn ){
        CNNFullConnLayer* fcLayer = (CNNFullConnLayer*)layerA ;
        if( layerB->getLayerType()==CNNLayerType::CNNLayerTypeFullConn ){
            CNNFullConnLayer* fcLayer2 = (CNNFullConnLayer*)layerB;
            this->doforward(fcLayer,fcLayer2) ;
        }else if( layerB->getLayerType()==CNNLayerType::CNNLayerTypeOutput ){
            CNNFullConnLayer* outLayer = (CNNFullConnLayer*)layerB ;
            this->doforward(fcLayer,outLayer) ;
        }else{
            cout<<"Error: 3 bad forward pair A2B."<<endl;
        }
    }else if( layerA->getLayerType()==CNNLayerType::CNNLayerTypePool ){
        CNNPoolLayer* poolLayer = (CNNPoolLayer*)layerA ;
        if( layerB->getLayerType()==CNNLayerType::CNNLayerTypeConv ){
            CNNConvLayer* convLayer = (CNNConvLayer*)layerB ;
            this->doforward(poolLayer,convLayer) ;
        }else if( layerB->getLayerType()==CNNLayerType::CNNLayerTypeFullConn ||
                  layerB->getLayerType()==CNNLayerType::CNNLayerTypeOutput ){
            CNNFullConnLayer* fcLayer = (CNNFullConnLayer*)layerB ;
            this->doforward(poolLayer,fcLayer) ;
        }else{
            cout<<"Error: 4 bad forward pair A2B."<<endl;
        }
    }else if( layerA->getLayerType()==CNNLayerType::CNNLayerTypeOutput ){
        cout<<"Error: 5 bad forward pair A2B."<<endl;
    }else{
        cout<<"Error: 6 bad forward pair A2B."<<endl;
    }
}
void ConvNetwork2::backpropagate_deltaA2B(CNNLayer* layerA,CNNLayer* layerB){
    CNNLayerType typeA = layerA->getLayerType() ;
    CNNLayerType typeB = layerB->getLayerType() ;
    if( typeA == CNNLayerType::CNNLayerTypeOutput ){
        if( typeB==CNNLayerType::CNNLayerTypeFullConn ){
            CNNFullConnLayer* outputLayer = (CNNFullConnLayer*)layerA ;
            CNNFullConnLayer* fcLayer = (CNNFullConnLayer*)layerB ;
            this->backpropagate_delta(outputLayer,fcLayer) ;
        }else if( typeB == CNNLayerType::CNNLayerTypePool ) {
            CNNFullConnLayer* outputLayer = (CNNFullConnLayer*)layerA ;
            CNNPoolLayer* pool = (CNNPoolLayer*)layerB ;
            this->backpropagate_delta(outputLayer,pool) ;
        }else{
            cout<<"Error: 1 bad back propagate A2B."<<endl;
        }
    }else if( typeA == CNNLayerType::CNNLayerTypeFullConn ){
        if( typeB==CNNLayerType::CNNLayerTypeFullConn ){
            CNNFullConnLayer* fcLayer = (CNNFullConnLayer*)layerA ;
            CNNFullConnLayer* fcLayer2 = (CNNFullConnLayer*)layerB ;
            this->backpropagate_delta(fcLayer,fcLayer2) ;
        }else if(typeB==CNNLayerType::CNNLayerTypePool){
            CNNFullConnLayer* fcLayer = (CNNFullConnLayer*)layerA ;
            CNNPoolLayer* poolLayer = (CNNPoolLayer*)layerB ;
            this->backpropagate_delta(fcLayer,poolLayer) ;
        }else{
            cout<<"Error: 2 bad back propagate A2B."<<endl;
        }
    }else if( typeA == CNNLayerType::CNNLayerTypePool ){
        if( typeB==CNNLayerType::CNNLayerTypeConv ){
            CNNPoolLayer* poolLayer = (CNNPoolLayer*)layerA ;
            CNNConvLayer* convLayer = (CNNConvLayer*)layerB ;
            this->backpropagate_delta(poolLayer,convLayer) ;
        }else{
            cout<<"Error: 3 bad back propagate A2B."<<endl;
        }
    }else if( typeA == CNNLayerType::CNNLayerTypeConv ){
        if(typeB==CNNLayerType::CNNLayerTypePool){
            CNNConvLayer* convLayer = (CNNConvLayer*)layerA ;
            CNNPoolLayer* poolLayer = (CNNPoolLayer*)layerB ;
            this->backpropagate_delta(convLayer,poolLayer) ;
        }else{
            cout<<"Error: 4 bad back propagate A2B."<<endl;
        }
    }else{
        cout<<"Error: 5 bad back propagate A2B."<<endl;
    }
}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void ConvNetwork2::backpropagate_delta(CNNOutputLayer* outputLayer, int targetIndex) {
    //use cross entropy
    #ifdef DEBUG_MODE
    cout<<"output layer delta"<<endl;
    #endif // DEBUG_MODE
    for(int i = 0 ; i<outputLayer->mdelta1d->get1DValueNum() ; ++ i ){
        double acti = outputLayer->mactiv1d->get1DValueAt(i) ;
        double y = 0.0 ;
        if( i==targetIndex ) y = 1.0 ;
        double delta1 = acti - y ;
        outputLayer->mdelta1d->set1DValueAt( i , delta1 ) ;
        #ifdef DEBUG_MODE
        printf("%8.4f %8.4f %8.4f " , acti , y , delta1 ) ;
        cout<<endl;
        #endif // DEBUG_MODE
    }
}
void ConvNetwork2::backpropagate_delta(CNNFullConnLayer* outputLayer, CNNFullConnLayer* fcLayer ) {

    for( int i = 0 ; i<fcLayer->mdelta1d->get1DValueNum() ; ++ i ){
        double sum1 = 0.0 ;

        for(int k = 0 ; k<outputLayer->mdelta1d->get1DValueNum() ; ++ k ){
            double delta_k_lplus1 = outputLayer->mdelta1d->get1DValueAt(k) ;
            double w_ki_lplus1 = outputLayer->mweight2d->getValueAt(i,k,0) ;
            sum1 += delta_k_lplus1 * w_ki_lplus1 ;

        }
        //activation function is sigmoid.
        double acti = fcLayer->mactiv1d->get1DValueAt(i) ;
        double da_vs_dz = acti*(1.0-acti) ;
        double dc_vs_dz = da_vs_dz * sum1 ;
        fcLayer->mdelta1d->set1DValueAt(i , dc_vs_dz ) ;

    }

}
void ConvNetwork2::backpropagate_delta(CNNFullConnLayer* fcLayer,   CNNPoolLayer* poolLayer ) {
    for(int i = 0 ; i<poolLayer->mactiv3d->get1DValueNum() ; ++ i ){
        double sum1 = 0.0 ;
        for(int k = 0 ; k<fcLayer->mdelta1d->get1DValueNum() ; ++ k ){
            double delta_k_lplus1 = fcLayer->mdelta1d->get1DValueAt(k) ;
            double w_ki_lplus1 = fcLayer->mweight2d->getValueAt(i,k,0) ;
            sum1 += delta_k_lplus1 * w_ki_lplus1 ;
        }
        //activation function is f(x)=x.
        double da_vs_dz = 1.0 ;
        double dc_vs_dz = da_vs_dz * sum1 ;
        poolLayer->mdelta3d->set1DValueAt(i,dc_vs_dz) ;
    }
    #ifdef DEBUG_MODE
    wft_printArray3d(poolLayer->mdelta3d,"back propagate delta to pool:") ;
    #endif // DEBUG_MODE
}
void ConvNetwork2::backpropagate_delta(CNNPoolLayer*   poolLayer,   CNNConvLayer* convLayer ) {

    int cnx = convLayer->mdelta3d->getNumx() ;
    int cny = convLayer->mdelta3d->getNumy() ;
    int cnz = convLayer->mdelta3d->getNumdep() ;

    int pnx = poolLayer->mdelta3d->getNumx() ;
    int pny = poolLayer->mdelta3d->getNumy() ;
    int pnz = poolLayer->mdelta3d->getNumdep() ;

    assert(pnz == cnz) ;
    assert(cnx==cny) ;
    assert(pnx==pny) ;
    assert(cnx/2==pnx) ;

    for(int cz = 0 ; cz < cnz ; ++ cz ){
        for(int cx = 0 ; cx < cnx ; ++ cx ){
            for(int cy = 0; cy < cny ; ++cy ){
                //max pooling used in forward step.
                double inputweight = poolLayer->minputweight3d->getValueAt(cx,cy,cz) ;
                if( inputweight > 0.99 ){
                    double convoutput = convLayer->moutput3d->getValueAt(cx,cy,cz) ;
                    //ReLU used at in forward step.
                    if( convoutput > 0 ){
                        double deltapool = poolLayer->mdelta3d->getValueAt(cx/2,cy/2,cz) ;
                        convLayer->mdelta3d->setValueAt(cx,cy,cz,deltapool) ;
                    }else{
                        convLayer->mdelta3d->setValueAt(cx,cy,cz,0.0) ;
                    }
                }else{
                    convLayer->mdelta3d->setValueAt(cx,cy,cz,0.0) ;
                }
            }
        }
    }
    #ifdef DEBUG_MODE
    wft_printArray3d(convLayer->mdelta3d,"back propagate delta to conv:") ;
    #endif // DEBUG_MODE
}


void ConvNetwork2::backpropagate_delta(CNNConvLayer*   convLayer,   CNNPoolLayer* poolLayer ) {

    int pnx = poolLayer->mdelta3d->getNumx() ;
    int pny = poolLayer->mdelta3d->getNumy() ;
    int pnz = poolLayer->mdelta3d->getNumdep() ;

    int cnx = convLayer->mdelta3d->getNumx() ;
    int cny = convLayer->mdelta3d->getNumy() ;
    int cnz = convLayer->mdelta3d->getNumdep() ;

    int knx = convLayer->mfilter3dVector[0]->getNumx() ;
    int kny = convLayer->mfilter3dVector[0]->getNumy() ;
    int knz = convLayer->mfilter3dVector[0]->getNumdep() ;

    //int nfilter = (int)convLayer->mfilter3dVector.size() ;

    assert(cnx==cny) ;
    assert(pnx==pny) ;
    assert(cnx == pnx - knx + 1 ) ;
    assert(pnz==knz) ;

    for( int pz = 0 ; pz < pnz ; ++ pz ){
        for(int px = 0 ; px < pnx ; ++ px ) {
            for(int py = 0 ; py < pny ; ++ py ){

                double sum1 = 0.0 ;
                for(int cz = 0 ; cz < cnz ; ++ cz ){
                    Array3d* filter3d = convLayer->mfilter3dVector[cz] ;
                    for(int kx = 0 ; kx <knx ; ++ kx ){
                        for(int ky = 0 ; ky < kny ; ++ ky ){
                            int xinc = px - kx ;
                            int yinc = py - ky ;
                            if( xinc>=0 && yinc>=0 && xinc < cnx && yinc < cny ){
                                double delta_conv = convLayer->mdelta3d->getValueAt(xinc,yinc,cz) ;
                                double wei = filter3d->getValueAt(kx,ky,pz) ;
                                sum1 += delta_conv * wei ;
                            }else{
                                //out of bound in convolution layer.
                                sum1 += 0.0 ;
                            }
                        }
                    }
                }
            }
        }
    }
}

void ConvNetwork2::doforward() {
    for(vector<CNNLayer*>::iterator it = mLayerVector.begin() ;
        it != mLayerVector.end()-1 ; ++it ){
        CNNLayer* layerA = *it ;
        CNNLayer* layerB = *(it+1) ;
        this->doforwardA2B(layerA,layerB) ;
    }
}
void ConvNetwork2::dobackpropagation(int targetIndex) {

    CNNLayer* lastLayer = *( mLayerVector.end()-1 ) ;
    assert(lastLayer->getLayerType()==CNNLayerType::CNNLayerTypeOutput);
    CNNOutputLayer* outputLayer = (CNNOutputLayer*)lastLayer ;
    this->backpropagate_delta(outputLayer,targetIndex) ;

    for(vector<CNNLayer*>::iterator it = mLayerVector.end()-1 ;
        it != mLayerVector.begin()+1 ; --it ){
        CNNLayer* layerA = *it ;
        CNNLayer* layerB = *(it-1) ;
        this->backpropagate_deltaA2B(layerA,layerB) ;
    }
}
void ConvNetwork2::computeAndSumPartialDerivForWeightsAndBias() {
    for(unsigned int ilayer = 1 ; ilayer < this->mLayerVector.size() ; ++ ilayer ){
        CNNLayer* clayer = this->mLayerVector[ilayer] ;
        CNNLayerType type = clayer->getLayerType() ;

        CNNLayer* prevlayer = this->mLayerVector[ilayer-1] ;
        CNNLayerType prevtype = prevlayer->getLayerType() ;

        if(type==CNNLayerType::CNNLayerTypeConv){
            CNNConvLayer* convlayer = (CNNConvLayer*)clayer ;
            if(prevtype==CNNLayerType::CNNLayerTypeInput){
                CNNInputLayer* inputlayer = (CNNInputLayer*)prevlayer ;
                convlayer->computeAndSumFilterDeltaWeights(inputlayer->mdata3d) ;
            }else if(prevtype==CNNLayerType::CNNLayerTypePool){
                CNNPoolLayer* poollayer = (CNNPoolLayer*)prevlayer ;
                convlayer->computeAndSumFilterDeltaWeights(poollayer->mactiv3d) ;
            }else{
                cout<<"Error: 1 compute dw sum."<<endl;
            }
        }else if(type==CNNLayerType::CNNLayerTypeFullConn || type==CNNLayerType::CNNLayerTypeOutput ){
            CNNFullConnLayer* fclayer = (CNNFullConnLayer*)clayer ;
            if(prevtype==CNNLayerType::CNNLayerTypeInput){
                CNNInputLayer* inputlayer = (CNNInputLayer*)prevlayer ;
                fclayer->computeAndSumDeltaWeightsAndBias(inputlayer->mdata3d) ;
            }else if(prevtype==CNNLayerType::CNNLayerTypePool){
                CNNPoolLayer* poollayer = (CNNPoolLayer*)prevlayer ;
                fclayer->computeAndSumDeltaWeightsAndBias(poollayer->mactiv3d) ;
            }else if(prevtype==CNNLayerType::CNNLayerTypeFullConn){
                CNNFullConnLayer* prevfclayer = (CNNFullConnLayer*)prevlayer ;
                fclayer->computeAndSumDeltaWeightsAndBias(prevfclayer->mactiv1d) ;
            }else{
                cout<<"Error: 2 compute dw sum."<<endl;
            }
        }
    }
}
void ConvNetwork2::updateWeightsAndBias(const double studyRate ,const double momentum ) {
    for(unsigned int ilayer = 1 ; ilayer < this->mLayerVector.size() ; ++ ilayer ){
        CNNLayer* clayer = this->mLayerVector[ilayer] ;
        CNNLayerType type = clayer->getLayerType() ;
        if(type==CNNLayerType::CNNLayerTypeConv){
            CNNConvLayer* convlayer = (CNNConvLayer*)clayer ;
            convlayer->updateFilterWeights(studyRate,momentum) ;
        }else if(type==CNNLayerType::CNNLayerTypeFullConn || type==CNNLayerType::CNNLayerTypeOutput ){
            CNNFullConnLayer* fclayer = (CNNFullConnLayer*)clayer ;
            fclayer->updateWeightsAndBias(studyRate,momentum) ;
        }
    }
}



void ConvNetwork2::saveToDisk(const char* filepath) {
    FILE* pf = fopen(filepath , "w") ;
    assert(pf) ;
    char buff[100] ;
    for(unsigned int i = 0 ; i<mLayerVector.size() ; ++ i ){
        sprintf(buff,"%s.layer%d.txt" ,filepath, i) ;
        CNNLayer* layer = mLayerVector[i] ;
        layer->saveToDisk(buff) ;
        fprintf(pf,"%d\n" , layer->getLayerType() ) ;
        fprintf(pf,"%s\n" , buff) ;
    }
    fclose(pf);
    pf = 0 ;
}
