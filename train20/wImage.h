#ifndef WIMAGE_H
#define WIMAGE_H
#include <string>
#include <vector>

class wImage
{
    public:
        wImage(const char* imagepath );
        ~wImage();
        void getRGB(const int row,const int col , int& r,int& g,int& b) ;
        unsigned int getRows() ;
        unsigned int getCols() ;
    protected:
        std::vector<unsigned char> data ;//row first
        unsigned int mRows ;
        unsigned int mCols ;
    private:

};

#endif // WIMAGE_H
