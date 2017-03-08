#ifndef WIMAGE_H
#define WIMAGE_H
#include <string>
#include <vector>
#include <tuple>

class wImage
{
    public:
        wImage(const char* imagepath );
        wImage(int nrow,int ncol) ;
        ~wImage();
        void getRGB(const int row,const int col , int& r,int& g,int& b) ;
        void setRGB(const int row,const int col , int r,int g , int b) ;
        void setRGB1d(const int index1d, int r,int g , int b) ;
        int getRows() ;
        int getCols() ;
        wImage gaussBlur( float sig ) ;
        void saveToFile(const std::string filepath ) ;
        void getRGB1d(const int index1d , int& r,int& g,int& b) ;

    protected:
        std::vector<unsigned char> data ;//row first
        int mRows ;
        int mCols ;
    private:

};

class wImaged
{
    public:
        wImaged(const char* imagepath );
        wImaged(int nrow,int ncol) ;
        ~wImaged();
        void getRGB(const int row,const int col , double& r,double& g,double& b) ;
        void getRGB(const int row,const int col , float& r,float& g,float& b) ;
        void setRGB(const int row,const int col , double r,double g , double b) ;
        void setRGB1d(const int index1d, double r,double g , double b) ;
        int getRows() ;
        int getCols() ;
        wImaged gaussBlur( double sig ) ;
        void saveToFile(const std::string filepath ) ;
        void getRGB1d(const int index1d , double& r,double& g,double& b) ;

    protected:
        std::vector<double> data ;//row first
        int mRows ;
        int mCols ;
    private:

};

#endif // WIMAGE_H
