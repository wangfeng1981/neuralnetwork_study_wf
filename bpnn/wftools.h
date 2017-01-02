#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

#ifndef WFTOOLS_H_INCLUDED
#define WFTOOLS_H_INCLUDED

std::vector<std::string> wft_split(const std::string &text, char sep) {
    std::vector<std::string> tokens;
    std::size_t start = 0, end = 0;
    while ((end = text.find(sep, start)) != std::string::npos) {
        if (end != start) {
          tokens.push_back(text.substr(start, end - start));
        }
        start = end + 1;
    }
    if (end != start && start < text.length() ) {
       tokens.push_back(text.substr(start));
    }
    return tokens;
}


std::vector<double> wft_split2d( const std::string &text, char sep ){
    std::vector<std::string> vstr = wft_split(text , sep) ;
    std::vector<double> vd ;
    for(unsigned int i = 0 ; i<vstr.size() ; ++ i ){
        double val=0 ;
        std::stringstream stream( vstr[i] );
        stream >> val ;
        if (stream.fail()) {
            std::cout<<"bad string to double."<<endl;
            std::cout<<vstr[i]<<endl;
            exit(10) ;
        }
        vd.push_back(val) ;
    }
    return vd ;
}

std::vector<int> wft_split2i( const std::string &text, char sep ){
    std::vector<std::string> vstr = wft_split(text , sep) ;
    std::vector<int> vd ;
    for(unsigned int i = 0 ; i<vstr.size() ; ++ i ){
        int val=0 ;
        std::cout<<(int)(vstr[i].c_str()[0])<<std::endl;
        std::stringstream stream( vstr[i] );
        stream >> val ;
        if (stream.fail()) {
            std::cout<<"bad string to int."<<endl;
            std::cout<<vstr[i]<<endl;
            exit(10) ;
        }
        vd.push_back(val) ;
    }
    return vd ;
}

#endif // WFTOOLS_H_INCLUDED
