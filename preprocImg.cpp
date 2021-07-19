#include <iostream>
#include <fstream>
#include "preprocImg.hpp"
#include "common.hpp"

char CALBR_FILE[256] = "cameraCalibrationData/out.txt";

Mat preprocImg(Mat img, Mat *invMatx)
{
    Mat imgUndistort, imgUnwarp;
    Mat imgHLS_L;
    Mat imgLAB_B;
    Mat combineOut;

    /* undistort img. */
    imgUndistort = undistortingImg(img, true);

    /* perspective transform(3d -> 2d(bird's view)) */
    imgUnwarp = unWarpingImg(imgUndistort, &invMatx, false);

    /* normalize img pixel for each color channel */
    imgHLS_L = normalize_HLS_L(imgUnwarp);
    imgLAB_B = normalize_LAB_B(imgUnwarp);

    /* combine hls and lab color pixel when either img pixel has 1 value or not. */
    combineOut = combine_both_img(imgHLS_L, imgLAB_B);

    // imshow("combined OUT", combineOut * 255);
    // imshow("imgUnwarp", imgUnwarp);

    return combineOut;
};

Mat combine_both_img(Mat hls, Mat lab)
{
    Mat imgOut;

    imgOut = make_zeros(hls);
    for (int i = 0; i < imgOut.rows; i++)
    {
        for (int j = 0; j < imgOut.cols; j++)
        {
            if (hls.at<uint8_t>(i, j) == 1 || lab.at<uint8_t>(i, j) == 1)
            {
                imgOut.at<uint8_t>(i, j) = 1;
            }
        }
    }
    return imgOut;
}

Mat make_zeros(Mat img)
{
    return Mat::zeros(img.rows, img.cols, img.type());
}

Mat make_ones(Mat img)
{
    return Mat::ones(img.rows, img.cols, img.type());
}

Mat normalize_HLS_L(Mat unWarp)
{
    /* normalizing L color channel pixel from hls img. */
    Mat imgHLS_L, imgNormal;
    double minVal, maxVal;
    Point minLoc, maxLoc;
    int lowThres = 220;

    // get a single channel img(filtered one.)
    imgHLS_L = filterImg(unWarp, HLS_CHANNEL, L_FILTER);

    // get max, min value of the matrix.
    minMaxLoc(imgHLS_L, &minVal, &maxVal, &minLoc, &maxLoc);

    // make normalized img.
    imgNormal = (255 / maxVal) * imgHLS_L;

    // apply threshold for L channel.
    Mat imgOut = make_zeros(imgNormal);
    threshold(imgNormal, imgOut, lowThres, 1, THRESH_BINARY);

    return imgOut;
}

Mat normalize_LAB_B(Mat unWarp)
{
    /* normalizing B color channel pixel from LAB img. */
    Mat imgLAB_B, imgNormal;
    double minVal, maxVal;
    Point minLoc, maxLoc;
    int yellowCrit = 175;
    int lowThres = 190;

    // get a single channel img(filtered one.)
    imgLAB_B = filterImg(unWarp, LAB_CHANNEL, B_FILTER_);

    // get max, min value of the matrix.
    minMaxLoc(imgLAB_B, &minVal, &maxVal, &minLoc, &maxLoc);

    // (conditional) make normalized img.
    // B channel means a range from blue(0) to yellow(255).
    // So, the bigger values, it becomes close to yellow color.(yellow lane)
    if (maxVal > yellowCrit)
    {
        imgNormal = (255 / maxVal) * imgLAB_B;
    }
    else
    {
        imgNormal = imgLAB_B;
    }

    // apply threshold for L channel.
    Mat imgOut = Mat::zeros(imgNormal.rows, imgNormal.cols, imgNormal.type());
    threshold(imgNormal, imgOut, lowThres, 1, THRESH_BINARY);

    return imgOut;
}

Mat combine_threshold(Mat gray)
{
    Mat sobelx, sobely;
    Mat magOut, dirOut;
    Mat binaryOut = Mat::zeros(gray.rows, gray.cols, gray.type());

    int edgeThres[2] = {50, 190};
    int magThres[2] = {50, 190};
    double dirThres[2] = {0.3, 1.51};

    int magKernel = 3;
    int dirKernel = 15;

    /* PIPELINE for img preprocess */
    // 1. sobel operation(x,y)
    sobelx = absSobel_Thres(gray, 1, 0, edgeThres);
    sobely = absSobel_Thres(gray, 0, 1, edgeThres);

    // 2. calculate MAG mat.
    magOut = grayTo_Mag(gray, magKernel, magThres);

    // 3. calculate DIR mat.
    dirOut = grayTo_Dir(gray, dirKernel, dirThres);

    // 4. find combined pixel Mat.
    for (int i = 0; i < gray.rows; i++)
    {
        for (int j = 0; j < gray.cols; j++)
        {
            if ((sobelx.at<uint8_t>(i, j) == 1 && sobely.at<uint8_t>(i, j) == 1) ||
                (magOut.at<uint8_t>(i, j) == 1 && dirOut.at<uint8_t>(i, j) == 1))
            {
                binaryOut.at<uint8_t>(i, j) = 1;
            }
        }
    }

    // visualization
    // imshow("sobelx", sobelx * 255);
    // imshow("sobely", sobely * 255);
    // imshow("magOut", magOut * 255);
    // imshow("dirOut", dirOut * 255);
    // imshow("combine", binaryOut * 255);
    // waitKey(0);

    return binaryOut;
}

