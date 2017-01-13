#include <vector>
#include "wImage.h"
#include "cnn.h"
using namespace std;


#ifndef WSAMPLEONE_H
#define WSAMPLEONE_H

class wSampleOne
{
    public:
        wSampleOne(const char* imgfile,const char* weightsfile);
        virtual ~wSampleOne();
        void startClassify();

    protected:
        wImage* mpImagedata ;
        CNn* mpNetwork;

    private:
};

#endif // WSAMPLEONE_H
