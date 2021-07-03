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
// void makeZero_UnderDiagonalElement(double xArr[3][3], double yArr[3][1]);
void makeZero_UnderDiagonalElement(double xArr[][3], double yArr[][1]);

vector<double> polyFit_cpp(vector<double> xCoord, vector<double> yCoord, int polyOrder);

double sumVecPow(vector<double> dataVec, int powOrder);
double sumVecPowXY(vector<double> dataVecX, int powOrderX, vector<double> dataVecY, int powOrderY);
void initGaussianMatrix(
    int polyOrder,
    vector<double> *xCoord,
    vector<double> *yCoord,
    double xArr[][3],
    double yArr[][1]);
void calcCoeffsValue(double xArr[][3], double yArr[][1], double aArr[][1]);

#endif