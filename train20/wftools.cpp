#include "wftools.h"
#include "array3d.h"
#include <assert.h>

std::vector<std::string> wft_split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        if (end != start) {
          tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start && start < text.length() ) {
       tokens.push_back(text.substr(start));
    }
    return tokens;
}


std::vector<double> wft_split2d( const std::string &text, char sep ){
    std::vector<std::string> vstr = wft_split(text , sep) ;
    std::vector<double> vd ;
    for(unsigned int i = 0 ; i<vstr.size() ; ++ i ){
        double val=0 ;
        std::stringstream stream( vstr[i] );
        stream >> val ;
        if (stream.fail()) {
            std::cout<<"bad string to double."<<endl;
            std::cout<<vstr[i]<<endl;
            exit(10) ;
        }
        vd.push_back(val) ;
    }
    return vd ;
}

std::vector<int> wft_split2i( const std::string &text, char sep ){
    std::vector<std::string> vstr = wft_split(text , sep) ;
    std::vector<int> vd ;
    for(unsigned int i = 0 ; i<vstr.size() ; ++ i ){
        int val=0 ;
        std::cout<<(int)(vstr[i].c_str()[0])<<std::endl;
        std::stringstream stream( vstr[i] );
        stream >> val ;
        if (stream.fail()) {
            std::cout<<"bad string to int."<<endl;
            std::cout<<vstr[i]<<endl;
            exit(10) ;
        }
        vd.push_back(val) ;
    }
    return vd ;
}

double wft_gaussrand( double muval , double stdval ){
	static double V1, V2, S;
	static int phase = 0;
	double X;
	if(phase == 0) {
		do {
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;
			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
        }
        while(S >= 1 || S == 0);
		X = V1 * sqrt(-2 * log(S) / S);
	} else {
	    X = V2 * sqrt(-2 * log(S) / S);
	}
	phase = 1 - phase;
	return muval + X * stdval ;
}

double wft_sigmoid(double z) {
    return 1.0/(1.0+exp(-z))  ;
}

int wft_randFromZeroToN(int n) {
    assert(n>0) ;
    return rand() % n;
}

void wft_backpropagation_pool2conv(
                           Array3d*       convDelta3d ,
                           const Array3d* poolDelta3d ,
                           const int      filterIndex ,
                           const Array3d* convOutput3d ,
                           const int     activfunctype )
{
    //activfunctype 暂时不考虑输入，先使用ReLU

    assert( convDelta3d->getNumx()   == convOutput3d->getNumx() ) ;
    assert( convDelta3d->getNumy()   == convOutput3d->getNumy() ) ;
    assert( convDelta3d->getNumdep() == convOutput3d->getNumdep() ) ;

    assert( convDelta3d->getNumx() == convDelta3d->getNumy() ) ;
    assert( poolDelta3d->getNumx() == poolDelta3d->getNumy() ) ;
    assert( convDelta3d->getNumdep() == poolDelta3d->getNumdep() ) ;

    int m = convDelta3d->getNumx() ;
    int n = poolDelta3d->getNumx() ;
    int k = 0;//
    assert(k) ;
    int nb = convDelta3d->getNumdep() ;//number of band

    for( int x = 0 ; x < m ; ++ x ){
        for(int y = 0 ; y < m ; ++y ){
            for(int z = 0 ; z<nb ; ++ z ){
                double delta_xyz = 0.0 ;
                double dfz = 0.0 ;//ReLU derivation
                if( convOutput3d->getValueAt(x,y,z) > 0 ){
                    dfz = 1.0 ;
                }else {
                    dfz = 0.0 ;
                }
                for( int kx = 0 ; kx<k ; ++ kx ){
                    for(int ky = 0 ; ky<k ; ++ ky ){
                        int i_x = x - kx ;
                        int j_y = y - ky ;
                        double delta1 = 0.0 ;
                        if( i_x >=0 && j_y >=0 && i_x < n && j_y < n ){
                            delta1 = poolDelta3d->getValueAt(i_x,j_y,z) ;
                        }else{
                            delta1 = 0.0 ;
                        }



                    }
                }
            }
        }
    }
}
