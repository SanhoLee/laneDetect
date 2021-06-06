#include <iostream>
#include <typeinfo>
// #include "gloabalVal.hpp"
// #include "CV_header.hpp"
// #include "handleImg.hpp"

// using namespace std;
// using namespace cv;
// int rtnKey = 0;

// string IMG_FILE = "data/";
// string SAVE_FILE_BASE = "data/output/";

// int main()
// {
//     // img file name setting.
//     IMG_FILE.append(filename2);

//     // Initialize houghLines variable(actual input value.)
//     double rho_thres = 0.0;
//     double theta_radian = 0;
//     double theta_degree = 0;
//     int ptr_votes_thres = 14;

//     double minLineLen = 0;
//     double maxGap = 0;
//     double topPos = 0;
//     double leftPos = 0;
//     double rightPos = 0;

//     int lowThres = 50;
//     int highThres = 150;

//     int leftPnt_Pos_unit = 5;
//     int rightPnt_Pos_unit = 5;

//     char trackbarName[] = "Trackbar";

//     namedWindow(trackbarName, WINDOW_NORMAL);
//     // resizeWindow("Trackbar", Size(700, 100));

//     // createTrackbar("Rho ", "Trackbar", &rho_unit, 10);
//     // createTrackbar("Theta ", "Trackbar", &theta_unit, 10);
//     // createTrackbar("votes_thres ", "Trackbar", &ptr_votes_thres, 300);
//     // createTrackbar("topPos ", "Trackbar", &topPos_unit, 10);
//     // createTrackbar("leftPos ", "Trackbar", &leftPos_unit, 10);
//     // createTrackbar("rightPos ", "Trackbar", &rightPos_unit, 10);
//     // createTrackbar("minLineLen_unit ", "Trackbar", &minLineLen_unit, 50);
//     // createTrackbar("maxGap_unit ", "Trackbar", &maxGap_unit, 50);

//     createTrackbar("hmin ", trackbarName, &hmin, 179);
//     createTrackbar("smin ", trackbarName, &smin, 255);
//     createTrackbar("lmin ", trackbarName, &lmin, 255);
//     createTrackbar("hmax ", trackbarName, &hmax, 179);
//     createTrackbar("smax ", trackbarName, &smax, 255);
//     createTrackbar("lmax ", trackbarName, &lmax, 255);

//     createTrackbar("lowThres ", "Trackbar", &lowThres, 200);
//     createTrackbar("highThres ", "Trackbar", &highThres, 200);

//     // 정수형 4개 데이터를 갖는 벡터 변수 선언.
//     vector<Vec4i> linePoints;
//     Mat img, imgMask, imgBlur, imgCanny, imgROI;

//     while (true)
//     {
//         // 이미지 초기화 !
//         img = imread(IMG_FILE);
//         isEmptyImg(img);
//         resize(img, img, Size(), 0.5, 0.5);

//         /* 1. preprocessing img. */
//         Scalar lower(hmin, smin, lmin);
//         Scalar upper(hmax, smax, lmax);
//         imgMask = maskingImg(img, lower, upper);

//         GaussianBlur(imgMask, imgBlur, Size(kernelSize, kernelSize), kernelSize, kernelSize);
//         Canny(imgBlur, imgCanny, lowThres, highThres);
//         // imgROI = ImgROI(imgCanny, yFixed_unit, leftPnt_Pos_unit, rightPnt_Pos_unit);

//         /* 2. Hough line Transform(normal version.)*/

//         rho_thres = (double)rho_unit * 0.25;
//         theta_degree = (double)theta_unit * 0.5;
//         theta_radian = theta_degree * toRadian();
//         minLineLen = minLineLen_unit * 0.5;
//         maxGap = maxGap_unit * 0.5;
//         topPos = topPos_unit * 0.1;
//         leftPos = leftPos_unit * 0.1;
//         rightPos = rightPos_unit * 0.1;

//         // HoughLinesP(
//         //     imgROI,
//         //     linePoints,
//         //     rho_thres,
//         //     theta_radian,
//         //     ptr_votes_thres,
//         //     minLineLen,
//         //     maxGap);

//         // drawLinesWithPoint(img, linePoints);
//         // showCurValue(
//         //     hmin,
//         //     smin,
//         //     lmin,
//         //     hmax,
//         //     smax,
//         //     lmax,
//         //     lowThres,
//         //     highThres);

//         imshow("img", img);
//         imshow("imgMask", imgMask);
//         imshow("imgCanny", imgCanny);
//         // imshow("imgROI", imgROI);

//         rtnKey = waitKey(1);
//         if (rtnKey == 27)
//         {
//             imwrite(SAVE_FILE_BASE + "HoughCurrent.png", imgMask);
//             return 0;
//         }
//     }
// }
