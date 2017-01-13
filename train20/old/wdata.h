#include <vector>
#include "wImage.h"

#ifndef WDATA_H_
#define WDATA_H_

class WData {
public :
	WData( double* xarr , int nx , double realy ,int outN) ;
	WData( unsigned char* xarr , int nx , double realy , double norm , int outN ) ;
	WData( wImage* img ,  double realy ,double norm ,int outN) ;
	WData( wImage* img , const int irow0,const int icol0,
            const int nrows,const int ncols,
            double realy ,double norm ,int outN) ;
	int getNumOfData() ;
	double getDataAt(int i) ;
	double getY() ;
	std::vector<double> vy ;
	int noNeedMoreCalculate ;
	int originIndex ;
private:
	std::vector<double> vx ;
	double y ;
} ;



#endif
