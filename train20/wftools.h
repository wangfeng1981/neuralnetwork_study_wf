#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "array3d.h"

using namespace std;

#ifndef WFTOOLS_H_INCLUDED
#define WFTOOLS_H_INCLUDED

std::vector<std::string> wft_split(const std::string &text, char sep) ;


std::vector<double> wft_split2d( const std::string &text, char sep ) ;

std::vector<int> wft_split2i( const std::string &text, char sep ) ;

double wft_gaussrand( double muval , double stdval ) ;

double wft_sigmoid(double z)  ;

int wft_randFromZeroToN(int n);

void wft_printArray3d(const Array3d* a3d,string title) ;

#endif // WFTOOLS_H_INCLUDED
