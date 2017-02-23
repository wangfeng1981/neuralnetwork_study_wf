#include <iostream>
#include <vector>
#include "../train20/wImage.h"
#include <cmath>
#include <algorithm>
#include <set>
#include "wftgraphbaseimageseg.h"

using namespace std;


bool fnWftEdgeCompare( const wftGraphEdge& e0 , const wftGraphEdge& e1 ){
    if( e0.weight < e1.weight ) return true ;
    else return false ;
}



int main()
{
    cout << "Graph based image segmentation by jfwf@yeah.net 20170222." << endl;

    wImaged image("beach.png") ;
    wImaged image2 = image.gaussBlur(0.5) ;
    cout<<"gauss smooth done."<<endl;
    wftGraph graph ;
    graph.segmentImage(image2 , 500 , 50 ) ;

    wImaged image3( image.getRows() , image.getCols() ) ;
    for( wftRegion* rptr : graph.regions ){
        double r = rand()%256 ;
        double g = rand()%256 ;
        double b = rand()%256 ;
        for( size_t iv = 0 ; iv < rptr->vertices.size() ; ++ iv ){
            int row = rptr->vertices[iv]->irow ;
            int col = rptr->vertices[iv]->icol ;
            image3.setRGB( row , col , r,g,b ) ;
        }
    }
    image3.saveToFile("segmented.png") ;

    return 0;
}
