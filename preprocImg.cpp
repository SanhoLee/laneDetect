#include <iostream>
#include <fstream>
#include "preprocImg.hpp"
#define HLS_CHANNEL 0
#define LAB_CHANNEL 1

#define H_FILTER 0
#define L_FILTER 1
#define S_FILTER 2

#define L_FILTER_ 0
#define A_FILTER_ 1
#define B_FILTER_ 2

char CALBR_FILE[256] = "cameraCalibrationData/out.txt";

Mat preprocImg(Mat img, Mat *invMatx)
{
    Mat imgUndistort, imgUnwarp;
    Mat imgHLS_H, imgHLS_L, imgHLS_S;
    Mat imgLAB_L, imgLAB_A, imgLAB_B;

    /* undistort img. */
    imgUndistort = undistortingImg(img, true);

    /* perspective transform(3d -> 2d(bird's view)) */
    imgUnwarp = unWarpingImg(imgUndistort, &invMatx, false);

    /* color space filtering 
     HLS, L channel is the best for detecting white lane. 
     LAB, B channel is the best for detecting yellow lane. */
    imgHLS_L = filterImg(imgUnwarp, HLS_CHANNEL, L_FILTER);
    imgLAB_B = filterImg(imgUnwarp, LAB_CHANNEL, B_FILTER_);

    /* COLORMAP for grayscale. */
    applyColorMap(imgHLS_L, imgHLS_L, COLORMAP_BONE);
    applyColorMap(imgLAB_B, imgLAB_B, COLORMAP_BONE);

    imshow("HLS - L filter", imgHLS_L);
    imshow("LAB - B filter", imgLAB_B);

    imshow("imgUnwarp", imgUnwarp);
    waitKey(0);

    // combined best color channel together.

    // return combined best output img.

    return img;
};

Mat filterImg(Mat imgUnwarp, int toColorChannel, int mode)
{
    /*
    channel mode definition.
        0 : Hue
        1 : Lightness
        2 : Saturation
        
        hue max : 179, l and s max : 255
    */
    Mat imgConverted;
    // Mat imgOUT = Mat(720, 1280, CV_8UC3);
    Mat imgOUT = Mat::zeros(720, 1280, CV_8UC3);

    /* 1. convert color channel from BGR to HLS or LAB. */
    if (toColorChannel == HLS_CHANNEL)
    {
        cvtColor(imgUnwarp, imgConverted, COLOR_BGR2HLS);
    }
    else if (toColorChannel == LAB_CHANNEL)
    {
        cvtColor(imgUnwarp, imgConverted, COLOR_BGR2Lab);
    }

    /* 2. pixel pointer variable setting.
    help -->  https://stackoverflow.com/questions/7899108/opencv-get-pixel-channel-value-from-mat-image
    */
    uint8_t *pixelPtr = (uint8_t *)imgConverted.data;
    int cn = imgConverted.channels();

    switch (mode)
    {
    case 0:
        // set H space Only  // set L space Only
        for (int i = 0; i < imgConverted.rows; i++)
        {
            for (int j = 0; j < imgConverted.cols; j++)
            {
                imgOUT.at<Vec3b>(i, j)[0] = pixelPtr[i * imgConverted.cols * cn + j * cn + 0];
                // imgOUT.at<Vec3b>(i, j)[1] = 0;
                // imgOUT.at<Vec3b>(i, j)[2] = 0;
            }
        }
        break;
    case 1:
        // set L space Only  // set A space Only
        for (int i = 0; i < imgConverted.rows; i++)
        {
            for (int j = 0; j < imgConverted.cols; j++)
            {
                // imgOUT.at<Vec3b>(i, j)[0] = 0;
                imgOUT.at<Vec3b>(i, j)[1] = pixelPtr[i * imgConverted.cols * cn + j * cn + 1];
                // imgOUT.at<Vec3b>(i, j)[2] = 0;
            }
        }
        break;

    case 2:
        // set S space Only  // set B space Only
        for (int i = 0; i < imgConverted.rows; i++)
        {
            for (int j = 0; j < imgConverted.cols; j++)
            {
                // imgOUT.at<Vec3b>(i, j)[0] = 0;
                // imgOUT.at<Vec3b>(i, j)[1] = 0;
                imgOUT.at<Vec3b>(i, j)[2] = pixelPtr[i * imgConverted.cols * cn + j * cn + 2];
            }
        }
        break;

    default:
        break;
    }

    return imgOUT;
}

