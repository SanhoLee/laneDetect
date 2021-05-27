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
    Mat imgSobelX, imgSobelY, imgSobelMag;

    /* undistort img. */
    imgUndistort = undistortingImg(img, true);

    /* perspective transform(3d -> 2d(bird's view)) */
    imgUnwarp = unWarpingImg(imgUndistort, &invMatx, false);

    /* color space filtering 
     HLS, L channel is the best for detecting white lane. 
     LAB, B channel is the best for detecting yellow lane. */
    imgHLS_L = filterImg(imgUnwarp, HLS_CHANNEL, L_FILTER);
    imgLAB_B = filterImg(imgUnwarp, LAB_CHANNEL, B_FILTER_);

    cout << "HLS_L channel :: " << imgHLS_L.channels() << endl;
    cout << "LAB_B channel :: " << imgLAB_B.channels() << endl;

    imshow("HLS - L filter", imgHLS_L);
    imshow("LAB - B filter", imgLAB_B);

    // todos...
    /* Gradient Threshold :: edge detector :: sobel edge */
    // imgSobelX = absSobelEdge(imgHLS_L, 1, 0);
    // imgSobelY = absSobelEdge(imgHLS_L, 0, 1);

    // imshow("L filtered - Sobel X ", imgSobelX);
    // imshow("L filtered - Sobel Y ", imgSobelY);

    imshow("imgUnwarp", imgUnwarp);
    waitKey(0);

    return img;
};

Mat absSobelEdge(Mat imgSRC, int dX, int dY)
{
    Mat raw_grad, abs_grad;
    Mat grad, scaled_sobel;
    Mat binary_sobel, binary_out;
    int ddepth = CV_64F;
    int low_Thres = 50;
    int high_Thres = 150;
    double minVal, maxVal;

    // gradient either X or Y pixel direction.
    Sobel(imgSRC, raw_grad, ddepth, dX, dY, 3, 1, 0, BORDER_DEFAULT);

    // make absolute value and converting back to 8 bit.
    convertScaleAbs(raw_grad, abs_grad);

    // cout << abs_grad.at<Vec3b>(50, 868) << endl;
    // abs_grad.at<Vec3b>(50, 868)[2] = 100;
    // cout << abs_grad.at<Vec3b>(50, 868) << endl;

    // declare zero mat.
    scaled_sobel = Mat::zeros(abs_grad.rows, abs_grad.cols, CV_8UC1);

    // for (int i = 0; i < scaled_sobel.rows; i++)
    // {
    //     for (int j = 0; j < scaled_sobel.cols; j++)
    //     {
    //         cout << scaled_sobel.at<Vec3b>(i, j) << endl;
    //     }
    // }

    // apply high and low threshold boundary.
    // for (int i = 0; i < abs_grad.rows; i++)
    // {
    //     for (int j = 0; j < abs_grad.cols; j++)
    //     {
    //         if (abs_grad.at<Vec3b>(i, j)[1] > low_Thres && abs_grad.at<Vec3b>(i, j)[1] < high_Thres)
    //         {
    //             // cout << abs_grad.at<Vec3b>(i, j) << endl;
    //             cout << scaled_sobel.at<int>(i, j) << endl;
    //         }
    //     }
    // }

    // todos
    // give value 1 if the gray scalor value is on Thres boundary.
    // number of channel : 3 (abs_grad)
    // for (int i = 0; i < scaled_sobel.rows; i++)
    // {
    //     for (int j = 0; j < scaled_sobel.cols; j++)
    //     {
    //         cout << scaled_sobel.at<uint8_t>(i, j) << endl;
    //     }
    // }

    // return scaled_sobel;
    return abs_grad;
}

void applySobelThres(Mat unWarp, Mat filteredImg, int maxThres, int minThres)
{
}

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
    // Mat imgOUT = Mat::zeros(720, 1280, CV_8UC3);
    Mat imgOUT = Mat::zeros(720, 1280, CV_8UC1);

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
                imgOUT.at<uint8_t>(i, j) = pixelPtr[i * imgConverted.cols * cn + j * cn + 0];
            }
        }
        break;
    case 1:
        // set L space Only  // set A space Only
        for (int i = 0; i < imgConverted.rows; i++)
        {
            for (int j = 0; j < imgConverted.cols; j++)
            {
                imgOUT.at<uint8_t>(i, j) = pixelPtr[i * imgConverted.cols * cn + j * cn + 1];
            }
        }
        break;

    case 2:
        // set S space Only  // set B space Only
        for (int i = 0; i < imgConverted.rows; i++)
        {
            for (int j = 0; j < imgConverted.cols; j++)
            {
                imgOUT.at<uint8_t>(i, j) = pixelPtr[i * imgConverted.cols * cn + j * cn + 2];
            }
        }
        break;

    default:
        break;
    }

    /* 
    COLORMAP for grayscale.     
    ref : https://learnopencv.com/applycolormap-for-pseudocoloring-in-opencv-c-python/

     applyColorMap(imgOUT, imgOUT, COLORMAP_BONE);
     컬러맵을 쓰니깐 채널이 1개에서 3개가 되버렸다. 이 메쏘드는 안쓰는게 맞는듯.
     컬러맵은 그레이 스케일에 대해서, 색상을 역으로 부여하는 꼴이 되기 때문에 채널이 3개로 늘어난 듯.
    */

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
    int dstX = 350;

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