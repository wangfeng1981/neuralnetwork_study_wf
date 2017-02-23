#include "wftgraphbaseimageseg.h"

#include <algorithm>
#include <iostream>
#include <cstdio>
#include <cassert>

using std::sort ;
using std::cout ;
using std::endl ;

double wftGraphVertex::colorDiff(wftGraphVertex& other){
    double d0 = this->r - other.r ;
    double d1 = this->g - other.g ;
    double d2 = this->b - other.b ;
    return sqrt( d0*d0+d1*d1+d2*d2 ) ;
}

void wftGraph::reset(){
    this->nCols = 0 ;
    this->nRows = 0 ;
    for( size_t i = 0 ; i< this->vertices.size() ; ++ i  ){
        if( this->vertices[i] ){
            delete this->vertices[i] ;
            this->vertices[i] = nullptr ;
        }
    }
    for( size_t i = 0 ; i< this->edges.size() ; ++ i  ){
        if( this->edges[i] ){
            delete this->edges[i] ;
            this->edges[i] = nullptr ;
        }
    }
    for( auto it = regions.begin() ; it != regions.end() ; ++ it  ){
        wftRegion* rptr = *it ;
        if( rptr ){
            delete rptr ;
            *it = nullptr ;
        }
    }
}

wftGraph::~wftGraph() {
    this->reset() ;
}

wftGraphVertex* wftGraph::getVertexPtr(int irow,int icol) {
    int index = irow * this->nCols + icol ;
    if( index < 0 || index >= (int)this->vertices.size() ){
        return nullptr ;
    }else{
        return this->vertices[index] ;
    }
}

bool wftFnEdgeCompare( const wftGraphEdge* e0 , const wftGraphEdge* e1 ){
    if( e0->weight < e1->weight ) return true ;
    else return false ;
}

wftRegion* wftGraph::joinTwoRegion( wftRegion* rptr0 , wftRegion* rptr1 ) {
    //assert(rptr0) ;
    //assert(rptr1) ;
    //assert(rptr0!=rptr1) ;
    for( wftGraphVertex* vptr : rptr1->vertices ){
        vptr->regionPtr = rptr0 ;
        rptr0->vertices.push_back(vptr) ;
    }
    rptr1->vertices.clear() ;
    return rptr0 ;
}

void wftGraph::segmentImage( wImaged& image , const int k , const int minsize ) {

    this->reset() ;
    this->nCols = image.getCols() ;
    this->nRows = image.getRows() ;

    const int nrows = image.getRows() ;
    const int ncols = image.getCols() ;
    for( int irow = 0 ; irow < nrows ; ++ irow ){
        for(int icol = 0 ; icol < ncols ; ++ icol ){
            wftGraphVertex* vptr = new wftGraphVertex() ;
            vptr->icol = icol ;
            vptr->irow = irow ;
            image.getRGB(irow,icol , vptr->r, vptr->g, vptr->b ) ;
            this->vertices.push_back(vptr) ;
        }
    }
    cout<<"vertices done"<<endl ;
    for( int irow = 0 ; irow < nrows ; ++ irow ){
        for(int icol = 0 ; icol < ncols ; ++ icol ){
            wftGraphVertex* vptr  = this->getVertexPtr( irow , icol ) ;
            wftGraphVertex* vptr1 = this->getVertexPtr( irow-1 , icol+1 ) ;
            wftGraphVertex* vptr2 = this->getVertexPtr( irow , icol+1 ) ;
            wftGraphVertex* vptr3 = this->getVertexPtr( irow+1 , icol+1 ) ;
            wftGraphVertex* vptr4 = this->getVertexPtr( irow+1 , icol ) ;
            if( vptr1 ){
                wftGraphEdge* eptr = new wftGraphEdge() ;
                eptr->vertex0Ptr = vptr  ;
                eptr->vertex1Ptr = vptr1 ;
                eptr->weight = vptr->colorDiff( *vptr1 ) ;
                this->edges.push_back(eptr) ;
                vptr->edges.push_back(eptr) ;
            }
            if( vptr2 ){
                wftGraphEdge* eptr = new wftGraphEdge() ;
                eptr->vertex0Ptr = vptr  ;
                eptr->vertex1Ptr = vptr2 ;
                eptr->weight = vptr->colorDiff( *vptr2 ) ;
                this->edges.push_back(eptr) ;
                vptr->edges.push_back(eptr) ;
            }
            if( vptr3 ){
                wftGraphEdge* eptr = new wftGraphEdge() ;
                eptr->vertex0Ptr = vptr  ;
                eptr->vertex1Ptr = vptr3 ;
                eptr->weight = vptr->colorDiff( *vptr3 ) ;
                this->edges.push_back(eptr) ;
                vptr->edges.push_back(eptr) ;
            }
            if( vptr4 ){
                wftGraphEdge* eptr = new wftGraphEdge() ;
                eptr->vertex0Ptr = vptr  ;
                eptr->vertex1Ptr = vptr4 ;
                eptr->weight = vptr->colorDiff( *vptr4 ) ;
                this->edges.push_back(eptr) ;
                vptr->edges.push_back(eptr) ;
            }
            //region
            wftRegion* rptr = new wftRegion() ;
            rptr->threshold = k ;
            rptr->vertices.push_back(vptr) ;
            vptr->regionPtr = rptr ;
            this->regions.push_back(rptr) ;
        }
    }
    cout<<"edges and regions done "<<this->edges.size()<<","<<this->regions.size()<<endl;
    //segmentation
    sort( this->edges.begin() , this->edges.end() , wftFnEdgeCompare ) ;

    for( size_t ie = 0 ; ie < this->edges.size() ; ++ie ){
        wftGraphEdge* currEdgePtr = edges[ie] ;
        wftGraphVertex* vert0Ptr = currEdgePtr->vertex0Ptr ;
        wftGraphVertex* vert1Ptr = currEdgePtr->vertex1Ptr ;
        if( vert0Ptr->regionPtr != vert1Ptr->regionPtr ){
            wftRegion* rptr0 = vert0Ptr->regionPtr ;
            wftRegion* rptr1 = vert1Ptr->regionPtr ;
            if( currEdgePtr->weight <= rptr0->threshold && currEdgePtr->weight <= rptr1->threshold ){
                //join two region
                wftRegion* joinRegion = this->joinTwoRegion( rptr0 , rptr1 ) ;
                joinRegion->threshold = currEdgePtr->weight + k*1.0 / joinRegion->vertices.size() ;
                //cout<<"joined by edge "<<ie<<endl;
            }
        }
    }
    for( auto it = this->regions.begin() ; it != this->regions.end() ;  ){
        wftRegion* rptr = *it ;
        if(rptr->vertices.size()==0){
            it = this->regions.erase(it) ;
            delete rptr ;
        }else{
            ++ it ;
        }
    }
    cout<<"first segment, region count:"<<this->regions.size()<<endl ;
    for(size_t ie = 0 ; ie < edges.size() ; ++ ie ){
        wftGraphEdge* currEdgePtr = edges[ie] ;
        wftGraphVertex* vert0Ptr = currEdgePtr->vertex0Ptr ;
        wftGraphVertex* vert1Ptr = currEdgePtr->vertex1Ptr ;
        if( vert0Ptr->regionPtr != vert1Ptr->regionPtr ){
            wftRegion* rptr0 = vert0Ptr->regionPtr ;
            wftRegion* rptr1 = vert1Ptr->regionPtr ;
            if( (int)rptr0->vertices.size() <= minsize || (int)rptr1->vertices.size() <= minsize ){
                wftRegion* joinRegion = this->joinTwoRegion( rptr0 , rptr1 ) ;
                joinRegion->threshold = currEdgePtr->weight + k / joinRegion->vertices.size() ;
            }
        }
    }
    for( auto it = this->regions.begin() ; it != this->regions.end() ;  ){
        wftRegion* rptr = *it ;
        if(rptr->vertices.size()==0){
            it = this->regions.erase(it) ;
            delete rptr ;
        }else{
            ++it ;
        }
    }
    cout<<"second segment, region count:"<<this->regions.size()<<endl ;
}




