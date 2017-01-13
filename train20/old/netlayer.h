#include "array3d.h"

#ifndef NETLAYER_H
#define NETLAYER_H


class NetLayer
{
    public:
        NetLayer(const int outputXnum ,const int outputYnum ,const int outputDepnum );
        virtual ~NetLayer();
        virtual void computeForewardOutput(const NetLayer* prevLayer ) ;
        virtual void computeAndSumPartialDerivativeOfWeightsAndBias(const NetLayer* prevlayer,const NetLayer* nextlayer,const int outputTargetIndex) ;
        virtual void updateWeightsAndBiasForBatch(const double studyRate,const double momentum ) ;

        virtual bool hasWeightsAndBias() ;

        int getOutXnum() const ;
        int getOutYnum() const ;
        int getOutDepnum() const ;

        double getOutputAt(int ix,int iy,int idep) const;
        int get1DOutputNum() const ;
        double get1DOutputAt(int i ) const ;

        double getActivaAt(int ix,int iy,int idep) const ;
        double get1DActivaAt(int i )const ;

        double getDeltaAt(int ix,int iy,int idep) const ;
        double get1DDeltaAt(int i )const;

        virtual double getWeightsAt(int ix,int iy,int idep)const ;

        virtual void print() ;


    protected:
        Array3d* mpOutputData ;
        Array3d* mpOutputActi ;
        Array3d* mpDelta ;

        void setOutputAt(int ix,int iy,int idep, double newval) ;
        void setActivaAt(int ix,int iy,int idep, double newval) ;
        void setDeltaAt (int ix,int iy,int idep, double newval) ;
        void set1DDeltaAt (int i, double newval) ;
        void zeroOutput() ;
        void zeroActiva() ;
        void zeroDelta() ;


    private:

};

#endif // NETLAYER_H
