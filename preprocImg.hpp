#ifndef __PREPROCESSING__HEADER__
#define __PREPROCESSING__HEADER__

Mat preprocImg(Mat img, Mat *invMatx);
Mat undistortingImg(Mat img);
Mat unWarpingImg(
    Mat imgUndistort,
    Mat **intMatx,
    vector<Point2f> srcRectCoord,
    vector<Point2f> dstRectCoord);

#endif