Mat get_oneFromZeros_withinBound(Mat img, int boundary_thres[])
{
    Mat binary_out;

    // by using opencv function, apply high and low threshold boundary.
    Mat min_bin_thres_sobel = Mat::zeros(img.rows, img.cols, img.type());
    Mat max_bin_thres_sobel = Mat::zeros(img.rows, img.cols, img.type());
    threshold(img, min_bin_thres_sobel, boundary_thres[0], 1, THRESH_BINARY);
    threshold(img, max_bin_thres_sobel, boundary_thres[1], 1, THRESH_BINARY_INV);

    // make absolute value and converting back to 8 bit.
    convertScaleAbs(min_bin_thres_sobel, min_bin_thres_sobel);
    convertScaleAbs(max_bin_thres_sobel, max_bin_thres_sobel);

    // bitwise and operation.
    bitwise_and(min_bin_thres_sobel, max_bin_thres_sobel, binary_out);

    return binary_out;
}

Mat grayTo_Dir(Mat gray, int dirKernelSize, double dir_threshold[])
{
    Mat sobelX, sobelY;
    Mat min_dir, max_dir;
    Mat binaryOutput;

    // sobel edge both x and y direction
    Sobel(gray, sobelX, CV_64F, 1, 0, dirKernelSize, 1, 0, BORDER_DEFAULT);
    Sobel(gray, sobelY, CV_64F, 0, 1, dirKernelSize, 1, 0, BORDER_DEFAULT);
    sobelX = abs(sobelX);
    sobelY = abs(sobelY);

    // GET gradient direction by calculating arctan value for absoute ones.
    Mat gradDir = Mat::ones(sobelX.rows, sobelX.cols, sobelX.type());
    for (int i = 0; i < sobelX.rows; i++)
    {
        for (int j = 0; j < sobelX.cols; j++)
        {
            double gradRadian = 0.0;
            gradRadian = atan2(
                sobelY.at<double>(i, j),
                sobelX.at<double>(i, j));
            if (gradRadian > dir_threshold[0] && gradRadian < dir_threshold[1])
            {
                gradDir.at<double>(i, j) = 0.0;
            }
        }
    }

    convertScaleAbs(gradDir, gradDir);

    return gradDir;
}

Mat grayTo_Mag(Mat gray, int magKernelSize, int mag_threshold[])
{
    Mat sobelX, sobelY;
    Mat magSobel;
    Mat binaryOutput;

    // sobel edge both x and y direction
    Sobel(gray, sobelX, CV_64F, 1, 0, magKernelSize, 1, 0, BORDER_DEFAULT);
    Sobel(gray, sobelY, CV_64F, 0, 1, magKernelSize, 1, 0, BORDER_DEFAULT);

    // calculate sum of sqrt value for x and y gradient(each sobel output)
    pow(sobelX, 2, sobelX);
    pow(sobelY, 2, sobelY);
    sqrt(sobelX + sobelY, magSobel);

    binaryOutput = get_oneFromZeros_withinBound(magSobel, mag_threshold);

    return binaryOutput;
};

Mat absSobel_Thres(Mat imgGray, int dX, int dY, int edge_threshold[])
{
    Mat raw_grad, abs_grad;
    Mat grad;
    Mat binary_out;
    int ddepth = CV_64F;
    double minVal, maxVal;

    // gradient either X or Y pixel direction.
    Sobel(imgGray, raw_grad, ddepth, dX, dY, 3, 1, 0, BORDER_DEFAULT);
    abs_grad = abs(raw_grad);

    binary_out = get_oneFromZeros_withinBound(abs_grad, edge_threshold);

    return binary_out;
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

    int upX_diff = 100;
    int upY_diff = 260;
    int downX_diff = 230;
    int downY_diff = 80;

    int dstX = 350;

    width = imgUndistort.cols;
    height = imgUndistort.rows;
    // width : 1280, height : 720

    Point p1s = Point2f(width / 2 - upX_diff, height - upY_diff);
    Point p2s = Point2f(width / 2 + upX_diff, height - upY_diff);
    Point p3s = Point2f(downX_diff, height - downY_diff);
    Point p4s = Point2f(width - downX_diff, height - downY_diff);

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