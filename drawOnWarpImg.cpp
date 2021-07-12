#include <iostream>

#include "common.hpp"
#include "calcLaneImg.hpp"
#include "drawOnWarpImg.hpp"

void drawOnWarpImg(Mat imgBinary)
{

    vector<vector<double>> coeffsLR;
    vector<vector<Rect>> rectWindowInfo;
    vector<vector<Point>> pixelPosXY;
    calcLaneImg(imgBinary, &rectWindowInfo, &pixelPosXY, &coeffsLR);

    Mat dstImg = make3ChImg(imgBinary);

    // polynomial coefficients를 이용해서, fit data를 이미지에 출력한다.
    // 2차 방정식에 사용할 x인풋을 생성
    vector<int> xPix;
    for (int i = 0; i < imgBinary.rows; i++)
    {
        xPix.push_back(i);
    }

    // Define yPix variables and push the value on the vector variables.
    vector<int> yPixLeft;
    vector<int> yPixRight;

    // By using 2nd-order polynomial coefficients, calculate y Pixl Value and push on the vector array.
    for (int i = 0; i < imgBinary.rows; i++)
    {
        int yLeft = 0;
        int yRight = 0;

        yLeft = calcPoly(xPix[i], coeffsLR[0]);
        yRight = calcPoly(xPix[i], coeffsLR[1]);

        yPixLeft.push_back(yLeft);
        yPixRight.push_back(yRight);
    }

    // give colored pixel value for fitted position from the polynomial.
    // 1. Left Lane
    for (int i = 0; i < imgBinary.rows; i++)
    {
        if (yPixLeft[i] > 0 && yPixLeft[i] < dstImg.cols)
        {
            dstImg.at<Vec3b>(xPix[i], yPixLeft[i]) = {0, 0, 255};
        }
    }

    // 2. Right Lane
    for (int i = 0; i < imgBinary.rows; i++)
    {
        if (yPixRight[i] > 0 && yPixRight[i] < dstImg.cols)
        {
            dstImg.at<Vec3b>(xPix[i], yPixRight[i]) = {0, 0, 255};
        }
    }

    drawRectangle(dstImg, rectWindowInfo);

    imshow("dstImg", dstImg);
    waitKey(0);
    // make uint 8U img.
    // give pixel value(255,255,255) to each index of mat.
}

void drawRectangle(Mat dstImg, vector<vector<Rect>> rectWindowInfo)
{
    int numRect = rectWindowInfo.size();

    if (numRect == 0)
    {
        //(TODO) end this function...
    }

    for (int i = 0; i < rectWindowInfo.size(); i++)
    {
        // left side
        rectangle(dstImg, rectWindowInfo[i][0], Scalar(255, 255, 0), 3, LINE_AA);
        // right side
        rectangle(dstImg, rectWindowInfo[i][1], Scalar(0, 255, 0), 3, LINE_AA);
    }
}

void drawPolygonAndFill(Mat imgBinary)
{
    vector<vector<double>> coeffsLR, coeffsLRNext;
    vector<vector<Rect>> rectWindowInfo;
    vector<vector<Point>> pixelPosXY, pixelPosXYNext;
    Mat dstImg = make3ChImg(imgBinary);

    calcLaneImg(imgBinary, &rectWindowInfo, &pixelPosXY, &coeffsLR);
    polyfit_using_prev_fitCoeffs(imgBinary, coeffsLR, &pixelPosXYNext, &coeffsLRNext);

    // with new coeffs, calculate each lane center position.
    vector<Point> leftLanePts;
    vector<Point> rightLanePts;

    for (int i = 0; i < imgBinary.rows; i++)
    {
        leftLanePts.push_back(Point(calcPoly(i, coeffsLRNext[LEFT_LANE]), i));
        rightLanePts.push_back(Point(calcPoly(i, coeffsLRNext[RIGHT_LANE]), i));
    }

    vector<Point> leftPolyPts;
    vector<Point> rightPolyPts;
    int margin = 80;

    for (int i = imgBinary.rows; i >= 0; i--)
    {
        leftPolyPts.push_back(Point(leftLanePts[i].x - margin, i));
        rightPolyPts.push_back(Point(rightLanePts[i].x - margin, i));
    }

    for (int i = 0; i < imgBinary.rows; i++)
    {
        leftPolyPts.push_back(Point(leftLanePts[i].x + margin, i));
        rightPolyPts.push_back(Point(rightLanePts[i].x + margin, i));
    }

    // do fillpoly function with pixel Pos variables.
    fillPoly(dstImg, leftPolyPts, Scalar(255, 0, 0), LINE_AA);
    fillPoly(dstImg, rightPolyPts, Scalar(0, 0, 255), LINE_AA);

    // (todo) addweighted funtion....
    imshow("dstImg", dstImg);
    waitKey(0);
}

void polyfit_using_prev_fitCoeffs(
    Mat imgBinary,
    vector<vector<double>> coeffsLR,
    vector<vector<Point>> *pixelPosXYNext,
    vector<vector<double>> *coeffsLRNext)
{
    Mat nonZeros;
    findNonZero(imgBinary, nonZeros);

    /* 
    with old polynomial coefficients, Calculating new lane pixel index. 
        pixelPosXYNext[0] : left
        pixelPosXYNext[1] : right
    */
    *pixelPosXYNext = getIndexArrayWithPolyCoeffs(nonZeros, coeffsLR);

    vector<double> inCoordLeft;
    vector<double> outCoordLeft;
    vector<double> inCoordRight;
    vector<double> outCoordRight;

    makeOneDirectionArray(*pixelPosXYNext, LEFT_LANE, &inCoordLeft, &outCoordLeft);
    makeOneDirectionArray(*pixelPosXYNext, RIGHT_LANE, &inCoordRight, &outCoordRight);

    // calculating polyfit coefficients.
    vector<double> polyCoeffsLeft_new;
    vector<double> polyCoeffsRight_new;
    polyCoeffsLeft_new = polyFit_cpp(inCoordLeft, outCoordLeft, 2);
    polyCoeffsRight_new = polyFit_cpp(inCoordRight, outCoordRight, 2);

    // push back on coeffs vector.
    coeffsLRNext->push_back(polyCoeffsLeft_new);
    coeffsLRNext->push_back(polyCoeffsRight_new);
}

Mat make3ChImg(Mat imgBinary)
{
    Mat dstImg;
    Mat channel_B = Mat::zeros(imgBinary.rows, imgBinary.cols, CV_8UC1);
    Mat channel_G = Mat::zeros(imgBinary.rows, imgBinary.cols, CV_8UC1);
    Mat channel_R = Mat::zeros(imgBinary.rows, imgBinary.cols, CV_8UC1);

    for (int i = 0; i < imgBinary.rows; i++)
    {
        for (int j = 0; j < imgBinary.cols; j++)
        {
            if (imgBinary.at<uint8_t>(i, j) == 1)
            {
                // set white color.
                channel_B.at<uint8_t>(i, j) = 255;
                channel_G.at<uint8_t>(i, j) = 255;
                channel_R.at<uint8_t>(i, j) = 255;
            }
        }
    }

    /* dstImg -> 8U3U 타입의 이미지 변수가 된다. */
    vector<Mat> channels = {channel_B, channel_G, channel_R};

    // 3개의 채널을 가지고 있는 Mat벡터를 Merge해서 dstImg를 반환한다.
    // dstImg :: CV_8UC3
    merge(channels, dstImg);
    return dstImg;
}