Mat undistortingImg(Mat img, bool activateThis)
{
    int iret = 0;
    Mat imgUndistorted;
    Mat intrinsicRead, distCoeffsRead;

    // calibrating camera with chessboard img.
    // but this time, just get parameters from a file.

    /* get matrix values. */
    if (activateThis)
        iret = getMatrixVal(CALBR_FILE, &intrinsicRead, &distCoeffsRead);
    else
        return img;

    /* check iret value */
    if (iret != 0)
    {
        return img;
    }

    // undistort function.
    undistort(img, imgUndistorted, intrinsicRead, distCoeffsRead);
    return imgUndistorted;
};

Mat unWarpingImg(Mat imgUndistort, Mat **invMatx, bool showWarpZone)
{
    Mat Matx;
    Mat imgWarp;
    int width, height;
    vector<Point2f> srcRectCoord;
    vector<Point2f> dstRectCoord;
    int dstX = 280;

    width = imgUndistort.cols;
    height = imgUndistort.rows;
    // width : 1280, height : 720

    Point p1s = Point2f(width / 2 - 150, height - 220);
    Point p2s = Point2f(width / 2 + 150, height - 220);
    Point p3s = Point2f(220, height - 50.0);
    Point p4s = Point2f(width - 220.0, height - 50.0);

    Point p1d = Point2f(dstX, 0);
    Point p2d = Point2f(width - dstX, 0);
    Point p3d = Point2f(dstX, height);
    Point p4d = Point2f(width - dstX, height);

    srcRectCoord = {p1s, p2s, p3s, p4s};
    dstRectCoord = {p1d, p2d, p3d, p4d};

    /* 1. calculating matrix between a img and bird view img. */
    Matx = getPerspectiveTransform(srcRectCoord, dstRectCoord);

    /* 2. calculating inverse matrix between a img and bird view img. */
    **invMatx = getPerspectiveTransform(dstRectCoord, srcRectCoord);

    /* 3. get perspective warping img */
    warpPerspective(imgUndistort, imgWarp, Matx, Size(width, height), INTER_LINEAR);

    if (showWarpZone)
    {
        for (int i = 0; i < 4; i++)
        {
            circle(imgUndistort, srcRectCoord[i], 10, Scalar(0, 0, 255), 3, LINE_AA);
        }
        line(imgUndistort, srcRectCoord[0], srcRectCoord[1], Scalar(0, 255, 0), 3, LINE_AA);
        line(imgUndistort, srcRectCoord[0], srcRectCoord[2], Scalar(0, 255, 0), 3, LINE_AA);
        line(imgUndistort, srcRectCoord[1], srcRectCoord[3], Scalar(0, 255, 0), 3, LINE_AA);
        line(imgUndistort, srcRectCoord[2], srcRectCoord[3], Scalar(0, 255, 0), 3, LINE_AA);
        imshow("undistorted", imgUndistort);
        imshow("imgWarp", imgWarp);
        waitKey(0);
    }

    return imgWarp;
};

int getMatrixVal(char calibration_matrix_file[], Mat *intrinsicRead, Mat *distCoeffsRead)
{
    /*matrix size variables*/
    int m = 0, n = 0;

    /* open the file that has calibrate parameters */
    ifstream in(calibration_matrix_file);
    if (in.fail())
    {
        cout
            << "FAILED :: IN STREAM ! :: LINE : "
            << __LINE__
            << "\tFILE : \t"
            << __FILE__
            << endl;
        return -1;
    }

    /* 1. camera matrix size. */
    in >> m >> n;

    // define matrix variable for reading intrinsic variables
    Mat intrinsicRead_tmp = Mat(m, n, 6);

    // 2. camera marix values.
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            in >> intrinsicRead_tmp.ptr<double>(i)[j];

    /* 3. distortion coefficients matrix size. */
    in >> m >> n;

    // define matrix variable for reading distCoeffs variables
    Mat distCoeffsRead_tmp = Mat(m, n, 6);

    /* 4. distortion coefficients matrix value. */
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            in >> distCoeffsRead_tmp.ptr<double>(i)[j];

    /* Make the Matrix toward output variables. */
    *intrinsicRead = intrinsicRead_tmp;
    *distCoeffsRead = distCoeffsRead_tmp;

    in.close();
    return 0;
};