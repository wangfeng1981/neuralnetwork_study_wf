#ifndef WFTGRAPHBASEIMAGESEG_H
#define WFTGRAPHBASEIMAGESEG_H

#include <vector>
#include <string>
#include <cmath>
#include "../train20/wImage.h"
#include <list>
#include <memory>

using std::vector ;
using std::string ;
using std::list ;
using std::shared_ptr ;

struct wftGraphVertex ;
struct wftRegion ;
struct wftGraphEdge ;

struct wftGraphEdge{
    inline wftGraphEdge():vertex0Ptr(nullptr),vertex1Ptr(nullptr){} ;
    wftGraphVertex* vertex0Ptr ;
    wftGraphVertex* vertex1Ptr ;
    double weight ;
};
struct wftGraphVertex{
    inline wftGraphVertex():regionPtr(nullptr){} ;
    int icol,irow ;
    double r,g,b ;
    wftRegion* regionPtr ;
    vector<wftGraphEdge*> edges ;
    double colorDiff( wftGraphVertex& other ) ;
};

struct wftRegion{
    vector<wftGraphVertex*> vertices ;
    double threshold ;
};

struct wftGraph{
    vector<wftGraphVertex*> vertices ;
    vector<wftGraphEdge*> edges ;
    list<wftRegion*> regions ;

    ~wftGraph() ;
    void reset() ;
    void segmentImage( wImaged& image  , const int k , const int minsize) ;
    wftGraphVertex* getVertexPtr(int irow,int icol) ;
private:
    int nCols , nRows ;
    wftRegion* joinTwoRegion( wftRegion* rptr0 , wftRegion* rptr1 ) ;
};


#endif // WFTGRAPHBASEIMAGESEG_H
