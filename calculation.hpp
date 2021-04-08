#ifndef __CALC_HEADER__
#define __CALC_HEADER__

#include <vector>
#include "gloabalVal.hpp"
#include "CV_header.hpp"

using namespace std;
using namespace cv;

vector<lineElement> calcLineElement(vector<Vec2f> lines);
vector<pointsElement> getLinePoints(vector<lineElement> lineElems, Mat img, double topPosY);

#endif