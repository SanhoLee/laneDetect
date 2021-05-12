#include "gloabalVal.hpp"
#include "preprocImg.hpp"

using namespace cv;

Mat imgUndistort, imgUnwarp;

// Todos...
// 사다리꼴, 사각형 좌표 초기화 필요함. 대략적으로 잡기.
vector<Point2f> srcRectCoord;
vector<Point2f> dstRectCoord;

Mat preprocImg(Mat img, Mat *invMatx)
{

    // undistort img.
    imgUndistort = undistortingImg(img);

    // perspective transform(3d -> 2d(bird's view))
    imgUnwarp = unWarpingImg(imgUndistort, &invMatx, srcRectCoord, dstRectCoord);

    // color space filtering

    // combined best color channel together.

    // return combined best output img.
};

Mat undistortingImg(Mat img){
    // calibrating camera with chessboard img.

};

Mat unWarpingImg(
    Mat imgUndistort,
    Mat **intMatx,
    vector<Point2f> srcRectCoord,
    vector<Point2f> dstRectCoord){

};