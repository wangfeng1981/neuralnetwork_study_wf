
#include "wdata.h"
#include <assert.h>
#include <cstddef>


int WData::getNumOfData() {
	return (int) vx.size()  ;
}
double WData::getDataAt(int i) {
	return vx[i] ;
}

WData::WData(double* xarr , int nx , double realy , int outN ) {
	assert( xarr!=NULL ) ;
	for(int i = 0 ; i<nx ; ++ i ){
		//double r = ((double) rand() / (RAND_MAX) * 10.0 ) ;
		vx.push_back( xarr[i] ) ;
	}
	this->y = realy ;
	for(int i = 0; i<outN ; ++i ){
        if(i==this->y){
            this->vy.push_back(1);
        }else{
            this->vy.push_back(0) ;
        }
	}
	this->noNeedMoreCalculate = 0 ;

}

WData::WData(unsigned char * xarr , int nx , double realy , double norm , int outN) {
	assert( xarr!=NULL ) ;
    if( norm > 0 ){
        for(int i = 0 ; i<nx ; ++ i ){
            double v1 = (double) xarr[i] / norm ;
            vx.push_back( v1 ) ;
        }
    }else{
        for(int i = 0 ; i<nx ; ++ i ){
            //double r = ((double) rand() / (RAND_MAX) * 10.0 ) ;
            double v1 = (double) xarr[i] ;
            vx.push_back( v1 ) ;
        }
    }

	this->y = realy ;
	for(int i = 0; i<outN ; ++i ){
        if(i==this->y){
            this->vy.push_back(1);
        }else{
            this->vy.push_back(0) ;
        }
	}

	this->noNeedMoreCalculate = 0 ;
}

WData::WData( wImage* img  , double realy ,double norm ,int outN) {
    for(int ir = 0 ; ir <img->getRows() ; ++ir ){
        for( int ic = 0 ; ic<img->getCols() ; ++ic ) {
            int r(0),g(0),b(0) ;
            img->getRGB(ir,ic,r,g,b) ;
            this->vx.push_back(r/norm) ;
            this->vx.push_back(g/norm) ;
            this->vx.push_back(b/norm) ;
        }
    }
	this->y = realy ;
	for(int i = 0; i<outN ; ++i ){
        if(i==this->y){
            this->vy.push_back(1);
        }else{
            this->vy.push_back(0) ;
        }
	}
	this->noNeedMoreCalculate = 0 ;
}

WData::WData( wImage* img , int irow0,int icol0,int nrows,int ncols,
            double realy ,double norm ,int outN) {
    for(int ir = irow0 ; ir < irow0 + nrows ; ++ir ){
        for( int ic = icol0 ; ic< icol0 + ncols ; ++ic ) {
            int r(0),g(0),b(0) ;
            img->getRGB(ir,ic,r,g,b) ;
            this->vx.push_back(r/norm) ;
            this->vx.push_back(g/norm) ;
            this->vx.push_back(b/norm) ;
        }
    }
	this->y = realy ;
	for(int i = 0; i<outN ; ++i ){
        if(i==this->y){
            this->vy.push_back(1);
        }else{
            this->vy.push_back(0) ;
        }
	}
	this->noNeedMoreCalculate = 0 ;
}

double WData::getY() {
	return this->y ;
}
