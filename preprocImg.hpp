#ifndef __PREPROCESSING__HEADER__
#define __PREPROCESSING__HEADER__

#include "CV_header.hpp"

using namespace std;
using namespace cv;

Mat preprocImg(Mat img, Mat *invMatx);
Mat undistortingImg(Mat img, bool activateThis);
Mat unWarpingImg(Mat imgUndistort, Mat **invMatx, bool showWarpZone);
Mat filterImg(Mat imgUnwarp, int toColorChannel, int mode);
Mat absSobel_Thres(Mat imgSRC, int dX, int dY, int edge_threshold[]);
Mat grayTo_Mag(Mat gray, int magKernelSize, int mag_threshold[]);
Mat grayTo_Dir(Mat gray, int dirKernelSize, double dir_threshold[]);
Mat combine_threshold(Mat gray);
Mat get_oneFromZeros_withinBound(Mat img, int boundary_thres[]);
Mat normalize_HLS_L(Mat unWarp);
Mat normalize_LAB_B(Mat unWarp);
Mat make_zeros(Mat img);
Mat make_ones(Mat img);
Mat combine_both_img(Mat hls, Mat lab);

int getMatrixVal(char calibration_matrix_file[], Mat *intrinsicRead, Mat *distCoeffsRead);

#endif