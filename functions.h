
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <sstream>
#include "ppm.h"

template <typename T> std::string itos (T Number) {
    std::stringstream ss;
    ss << Number;
    return (ss.str());
}
unsigned char *buildAlphaData (Ppmimage *img);
int diff_ms (timeval t1, timeval t2);
#endif
