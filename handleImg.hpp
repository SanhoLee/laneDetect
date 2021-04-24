#ifndef __HANDLEIMG__HEADER__
#define __HANDLEIMG__HEADER__
#include "CV_header.hpp"

using namespace cv;
using namespace std;

vector<Vec4i> filterBtmCtrPt(
    Mat img,
    vector<Vec4i> linePoints,
    double topPos,
    double leftPos,
    double rightPos);
void drawHoughLines(Mat img, vector<Vec2f> lines, int leftSide_Angle, int rightSide_Angle, int yFixed);
void drawLinesWithPoint(Mat img, vector<Vec4i> linePoints);
int isEmptyImg(Mat img);
Mat preProcessing(Mat img, Scalar lower, Scalar upper);
void showCurValue(
    int lowThres,
    int highThres);
Mat ImgROI(Mat imgCanny, int yFixed_unit, int leftPnt_Pos_unit, int rightPnt_Pos_unit);

#endif