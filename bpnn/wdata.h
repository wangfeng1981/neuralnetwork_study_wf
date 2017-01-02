#include <vector>


#ifndef WDATA_H_
#define WDATA_H_

class WData {
public :
	WData( double* xarr , int nx , double realy ,int outN) ;
	WData( unsigned char* xarr , int nx , double realy , double norm , int outN ) ;
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
