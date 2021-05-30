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
    Mat imgSobelX, imgSobelY;
    Mat sobelMag, sobelDir;
    int edgeLow = 50, edgeHigh = 150;
    int magLow = 100, magHigh = 190;
    double dirLow = 0, dirHigh = CV_PI / 2;

    int magKernelSize = 3;
    int dirKernelSize = 3;

    int mag_threshold[2] = {magLow, magHigh};
    int edge_threshold[2] = {edgeLow, edgeHigh};
    double dir_threshold[2] = {dirLow, dirHigh};

    /* undistort img. */
    imgUndistort = undistortingImg(img, true);

    /* perspective transform(3d -> 2d(bird's view)) */
    imgUnwarp = unWarpingImg(imgUndistort, &invMatx, false);

    /* color space filtering, returned gray scale IMG. 
     HLS, L channel is the best for detecting white lane. 
     LAB, B channel is the best for detecting yellow lane. */
    imgHLS_L = filterImg(imgUnwarp, HLS_CHANNEL, L_FILTER);
    imgLAB_B = filterImg(imgUnwarp, LAB_CHANNEL, B_FILTER_);

    // imgSobelX = absSobel_Thres(imgLAB_B, 1, 0, edgeLow, edgeHigh);
    // imgSobelY = absSobel_Thres(imgLAB_B, 0, 1, edgeLow, edgeHigh);

    imgSobelX = absSobel_Thres(imgHLS_L, 1, 0, edge_threshold);
    imgSobelY = absSobel_Thres(imgHLS_L, 0, 1, edge_threshold);

    sobelMag = grayTo_Mag(imgHLS_L, magKernelSize, mag_threshold);
    sobelDir = grayTo_Dir(imgHLS_L, dirKernelSize, dir_threshold);

    imshow("Sobel X on Boundary. ", imgSobelX);
    imshow("Sobel Y on Boundary. ", imgSobelY);
    imshow("Sobel MAG on Boundary. ", sobelMag);
    imshow("Sobel DIR on Boundary. ", sobelDir);

    // imshow("LAB img, B filterd.", imgLAB_B);
    imshow("HLS img, L filterd.", imgHLS_L);

    imshow("imgUnwarp", imgUnwarp);
    waitKey(0);

    return img;
};

Mat grayTo_Dir(Mat gray, int dirKernelSize, double dir_threshold[])
{
    Mat sobelX, sobelY;
    Mat sobelXAbs, sobelYAbs;
    Mat binaryOutput;

    // sobel edge both x and y direction
    Sobel(gray, sobelX, CV_64F, 1, 0, dirKernelSize, 1, 0, BORDER_DEFAULT);
    Sobel(gray, sobelY, CV_64F, 0, 1, dirKernelSize, 1, 0, BORDER_DEFAULT);

    convertScaleAbs(sobelX, sobelXAbs);
    convertScaleAbs(sobelY, sobelYAbs);

    // calculate gradient direction by calculating arctan value for absoute one.
    Mat absGradDir = Mat::zeros(sobelXAbs.rows, sobelYAbs.cols, CV_64F);
    Mat absGradDir_8b;

    int cnt = 0;

    for (int i = 0; i < sobelXAbs.rows; i++)
    {
        for (int j = 0; j < sobelXAbs.cols; j++)
        {
            double temp = 0.0;
            absGradDir.at<double>(i, j) = atan2(
                (double)sobelYAbs.at<uint8_t>(i, j),
                (double)sobelXAbs.at<uint8_t>(i, j));

            temp = absGradDir.at<double>(i, j);
            if (temp != 0.0)
            {
                // cout << i << "," << j << ", absGradDir Value :: " << temp << endl;
                cnt++;
            }
            /* code */
        }
    }

    cout << "cnt :: " << cnt << endl;

    // cout << "basGradDir :: " << absGradDir << endl;

    // scale down to 8 bit
    convertScaleAbs(absGradDir, absGradDir_8b);

    // temp...
    absGradDir_8b = absGradDir_8b * 100;
    // thresholding with sum of sqrt value.
    // threshold(absGradDir_8b, binaryOutput, dir_threshold[0], dir_threshold[1], THRESH_BINARY);

    return absGradDir_8b;
}

Mat grayTo_Mag(Mat gray, int magKernelSize, int mag_threshold[])
{
    Mat sobelX, sobelY;
    Mat magSobel, magSobel_8b;
    Mat binaryOutput;

    // sobel edge both x and y direction
    Sobel(gray, sobelX, CV_64F, 1, 0, magKernelSize, 1, 0, BORDER_DEFAULT);
    Sobel(gray, sobelY, CV_64F, 0, 1, magKernelSize, 1, 0, BORDER_DEFAULT);

    // calculate sum of sqrt value for x and y gradient(each sobel output)
    pow(sobelX, 2, sobelX);
    pow(sobelY, 2, sobelY);
    sqrt(sobelX + sobelY, magSobel);

    // scale down to 8 bit
    // magSobel is already have absolute values. It is just changed to scale down to 8 bit.
    convertScaleAbs(magSobel, magSobel_8b);

    // thresholding with sum of sqrt value.
    threshold(magSobel_8b, binaryOutput, mag_threshold[0], mag_threshold[1], THRESH_BINARY);

    return binaryOutput;
};

Mat absSobel_Thres(Mat imgGray, int dX, int dY, int edge_threshold[])
{
    Mat raw_grad, abs_grad;
    Mat grad, bin_thres_sobel;
    Mat binary_sobel, binary_out;
    int ddepth = CV_64F;
    double minVal, maxVal;

    // gradient either X or Y pixel direction.
    Sobel(imgGray, raw_grad, ddepth, dX, dY, 3, 1, 0, BORDER_DEFAULT);

    // make absolute value and converting back to 8 bit.
    convertScaleAbs(raw_grad, abs_grad);

    // (by using opencv function)apply high and low threshold boundary.
    threshold(abs_grad, bin_thres_sobel, edge_threshold[0], edge_threshold[1], THRESH_BINARY);

    return bin_thres_sobel;
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