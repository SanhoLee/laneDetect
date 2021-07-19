#ifndef __DRAWON_WARPIMG_HEADER__
#define __DRAWON_WARPIMG_HEADER__

#include "CV_header.hpp"

using namespace std;
using namespace cv;

Mat drawAll(Mat original, Mat binary, Mat invMatx, drawDataInfo drawDataSet);
void drawOnWarpImg(Mat imgBinary);
void drawRectangle(Mat dstImg, vector<vector<Rect>> rectWindowInfo);
void drawPolygonAndFill(Mat imgBinary);
void polyfit_using_prev_fitCoeffs(
    Mat imgBinary,
    vector<vector<double>> coeffsLR,
    vector<vector<Point>> *pixelPosXYNext,
    vector<vector<double>> *coeffsLRNext);

Mat make3ChImg(Mat imgBinary);
Mat drawLane(Mat original);

#endif