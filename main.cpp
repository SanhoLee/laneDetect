#include <iostream>
#include <typeinfo>
#include "gloabalVal.hpp"
#include "CV_header.hpp"
#include "handleImg.hpp"
#include "calculation.hpp"

using namespace std;
using namespace cv;
int rtnKey = 0;

double degree_90_std = CV_PI / 2;
double degree_60 = CV_PI / 3;

int rightSide_Angle = 19;
int leftSide_Angle = 30;
int yFixed = 100;

String IMG_FILE = "data/cac07407-196cd6f8.jpg";
String SAVE_FILE_BASE = "data/output/";

int main()
{
    // 이 값 설정으로 근처에 있는 서로 다른 직선을 구별한다.
    // Initialize rho and theta
    double rho_thres = 0.0;
    double theta_radian = 0;
    double theta_degree = 0;
    int ptr_votes_thres = 70;

    /*
    1 unit scale transform...
    rho 1 unit -> 0.25 px
    theta 1 unit -> 0.5 degree
    */
    int rho_unit = 3;
    int theta_unit = 8;

    namedWindow("Trackbar", WINDOW_NORMAL);
    resizeWindow("Trackbar", Size(700, 100));

    createTrackbar("Rho ", "Trackbar", &rho_unit, 10);
    createTrackbar("Theta ", "Trackbar", &theta_unit, 10);
    createTrackbar("votes_thres ", "Trackbar", &ptr_votes_thres, 300);
    createTrackbar("right degree ", "Trackbar", &rightSide_Angle, 90);
    createTrackbar("left degree ", "Trackbar", &leftSide_Angle, 90);
    createTrackbar("yFixed ", "Trackbar", &yFixed, 500);

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

        /* 2. Hough line Transform(normal version.)*/

        rho_thres = (double)rho_unit * 0.25;
        theta_degree = (double)theta_unit * 0.5;
        theta_radian = theta_degree * toRadian();

        HoughLines(imgEdge, lines, rho_thres, theta_radian, ptr_votes_thres, 0, 0);

        // cout << "votes : " << ptr_votes_thres
        //      << "\t left Angle :" << leftSide_Angle
        //      << "\t right Angle :" << rightSide_Angle
        //      << "\t Rho : " << rho_thres
        //      << "\t theta(degree) : " << theta_degree
        //      << "\t yFixed : " << yFixed
        //      << endl;

        drawHoughLines(img, lines, leftSide_Angle, rightSide_Angle, yFixed);

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