/*
vector<wftRegion> wftGraphBaseImageSeg::segmentGraph( wftGraph& graph ,const float k ,const float regionMin ){
    const int numVert = graph.vertices.size() ;
    vector<wftGraphEdge> edges  ;
    for(int i = 0 ; i<numVert ; ++ i ){
        for(auto& e1: graph.vertices[i].edges ){
            edges.push_back(e1) ;
        }
    }
    sort( edges.begin() , edges.end() , wftFnEdgeCompare ) ;

    vector<wftRegion> regions ;
    vector<int> vertexRegionIds ;
    for(int i = 0 ; i<numVert ; ++ i ){
        wftRegion r1 ;
        r1.vertices.push_back(i) ;
        r1.threshold = k ;
        regions.push_back(r1) ;
        vertexRegionIds.push_back(i) ;
    }

    for(size_t i = 0 ; i<edges.size() ; ++ i ){
        wftGraphEdge& edge1 = edges[i] ;
        int id0 = vertexRegionIds[edge1.index0] ;
        int id1 = vertexRegionIds[edge1.index1] ;
        if( id0 != id1 ){
            //try merge it
            float innerdiff0 = regions[id0].getInnerDiff() ;
            float innerdiff1 = regions[id1].getInnerDiff() ;
            float outerdiff = edge1.weight ;
            if( outerdiff <= innerdiff0 && outerdiff <= innerdiff1 ){
                for(int ii = 0 ; ii< (int)regions[id1].vertices.size() ; ++ ii ){
                    int vindex = regions[id1].vertices[ii] ;
                    vertexRegionIds[vindex] = id0 ;
                    regions[id0].vertices.push_back(vindex) ;
                }
                regions[id0].threshold = edge1.weight + k/regions[id0].vertices.size() ;
                regions[id1].vertices.clear() ;
            }
        }
    }

    int nvalid = 0 ;
    for(int i = 0 ; i<regions.size() ; ++ i ){
        if( regions[i].vertices.size()>0 ){
            ++ nvalid ;
        }
    }
    cout<<"all region num:"<<regions.size()<<endl ;
    cout<<"first wft segment, valid region num:"<<nvalid<<endl ;

    for(int ie = 0 ; ie < edges.size() ; ++ ie ){
        wftGraphEdge& edge1 = edges[ie]  ;
        int regionid0 = vertexRegionIds[edge1.index0] ;
        int regionid1 = vertexRegionIds[edge1.index1] ;
        if( regionid0 != regionid1 ){
            int vsize0 = regions[regionid0].vertices.size() ;
            int vsize1 = regions[regionid1].vertices.size() ;
            if( vsize0 <= regionMin || vsize1 <= regionMin ){
                for(int ii = 0 ; ii<vsize1 ; ++ ii ){
                    int vindex = regions[regionid1].vertices[ii] ;
                    vertexRegionIds[vindex] = regionid0 ;
                    regions[regionid0].vertices.push_back(vindex) ;
                }
                regions[regionid1].vertices.clear() ;
            }
        }
    }

    vector<wftRegion> regions2 ;
    for( auto it = regions.begin() ; it != regions.end() ; ++ it ){
        if( (*it).vertices.size() > 0 ){
            regions2.push_back( *it ) ;
        }
    }
    cout<<"second wft segment, region num:"<<regions2.size()<<endl ;
    return regions2 ;
}
*/
