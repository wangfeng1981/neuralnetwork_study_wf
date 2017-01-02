#include <iostream>
#include <stdio.h>
#include <iomanip>
#include "wImage.h"


using namespace std;



int main_deprecated()
{
    cout << "Hello jfwf 2016 dec!" << endl;
    cout<<"input num of class"<<endl;
    int numclass (0) ;
    cin>>numclass ;
    cout<<"You have "<<numclass<<" class"<<endl;
    for(int ic = 0 ; ic<numclass ; ++ ic ){
        int i0 (0) , i1(0) ;
        cout<<"input i0 - i1 for class "<<ic<<endl ;
        cin>>i0>>i1 ;
        char buff[100] ;
        for( int ifile = i0 ; ifile <=i1 ; ++ifile ){
            sprintf(buff , "../train_samples/%d_%d.png" , ic , ifile ) ;
            printf("%s\n" , buff) ;
        }
    }
    wImage img0("../blocks/0_20.png") ;
    cout<<img0.getRows()<<","<<img0.getCols()<<endl;
    getchar() ;
    return 0;
}
