#include "netlayer.h"
#include <iostream>
#include <assert.h>
#include <string.h>

using namespace std;

NetLayer::NetLayer(const int outputXnum ,const int outputYnum ,const int outputDepnum )
{
    assert(outputXnum>0);
    assert(outputYnum>0);
    assert(outputDepnum>0) ;

    this->mpOutputData = new Array3d(outputXnum, outputYnum , outputDepnum,false) ;
    this->mpOutputActi = new Array3d(outputXnum,outputYnum,outputDepnum,false) ;
    this->mpDelta = new Array3d(outputXnum,outputYnum,outputDepnum,false) ;

}

NetLayer::~NetLayer()
{
    cout<<"net layer dealloc."<<endl;
    if( this->mpOutputData ){
        delete this->mpOutputData ;
        this->mpOutputData = 0 ;
    }
    if( this->mpOutputActi ){
        delete this->mpOutputActi ;
        this->mpOutputActi =0 ;
    }

    if( this->mpDelta ){
        delete mpDelta ;
        this->mpDelta = 0;
    }
}

int NetLayer::getOutXnum() const {
    return this->mpOutputData->getNumx() ;
}
int NetLayer::getOutYnum() const {
    return this->mpOutputData->getNumy() ;
}
int NetLayer::getOutDepnum() const {
    return this->mpOutputData->getNumdep() ;
}
double NetLayer::getOutputAt(int ix,int iy,int idep) const {
    return this->mpOutputData->getValueAt(ix,iy,idep) ;
}
void NetLayer::computeForewardOutput(const NetLayer* prevLayer ) {

}

int NetLayer::get1DOutputNum() const {
    return this->mpOutputData->get1DValueNum()  ;
}
double NetLayer::get1DOutputAt(int i ) const {
    return this->mpOutputData->get1DValueAt(i) ;
}

void NetLayer::print() {
    cout<<"Net Layer Print()."<<endl;
}

bool NetLayer::hasWeightsAndBias() {
    return true ;
}

double NetLayer::getDeltaAt(int ix,int iy,int idep) const {
    return mpDelta->getValueAt(ix,iy,idep) ;
}
double NetLayer::get1DDeltaAt(int i)const {
    return mpDelta->get1DValueAt(i);
}

double NetLayer::getActivaAt(int ix,int iy,int idep) const {
    return mpOutputActi->getValueAt(ix,iy,idep) ;
}
double NetLayer::get1DActivaAt(int i )const {
    return mpOutputActi->get1DValueAt(i) ;
}
void NetLayer::setOutputAt(int ix,int iy,int idep, double newval) {
    mpOutputData->setValueAt(ix,iy,idep,newval) ;
}
void NetLayer::setActivaAt(int ix,int iy,int idep, double newval) {
    mpOutputActi->setValueAt(ix,iy,idep,newval) ;
}
void NetLayer::setDeltaAt(int ix,int iy,int idep, double newval) {
    mpDelta->setValueAt(ix,iy,idep,newval) ;
}
void NetLayer::set1DDeltaAt (int i, double newval) {
    mpDelta->set1DValueAt(i,newval) ;
}
void NetLayer::zeroOutput() {
    this->mpOutputData->zero() ;
}
void NetLayer::zeroActiva() {
    this->mpOutputActi->zero() ;
}
void NetLayer::zeroDelta() {
    this->mpDelta->zero() ;
}

double NetLayer::getWeightsAt(int ix,int iy,int idep) const {
    cout<<"Error: A netlayer without weights."<<endl;
    return 0 ;
}
void NetLayer::computeAndSumPartialDerivativeOfWeightsAndBias(
                                                              const NetLayer* prevlayer,
                                                              const NetLayer* nextlayer,
                                                              const int outputTargetIndex) {
    cout<<"Error: empty computeAndSumPartialDerivativeOfWeightsAndBias function."<<endl;
}
void NetLayer::updateWeightsAndBiasForBatch(const double studyRate,const double momentum ) {
    cout<<"Error: empty updateWeightsAndBiasForBatch function."<<endl;
}
