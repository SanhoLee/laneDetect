#ifndef __STRUCT_DRAWDATA__
#define __STRUCT_DRAWDATA__

#include "CV_header.hpp"

using namespace cv;
using namespace std;

typedef struct drawData
{
    vector<Point> leftLanePts;
    vector<Point> rightLanePts;
    double leftRadius;
    double rightRadius;
    double centerOffset;

} drawDataInfo;

#endif