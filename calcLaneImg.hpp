#ifndef __CALCLANEIMG__HEADER__
#define __CALCLANEIMG__HEADER__

#include "CV_header.hpp"

using namespace std;
using namespace cv;

vector<Point2f> calcLaneImg(Mat imgCombined);
vector<int> sumColElm(Mat img_binary);
int getLeftX_base(vector<int> sumArray);
int getRightX_base(vector<int> sumArray);

Mat halfDownImg(Mat original);

#endif