#ifndef __CALCLANEIMG__HEADER__
#define __CALCLANEIMG__HEADER__

#include "CV_header.hpp"

using namespace std;
using namespace cv;

Mat halfDownImg(Mat original);

vector<Point2f> calcLaneImg(Mat imgCombined);
vector<int> sumColElm(Mat img_binary);
vector<int> getIndexArray_onWindow(Mat nonZeroPos, Rect windowBox);

int getLeftX_base(vector<int> sumArray);
int getRightX_base(vector<int> sumArray);
int mean_vectorArray(vector<int> target_vector);

void reCenterCurrentPos(vector<int> pntXIndexArray, int *currentXPos);
void winSearchImg(Mat preprocess,
                  int numWindow,
                  vector<int> xBase,
                  vector<vector<Rect>> rectWindowInfo,
                  vector<vector<int>> leftLanePixelContainer,
                  vector<vector<int>> rightLanePixelContainer);

#endif