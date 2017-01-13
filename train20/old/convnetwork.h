#include "netlayer.h"
#include <vector>

using namespace std;

#ifndef CONVNETWORK_H
#define CONVNETWORK_H


class ConvNetwork
{
    public:
        ConvNetwork();
        virtual ~ConvNetwork();
        vector<NetLayer*> mLayerVector ;
        int doForward() ;
        void doTrain(const int targetIndex) ;
        void finishBatch() ;
    protected:

    private:
};

#endif // CONVNETWORK_H
