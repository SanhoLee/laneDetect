#ifndef __HANDLEIMG__HEADER__
#define __HANDLEIMG__HEADER__
#include "CV_header.hpp"

using namespace cv;
using namespace std;

void drawHoughLines(Mat img, vector<Vec2f> lines, int leftSide_Angle, int rightSide_Angle, int yFixed);
void drawLinesWithPoint(Mat img, vector<pointsElement> pntElms);
int isEmptyImg(Mat img);
Mat preProcessing(Mat img);

#endif