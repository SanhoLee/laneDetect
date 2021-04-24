#include <iostream>
#include <typeinfo>
#include "gloabalVal.hpp"
#include "CV_header.hpp"
#include "handleImg.hpp"

using namespace std;
using namespace cv;
int rtnKey = 0;

double degree_90_std = CV_PI / 2;
double degree_60 = CV_PI / 3;

/*
    1 unit scale transform...
    rho 1 unit -> 0.25 px
    theta 1 unit -> 0.5 degree
*/
int rho_unit = 3;
int theta_unit = 8;
int minLineLen_unit = 10;
int maxGap_unit = 10;
int topPos_unit = 3;
int leftPos_unit = 3;
int rightPos_unit = 3;

int rightSide_Angle = 19;
int leftSide_Angle = 30;

int yFixed_unit = 1;

string filename1 = "cac07407-196cd6f8.jpg";
string filename2 = "cb7bffcf-5e7c5677.jpg";

string IMG_FILE = "data/";
string SAVE_FILE_BASE = "data/output/";

int main()
{
    // img file name setting.
    IMG_FILE.append(filename2);

    // 이 값 설정으로 근처에 있는 서로 다른 직선을 구별한다.
    // Initialize rho and theta
    double rho_thres = 0.0;
    double theta_radian = 0;
    double theta_degree = 0;
    int ptr_votes_thres = 14;

    double minLineLen = 0;
    double maxGap = 0;
    double topPos = 0;
    double leftPos = 0;
    double rightPos = 0;

    // HSV space boundary set.
    int hmin = 30;
    int smin = 11;
    int vmin = 167;
    int hmax = 41;
    int smax = 26;
    int vmax = 192;

    int lowThres = 170;
    int highThres = 190;

    int leftPnt_Pos_unit = 5;
    int rightPnt_Pos_unit = 5;

    namedWindow("Trackbar", WINDOW_NORMAL);
    // resizeWindow("Trackbar", Size(700, 100));

    // createTrackbar("Rho ", "Trackbar", &rho_unit, 10);
    // createTrackbar("Theta ", "Trackbar", &theta_unit, 10);
    // createTrackbar("votes_thres ", "Trackbar", &ptr_votes_thres, 300);
    // createTrackbar("topPos ", "Trackbar", &topPos_unit, 10);
    // createTrackbar("leftPos ", "Trackbar", &leftPos_unit, 10);
    // createTrackbar("rightPos ", "Trackbar", &rightPos_unit, 10);
    // createTrackbar("minLineLen_unit ", "Trackbar", &minLineLen_unit, 50);
    // createTrackbar("maxGap_unit ", "Trackbar", &maxGap_unit, 50);

    // createTrackbar("hmin ", "Trackbar", &hmin, 179);
    // createTrackbar("smin ", "Trackbar", &smin, 255);
    // createTrackbar("vmin ", "Trackbar", &vmin, 255);
    // createTrackbar("hmax ", "Trackbar", &hmax, 179);
    // createTrackbar("smax ", "Trackbar", &smax, 255);
    // createTrackbar("vmax ", "Trackbar", &vmax, 255);

    createTrackbar("lowThres ", "Trackbar", &lowThres, 200);
    createTrackbar("highThres ", "Trackbar", &highThres, 200);

    // 정수형 4개 데이터를 갖는 벡터 변수 선언.
    vector<Vec4i> linePoints;
    Mat img, imgMask, imgBlur, imgCanny, imgROI;

    int kernelSize = 5;

    while (true)
    {
        // 이미지 초기화 !
        img = imread(IMG_FILE);
        isEmptyImg(img);
        resize(img, img, Size(), 0.5, 0.5);

        /* 1. preprocessing img. */
        Scalar lower(hmin, smin, vmin);
        Scalar upper(hmax, smax, vmax);
        imgMask = preProcessing(img, lower, upper);

        GaussianBlur(imgMask, imgBlur, Size(kernelSize, kernelSize), kernelSize, kernelSize);
        Canny(imgBlur, imgCanny, lowThres, highThres);
        imgROI = ImgROI(imgCanny, yFixed_unit, leftPnt_Pos_unit, rightPnt_Pos_unit);

        /* 2. Hough line Transform(normal version.)*/

        // rho_thres = (double)rho_unit * 0.25;
        // theta_degree = (double)theta_unit * 0.5;
        // theta_radian = theta_degree * toRadian();
        // minLineLen = minLineLen_unit * 0.5;
        // maxGap = maxGap_unit * 0.5;
        // topPos = topPos_unit * 0.1;
        // leftPos = leftPos_unit * 0.1;
        // rightPos = rightPos_unit * 0.1;

        // HoughLinesP(
        //     imgEdge,
        //     linePoints,
        //     rho_thres,
        //     theta_radian,
        //     ptr_votes_thres,
        //     minLineLen,
        //     maxGap);

        // filtering points on Specific Area.
        // linePoints = filterBtmCtrPt(img, linePoints, topPos, leftPos, rightPos);

        // drawHoughLines(img, lines, leftSide_Angle, rightSide_Angle, yFixed);
        // drawLinesWithPoint(img, linePoints);
        // showCurValue(lowThres, highThres);

        imshow("img", img);
        // imshow("imgMask", imgMask);
        // imshow("imgCanny", imgCanny);
        imshow("imgROI", imgROI);

        rtnKey = waitKey(1);
        if (rtnKey == 27)
        {
            imwrite(SAVE_FILE_BASE + "HoughCurrent.png", imgROI);
            return 0;
        }
    }
}
