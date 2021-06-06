// #include <iostream>
// #include <fstream>
// #include "CV_header.hpp"
// #include "getDistortCoeff.hpp"

// using namespace cv;
// using namespace std;

// // initialize chessboard variables. one squre length : 20mm <-- no need to concern.
// // int numHorzCorner = 9;
// int numHorzCorner = 8;
// // int numVertCorner = 6;
// int numVertCorner = 6;
// int oneSqureLen = 20;
// int numBoards = 0;

// int main(int argc, char **argv)
// {
//     int rtnWaitKey = 0;
//     int numSqures = (numHorzCorner * numVertCorner);
//     Size board_sz = Size(numHorzCorner, numVertCorner);

//     /*  Define vector variables.

//     object_Points   : 3-Dimensional object coordinate Points
//     image_Points    : 2-Dimensional image coordinate Points.(On display)
//     corners         : 2-Dimensional Points for keeping corner on the chessboard.
//                       Temp Array for saveing on image_Points
//     */
//     vector<vector<Point3f>> object_Points;
//     vector<vector<Point2f>> image_Points;
//     vector<Point2f> corners;
//     int success = 0;

//     // VideoCapture cap(0);
//     Mat img, imgGray;

//     // get number of boards
//     // cout << "Enter number of boards. : ";
//     // cin >> numBoards;
//     numBoards = 1;

//     // cap >> img;
//     img = imread("data/cb_src.png");

//     vector<Point3f> obj;
//     for (int j = 0; j < numSqures; j++)
//     {
//         obj.push_back(Point3f(j / numHorzCorner, j % numHorzCorner, 0.0f));
//     }

//     while (success < numBoards)
//     {
//         cvtColor(img, imgGray, COLOR_BGR2GRAY);
//         bool found = findChessboardCorners(
//             img,
//             board_sz,
//             corners,
//             CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FILTER_QUADS);
//         // find the corners base on board_sz(horizontal, vertical direction) size.
//         // detected corner pixel is stored in corners array.
//         // 마지막 파라미터 : to improve the chances of detecting the corners.
//         if (found)
//         {
//             cornerSubPix(
//                 imgGray,
//                 corners,
//                 Size(11, 11),
//                 Size(-1, -1),
//                 TermCriteria(TermCriteria::MAX_ITER | TermCriteria::EPS, 30, 0.1));
//             drawChessboardCorners(imgGray, board_sz, corners, found);
//         }
//         // imshow("img", img);
//         imshow("img Gray", imgGray);

//         // update new frame again.
//         // cap >> img;
//         img = imread("data/cb_src.png");
//         rtnWaitKey = waitKey(1);
//         if (rtnWaitKey == 27)
//         {
//             return 0;
//         }
//         if (rtnWaitKey == ' ' && found != 0)
//         {
//             // when detecting corners correctly.
//             // obj 는 기본 베이스로 일정한 간격의 좌표값(x,x,x)을 board 갯수만큼 채워준다.
//             image_Points.push_back(corners);
//             object_Points.push_back(obj);

//             cout << "snap stored ! " << endl;
//             success++;
//             if (success >= numBoards)
//             {
//                 break;
//             }
//         }
//     }

//     /*
//      now, go to calibration.
//      declare variables that will hold the unknowns

//      intrinsic  : intrinsic matrix
//      distCoeffs : distortion coefficient matrix
//      rvecs      : rotation vectors
//      tvecs      : translation vectors
//     */
//     Mat intrinsic = Mat(3, 3, CV_32FC1);
//     Mat distCoeffs;
//     vector<Mat> rvecs;
//     vector<Mat> tvecs;

//     /*
//     Mat.ptr 메소드 : 매트릭스의 특정 주소 위치를 포인터를 사용하여 명시할 수 있다.
//     포인터 메소드 설명 : https://docs.opencv.org/3.4/d3/d63/classcv_1_1Mat.html#a13acd320291229615ef15f96ff1ff738
//     */

//     // set camera's aspect ratio --> 1
//     // focal length(x element : fx)
//     intrinsic.ptr<float>(0)[0] = 1;
//     // // focal length(y element : fy)
//     intrinsic.ptr<float>(1)[1] = 1;

//     /*
//     Get Camera calibrating parameters.

//      input  :
//         object_Points(3D),
//         image_Points(2D),
//         img.size()
//      output :
//         a property of camera and lens
//             - intrinsic(camera coefficient),
//             - distCoeffs(distorting coefficient),
//         rvecs(rotation vector),
//         tvecs(traslation vector)

//     Note: The calibrateCamera function converts all matrices into 64F format even if you initialize it to 32F.

//     CV_8U : 8 bit unsigned integer (pixel range : 0~255)
//     CV_32F : 32 bit(4-byte) float , can have any value between 0~1.0

//     as long as you use same camera(lens), you don't need to calculate this process again.
//     save the coeffiecient value and reuse it.
//     */

//     calibrateCamera(object_Points, image_Points, img.size(), intrinsic, distCoeffs, rvecs, tvecs);

//     // write variables on the out.txt file.
//     ofstream out("cameraCalibrationData/out.txt");
//     if (out.fail())
//     {
//         cout << "FAILED :: OUT STREAM ! :: LINE : " << __LINE__ << ", FILE : " << __FILE__ << endl;
//         return -1;
//     }

//     int m = intrinsic.rows;
//     int n = intrinsic.cols;
//     out << m << ' ' << n << endl;
//     for (int i = 0; i < m; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             out << intrinsic.ptr<double>(i)[j];
//             out << ' ';
//         }
//         out << endl;
//     }

//     m = distCoeffs.rows;
//     n = distCoeffs.cols;
//     out << m << ' ' << n << endl;
//     for (int i = 0; i < m; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             out << distCoeffs.ptr<double>(i)[j];
//             out << ' ';
//         }
//         out << endl;
//     }
//     out.close();

//     return 0;
// }