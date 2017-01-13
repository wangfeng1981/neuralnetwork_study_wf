#ifndef ARRAY3D_H
#define ARRAY3D_H
#include "wftools.h"

class Array3d
{
    public:
        Array3d(const int numx ,const int numy ,const int numd ,const bool fillGaussRand );
        virtual ~Array3d();
        void zero() ;
        double getValueAt(int ix,int iy , int idep ) const;
        void setValueAt(int ix,int iy ,int idep, double newValue) ;
        int getNumx() const;
        int getNumy() const;
        int getNumdep() const ;
        int get1DValueNum() const ;
        double get1DValueAt(int i) const ;
        void set1DValueAt(int i ,double newval) ;
    protected:

    private:
        double *mpData ;
        int mNumx;
        int mNumy;
        int mNumdep;
        int mNxy;
};

#endif // ARRAY3D_H
