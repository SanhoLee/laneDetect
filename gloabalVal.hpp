#ifndef __GLOBAL_VAR_HEADER__
#define __GLOBAL_VAR_HEADER__

#include "CV_header.hpp"

using namespace cv;

typedef struct lineEquationElment
{
    double slope;
    double yItcp;
} lineElement;

typedef struct pointsFormLineElmes
{
    Point topPt;
    Point bottomPt;
} pointsElement;

#endif