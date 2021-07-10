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

    Mat dstImg;
    // 이렇게 하면 모든 위치의 픽셀 값이 지정한 하나의 값으로 채워진다.
    // Mat channel_B = Mat(imgBinary.rows, imgBinary.cols, CV_8UC1, 100);
    // Mat channel_G = Mat(imgBinary.rows, imgBinary.cols, CV_8UC1, 100);
    // Mat channel_R = Mat(imgBinary.rows, imgBinary.cols, CV_8UC1, 100);

    Mat channel_B = Mat::zeros(imgBinary.rows, imgBinary.cols, CV_8UC1);
    Mat channel_G = Mat::zeros(imgBinary.rows, imgBinary.cols, CV_8UC1);
    Mat channel_R = Mat::zeros(imgBinary.rows, imgBinary.cols, CV_8UC1);

    // 그래서, 모든 값을 0으로 셋팅하고, imgBinary 값을 이용해서 loop를 돌린다
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

    /* 
    np.dstack(vector A, vector B, vector C) 와 같은 작업을 C++로 실행
    각 채널의 픽셀 값을 바이너리 이미지의 픽셀 값으로 채워준다.
    바이너리에서 각 위치에서 픽셀 값은 0 또는 1(검출된 곳) 으로 분류 되어 있다.
    
    dstImg -> 8U3U 타입의 이미지 변수가 된다.
    */
    vector<Mat> channels = {channel_B, channel_G, channel_R};

    // 3개의 채널을 가지고 있는 Mat벡터를 Merge해서 dstImg를 반환한다.
    // dstImg :: CV_8UC3
    merge(channels, dstImg);

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

        yLeft = (int)round(coeffsLR[0][0] + coeffsLR[0][1] * xPix[i] + coeffsLR[0][2] * xPix[i] * xPix[i]);
        yRight = (int)round(coeffsLR[1][0] + coeffsLR[1][1] * xPix[i] + coeffsLR[1][2] * xPix[i] * xPix[i]);

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

    calcLaneImg(imgBinary, &rectWindowInfo, &pixelPosXY, &coeffsLR);
    polyfit_using_prev_fitCoeffs(imgBinary, coeffsLR, &pixelPosXYNext, &coeffsLRNext);

    // (todo) 부드러운 형상의 레인 바운더리를 형성함.
}

void polyfit_using_prev_fitCoeffs(
    Mat imgBinary,
    vector<vector<double>> coeffsLR,
    vector<vector<Point>> *pixelPosXYNext,
    vector<vector<double>> *coeffsLRNext)
{
    Mat nonZeros;
    findNonZero(imgBinary, nonZeros);

    // todo : 레퍼런스에서 제시하고 있는 다항식 계산에 사용된 x,y 에 대한 데이터 사용이 반대인 듯.
    // 나 , 이미지의 폭방향 x 높이 방향 y
    // 레퍼런스, 이미지의 높이 방향 : x, 이미지의 폭방향 : y
    // --> 내가 한 방법으로 하면, 폭방향의 새로운 기준 위치를 찾아주기 위해, 다항식의 해를 구하는 방법으로 접근해야한다. 너무 복잡함. 그러므로, 이 과정을 쉽게 처리하기 위해서 x, y의 대입 방식을 다시 고쳐줄 필요가 있다. 그렇게 되면, 폭방향의 새로운 위치는, 이전에 구한 다항식에서 미지수 값만 대입시킨 f(x) 값을 그대로 사용할 수 있다.
}