#include <iostream>
#include <fstream>
#include "preprocImg.hpp"
#define MODE_HLS 1
#define MODE_LAB 2

char CALBR_FILE[256] = "cameraCalibrationData/out.txt";

Mat preprocImg(Mat img, Mat *invMatx)
{
    Mat imgUndistort, imgUnwarp, imgHLS;
    // Todos...
    // 사다리꼴, 사각형 좌표 초기화 필요함. 대략적으로 잡기.

    // undistort img.
    imgUndistort = undistortingImg(img, true);

    // perspective transform(3d -> 2d(bird's view))
    imgUnwarp = unWarpingImg(imgUndistort, &invMatx, false);

    // color space filtering
    imgHLS = Filtering_imgPixel(imgUnwarp, MODE_HLS);
    // imgLAB = Filtering_imgPixel(imgUnwarp, MODE_LAB);

    imshow("imgUnwarp", imgUnwarp);
    imshow("imgHLS", imgHLS);
    waitKey(0);

    // combined best color channel together.

    // return combined best output img.

    return img;
};

Mat Filtering_imgPixel(Mat imgUnwarp, int mode)
{
    int iret = 0;
    Mat imgConverted;
    Mat imgFiltered;
    Mat imgOUT = Mat(720, 1280, CV_8UC3);

    cvtColor(imgUnwarp, imgConverted, COLOR_BGR2RGB);
    if (mode == 1)
    {
        // MODE_HLS
        // hue max : 179, l and s max : 255
        cvtColor(imgConverted, imgConverted, COLOR_RGB2HLS);

        // color channel filtering.
        // help -->  https://stackoverflow.com/questions/7899108/opencv-get-pixel-channel-value-from-mat-image

        // imgConverted.rows : 720
        // imgConverted.cols : 1280

        uint8_t *pixelPtr = (uint8_t *)imgConverted.data;
        int cn = imgConverted.channels();
        Scalar_<uint8_t> hlsPixel;
        for (int i = 0; i < imgConverted.rows; i++)
        {
            for (int j = 0; j < imgConverted.cols; j++)
            {
                /* when it needs to handle each channel value. */
                // hlsPixel.val[0] = pixelPtr[i * imgConverted.cols * cn + j * cn + 0];
                // hlsPixel.val[1] = pixelPtr[i * imgConverted.cols * cn + j * cn + 1];
                // hlsPixel.val[2] = pixelPtr[i * imgConverted.cols * cn + j * cn + 2];

                imgOUT.at<Vec3b>(i, j)[0] = pixelPtr[i * imgConverted.cols * cn + j * cn + 0];
                // imgOUT.at<Vec3b>(i, j)[1] = pixelPtr[i * imgConverted.cols * cn + j * cn + 1];
                imgOUT.at<Vec3b>(i, j)[1] = 0;
                imgOUT.at<Vec3b>(i, j)[2] = pixelPtr[i * imgConverted.cols * cn + j * cn + 2];
            }
        }
    }
    else if (mode == 2)
    {
        // MODE_LAB
        cvtColor(imgUnwarp, imgConverted, COLOR_BGR2Lab);
    }

    // return imgConverted;
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

    width = imgUndistort.cols;
    height = imgUndistort.rows;
    // width : 1280, height : 720

    Point p1s = Point2f(width / 2 - 190, height - 200);
    Point p2s = Point2f(width / 2 + 190, height - 200);
    Point p3s = Point2f(240, height - 50.0);
    Point p4s = Point2f(width - 240.0, height - 50.0);

    Point p1d = Point2f(400, 0);
    Point p2d = Point2f(width - 400, 0);
    Point p3d = Point2f(400, height);
    Point p4d = Point2f(width - 400, height);

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