#include <iostream>
#include <vector>
#include "../train20/wImage.h"
#include <cmath>
#include <algorithm>
#include <set>
#include "wftgraphbaseimageseg.h"
#include <cstdio>

using namespace std;


bool fnWftEdgeCompare( const wftGraphEdge& e0 , const wftGraphEdge& e1 ){
    if( e0.weight < e1.weight ) return true ;
    else return false ;
}



int main()
{
    cout << "Graph based image segmentation by jfwf@yeah.net 20170222." << endl;
    string inputimage ;
    cout<<"input png image path:"<<endl ;
    cin>>inputimage ;
    cout<<"input sigma(for gauss smooth) k(for init region weight) minsize(for merge small region):"<<endl ;
    double sigma ;
    int k , minsize ;
    cin>>sigma>>k>>minsize ;
    cout<<"Your input:"<<endl ;
    cout<<inputimage<<endl;
    cout<<sigma<<endl;
    cout<<k<<endl;
    cout<<minsize<<endl;

    wImaged image(inputimage.c_str()) ;
    wImaged image2 = image.gaussBlur(sigma) ;
    cout<<"gauss smooth done."<<endl;
    wftGraph graph ;
    graph.segmentImage(image2 , k , minsize ) ;

    wImaged image3( image.getRows() , image.getCols() ) ;
    wftRegion* pRegion = graph.regions.rootNode ;
    while( pRegion){
        double r = rand()%256 ;
        double g = rand()%256 ;
        double b = rand()%256 ;
        for( size_t iv = 0 ; iv < pRegion->vertices.size() ; ++ iv ){
            int row = pRegion->vertices[iv]->irow ;
            int col = pRegion->vertices[iv]->icol ;
            image3.setRGB( row , col , r,g,b ) ;
        }
        pRegion = pRegion->nextNode ;
    }
    /*for( wftRegion* rptr : graph.regions ){
        double r = rand()%256 ;
        double g = rand()%256 ;
        double b = rand()%256 ;
        for( size_t iv = 0 ; iv < rptr->vertices.size() ; ++ iv ){
            int row = rptr->vertices[iv]->irow ;
            int col = rptr->vertices[iv]->icol ;
            image3.setRGB( row , col , r,g,b ) ;
        }
    }*/
    string outpath = inputimage + ".segout.png" ;
    image3.saveToFile(outpath) ;
    cout<<"Result in "<<outpath<<endl ;
    cout<<"All finished."<<endl ;
    getchar() ;
    getchar() ;
    getchar() ;
    getchar() ;
    return 0;
}
