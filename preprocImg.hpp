#ifndef __PREPROCESSING__HEADER__
#define __PREPROCESSING__HEADER__

#include "CV_header.hpp"

using namespace std;
using namespace cv;

Mat preprocImg(Mat img, Mat *invMatx);
Mat undistortingImg(Mat img, bool activateThis);
Mat unWarpingImg(Mat imgUndistort, Mat **invMatx, bool showWarpZone);
Mat Filtering_imgPixel(Mat imgUnwarp, int mode);

int getMatrixVal(char calibration_matrix_file[], Mat *intrinsicRead, Mat *distCoeffsRead);

#endif