#ifndef __CALCLANEIMG__HEADER__
#define __CALCLANEIMG__HEADER__

#include "CV_header.hpp"

using namespace std;
using namespace cv;

Mat halfDownImg(Mat original);

vector<Point2f> calcLaneImg(Mat imgCombined);
vector<int> sumColElm(Mat img_binary);
vector<Point> getIndexArray_onWindow(Mat nonZeroPos, Rect windowBox);
vector<Point> dimDownFrom2To1(vector<vector<Point>> twoDimVector);

int getLeftX_base(vector<int> sumArray);
int getRightX_base(vector<int> sumArray);
int mean_vectorArray(vector<Point> target_vector);

void reCenterCurrentPos(vector<Point> pntIndexArray, int *currentXPos);
void winSearchImg(Mat preprocess,
                  int numWindow,
                  vector<int> xBase,
                  vector<vector<Rect>> *rectWindowInfo,
                  vector<vector<Point>> *leftLanePixelContainer,
                  vector<vector<Point>> *rightLanePixelContainer);
vector<double> polyFit_cpp(vector<double> xCoord, vector<double> yCoord, int polyOrder);
double sumVec(vector<double> dataVec);
double sumVecPow(vector<double> dataVec, int powOrder);
double sumVecPowXY(vector<double> dataVecX, int powOrderX, vector<double> dataVecY, int powOrderY);

#endif