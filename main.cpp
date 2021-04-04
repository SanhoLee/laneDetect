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
int rightSide_Angle = 1;
int leftSide_Angle = 1;
String IMG_FILE = "data/cac07407-196cd6f8.jpg";
String SAVE_FILE_BASE = "data/output/";

int main()
{
    // 이 값 설정으로 근처에 있는 서로 다른 직선을 구별한다.
    // Initialize rho and theta
    double rho_thres = 0;
    double theta_radian = 0;
    double theta_degree = 0;
    int ptr_votes_thres = 100;

    // unit change format
    double to_radian = (CV_PI / 180);
    double to_degree = (180 / CV_PI);

    /*
    1 unit scale transform...
    rho 1 unit -> 0.25 px
    theta 1 unit -> 0.5 degree
    */
    int rho_unit = 1;
    int theta_unit = 1;

    namedWindow("Trackbar", WINDOW_NORMAL);
    resizeWindow("Trackbar", Size(700, 100));

    createTrackbar("HLine :: Rho ", "Trackbar", &rho_unit, 10);
    createTrackbar("HLine :: Theta ", "Trackbar", &theta_unit, 10);
    createTrackbar("HLine :: ptr_votes_thres ", "Trackbar", &ptr_votes_thres, 300);
    createTrackbar("detect :: right lane degree ", "Trackbar", &rightSide_Angle, 90);
    createTrackbar("detect :: left lane degree ", "Trackbar", &leftSide_Angle, 90);

    // float 값 두개를 가지는 lines 벡터 변수 선언.
    vector<Vec2f> lines;
    Mat img, imgEdge;

    while (true)
    {
        // 이미지 초기화 !
        img = imread(IMG_FILE);
        isEmptyImg(img);

        /* 1. preprocessing img. */
        imgEdge = preProcessing(img);

        /* 2. Hough line Transform(normal version.)
          */

        rho_thres = (double)rho_unit * 0.25;
        theta_degree = (double)theta_unit * 0.5;
        theta_radian = theta_degree * to_radian;

        HoughLines(imgEdge, lines, rho_thres, theta_radian, ptr_votes_thres, 0, 0);
        drawHoughLines(img, lines, leftSide_Angle, rightSide_Angle);

        resize(img, img, Size(), 0.5, 0.5);
        imshow("img", img);
        rtnKey = waitKey(1);
        if (rtnKey == 27)
        {
            imwrite(SAVE_FILE_BASE + "HoughCurrent.png", img);
            return 0;
        }
    }
}
