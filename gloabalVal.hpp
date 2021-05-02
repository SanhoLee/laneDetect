#ifndef __GLOBAL_VAR_HEADER__
#define __GLOBAL_VAR_HEADER__
#include "CV_header.hpp"

using namespace std;
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

/* -------------------initialize each Variables.------------------- */

//houghLines variables.
int rho_unit = 3;
int theta_unit = 8;
int minLineLen_unit = 10;
int maxGap_unit = 10;
int topPos_unit = 3;
int leftPos_unit = 3;
int rightPos_unit = 3;

// filtering variables.
int rightSide_Angle = 19;
int leftSide_Angle = 30;
int yFixed_unit = 1;

// filename variables.
string filename1 = "cac07407-196cd6f8.jpg";
string filename2 = "cb7bffcf-5e7c5677.jpg";

// convert angle value variables.
double degree_90_std = CV_PI / 2;
double degree_60 = CV_PI / 3;

// BLUR KERNEL SIZE.
int kernelSize = 11;

// HLS AND HSV space boundary set.
int hmin = 28;
int smin = 166;
int lmin = 16;
int hmax = 59;
int smax = 226;
int lmax = 28;

int vmin = 0;
int vmax = 255;

#endif