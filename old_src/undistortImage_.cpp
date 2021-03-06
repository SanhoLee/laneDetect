#include <iostream>
#include <fstream>
#include "CV_header.hpp"

using namespace cv;
using namespace std;

// int main(int argc, char **argv)
// {
//     Mat img;
//     VideoCapture cap(0);
//     int m = 0, n = 0;
//     int rtnWaitKey = 0;

//     // read variables from the out file.
//     ifstream in("cameraCalibrationData/out.txt");
//     if (in.fail())
//     {
//         cout << "FAILED :: IN STREAM ! :: LINE : " << __LINE__ << ", FILE : " << __FILE__ << endl;
//         return -1;
//     }

//     // get matrix values.
//     // 1. camera matrix size.
//     in >> m >> n;

//     // define matrix variable for reading intrinsic variables
//     // Mat intrinsicRead = Mat(m, n, intrinsic.type());
//     Mat intrinsicRead = Mat(m, n, 6);

//     // 2. camera marix values.
//     for (int i = 0; i < m; i++)
//         for (int j = 0; j < n; j++)
//             in >> intrinsicRead.ptr<double>(i)[j];

//     // 3. distortion coefficients matrix size.
//     in >> m >> n;

//     // define matrix variable for reading distCoeffs variables
//     // Mat distCoeffsRead = Mat(m, n, distCoeffs.type());
//     Mat distCoeffsRead = Mat(m, n, 6);

//     // 4. distortion coefficients matrix value.
//     for (int i = 0; i < m; i++)
//         for (int j = 0; j < n; j++)
//             in >> distCoeffsRead.ptr<double>(i)[j];

//     in.close();

//     /* undistorting image. */
//     Mat imgUndistorted;
//     while (true)
//     {
//         cap >> img;
//         // img = imread("data/cb_src.png");
//         undistort(img, imgUndistorted, intrinsicRead, distCoeffsRead);

//         resize(img, img, Size(), 0.5, 0.5);
//         resize(imgUndistorted, imgUndistorted, Size(), 0.5, 0.5);

//         imshow("image", img);
//         imshow("image undistorted", imgUndistorted);
//         rtnWaitKey = waitKey(1);
//         if (rtnWaitKey == 27)
//         {
//             // imwrite("imgOrigin_resized.png", img);
//             // imwrite("imgUndistorted_resized.png", imgUndistorted);
//             cap.release();
//             return 0;
//         }
//     }
// }
