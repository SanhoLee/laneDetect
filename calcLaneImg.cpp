#include <iostream>
#include <numeric>
#include "calcLaneImg.hpp"
#include "common.hpp"
#include "struct_drawData.h"

// minimum pixel for left and right window search.
int minNumPix = 40;
int numWindow = 10;

drawDataInfo calcImg(Mat imgBinary)
{
    /* return this.
    
     vector<Point> leftLanePts;
     vector<Point> rightLanePts;
     double leftRadius;
     double rightRadius;
     double centerOffset;
    */
    vector<vector<double>> coeffsLR;
    vector<vector<Rect>> rectWindowInfo;
    vector<vector<Point>> pixelPosXY;

    // initializing struct variable
    drawDataInfo drawDataStruct;
    drawDataStruct.leftRadius = 0.0;
    drawDataStruct.rightRadius = 0.0;
    drawDataStruct.centerOffset = 0.0;

    calcLaneImg(imgBinary, &rectWindowInfo, &pixelPosXY, &coeffsLR);
    calcLaneRadiusAndCenter(
        imgBinary,
        pixelPosXY,
        &drawDataStruct.leftRadius,
        &drawDataStruct.rightRadius,
        &drawDataStruct.centerOffset);

    for (int i = 0; i < imgBinary.rows; i++)
    {
        drawDataStruct.leftLanePts.push_back(Point(calcPoly(i, coeffsLR[LEFT_LANE]), i));
        drawDataStruct.rightLanePts.push_back(Point(calcPoly(i, coeffsLR[RIGHT_LANE]), i));
    }

    return drawDataStruct;
}

// vector<vector<double>> calcLaneImg(Mat imgCombined, vector<vector<Rect>> rectWindowInfo, vector<vector<Point>> pixelPosXY, vector<vector<double>> coeffsLR)
void calcLaneImg(
    Mat imgCombined,
    vector<vector<Rect>> *rectWindowInfo,
    vector<vector<Point>> *pixelPosXY,
    vector<vector<double>> *coeffsLR)
{
    /*  
        description :
            Do Window Search.
            find 2nd Polynomial Coefficient and get x, y pixel for detected lanes.
        input   :
            imgCombined(binary img src.)
        return  : 
            rectWindowInfo
            pixelPosXY
            coeffsLR

            Data by calculatin for 2nd-order polynomial equation.
            And some source for vizualiztion the detected and fitted pixel position.
    */

    // container for each side window pixel
    vector<vector<Point>> leftPixelContainer;
    vector<vector<Point>> rightPixelContainer;

    /* 1. get half down space. */
    Mat halfdown = halfDownImg(imgCombined);

    /* 2. find sum value from each column pixel data. */
    vector<int> sumArray = sumColElm(halfdown);

    /* 3. get left and right X coordinate for base position. */
    int leftX_base = getLeftX_base(sumArray);
    int rightX_base = getRightX_base(sumArray);

    /* 4. window search squre data */
    winSearchImg(imgCombined,
                 numWindow,
                 {leftX_base, rightX_base},
                 &rectWindowInfo,
                 &leftPixelContainer,
                 &rightPixelContainer);

    /* 5. coefficient for the 2nd polynomial equation */
    // concatenating two dimension vector -> one dimension vector.
    vector<Point> leftPixelPosXY_OneDim = dimDownFrom2To1(leftPixelContainer);
    vector<Point> rightPixelPosXY_OneDim = dimDownFrom2To1(rightPixelContainer);

    // return pixelPosXY
    pixelPosXY->push_back(leftPixelPosXY_OneDim);
    pixelPosXY->push_back(rightPixelPosXY_OneDim);

    // Starting Fitting 2nd-order polynomial equation.
    vector<double> inCoordLeft;
    vector<double> outCoordLeft;
    vector<double> inCoordRight;
    vector<double> outCoordRight;

    vector<double> polyCoeffsLeft;
    vector<double> polyCoeffsRight;

    // make each x and y array from point container.
    makeOneDirectionArray(*pixelPosXY, LEFT_LANE, &inCoordLeft, &outCoordLeft);
    makeOneDirectionArray(*pixelPosXY, RIGHT_LANE, &inCoordRight, &outCoordRight);

    polyCoeffsLeft = polyFit_cpp(inCoordLeft, outCoordLeft, 2);
    polyCoeffsRight = polyFit_cpp(inCoordRight, outCoordRight, 2);

    // ????????? ?????? ??????
    coeffsLR->push_back(polyCoeffsLeft);
    coeffsLR->push_back(polyCoeffsRight);
}

Mat halfDownImg(Mat original)
{
    return original(Range(original.rows / 2, original.rows), Range(0, original.cols));
}

vector<int> sumColElm(Mat img_binary)
{
    vector<int> sumArray;
    for (int i = 0; i < img_binary.cols; i++)
    {
        // make one column matrix.
        Mat oneColumn = img_binary.col(i);

        // sum all element and push back on a array.
        int sumResult = sum(oneColumn)[0];
        sumArray.push_back(sumResult);
    }

    return sumArray;
}

int getLeftX_base(vector<int> sumArray)
{
    int leftX_base;

    int midPoint = sumArray.size() / 2;
    int qtrPoint = midPoint / 2;

    // This is for get subset of vector range.
    vector<int>::const_iterator begin = sumArray.begin();
    vector<int>::const_iterator last = sumArray.begin() + sumArray.size();

    // full width : 1280px
    // Get pixel data from 1/4 width ~ 2/4 width(or 2/4 ~ 3/4 width for right side.) for sumArray matrix.
    // position should be referenced with 'begin' iterator.
    vector<int> left_qtr(begin + qtrPoint, begin + midPoint);

    // max index and value from a certain array.
    int leftMaxIndex = max_element(left_qtr.begin(), left_qtr.end()) - left_qtr.begin();
    // int leftMaxValue = *max_element(left_qtr.begin(), left_qtr.end());

    // adjust pixel index for global x width.
    leftX_base = leftMaxIndex + qtrPoint;

    return leftX_base;
}

int getRightX_base(vector<int> sumArray)
{
    int rightX_base;

    int midPoint = sumArray.size() / 2;
    int qtrPoint = midPoint / 2;

    // This is for get subset of vector range.
    vector<int>::const_iterator begin = sumArray.begin();
    vector<int>::const_iterator last = sumArray.begin() + sumArray.size();

    vector<int> right_qtr(begin + midPoint, begin + midPoint + qtrPoint);

    // max index and value from a certain array.
    int rightMaxIndex = max_element(right_qtr.begin(), right_qtr.end()) - right_qtr.begin();
    // int rightMaxValue = *max_element(right_qtr.begin(), right_qtr.end());

    // adjust pixel index for global x width.
    rightX_base = rightMaxIndex + midPoint;

    return rightX_base;
}

int mean_vectorArray(vector<Point> target_vector)
{
    int sum = 0;
    int mean = 0;

    // 1. sum
    for (int i = 0; i < target_vector.size(); i++)
    {
        sum = sum + target_vector[i].x;
    }

    // 2. divide by the size of vector.
    mean = (int)sum / target_vector.size();

    return mean;
}

vector<Point> getIndexArray_onWindow(Mat nonZeroPos, Rect windowBox)
{
    // commonly use for left and right side winodw.
    // vector<int> windowPoint_IDX;
    vector<Point> windowPoint_IDX;

    // define boundary window.
    int xLow = windowBox.x;
    int xHigh = xLow + windowBox.width;
    int yLow = windowBox.y;
    int yHigh = yLow + windowBox.height;

    // Put a certain Point that is on the window boundary.
    for (int i = 0; i < nonZeroPos.rows; i++)
    {
        int nonZeroX = nonZeroPos.at<Point>(i).x;
        int nonZeroY = nonZeroPos.at<Point>(i).y;

        // initializing Point variable.
        Point onePt = {0, 0};
        onePt = {nonZeroX, nonZeroY};

        // When non-zero position is on spcified Window Area.
        if (nonZeroX >= xLow && nonZeroX < xHigh && nonZeroY >= yLow && nonZeroY < yHigh)
        {
            windowPoint_IDX.push_back(onePt);
        }
    }

    return windowPoint_IDX;
}

vector<vector<Point>> getIndexArrayWithPolyCoeffs(
    Mat nonZeroPos,
    vector<vector<double>> coeffsLR)
{
    /* return : pixel pos index for left and right side. */
    vector<Point> leftLane_IDX;
    vector<Point> rightLane_IDX;

    int margin = 80;
    for (int i = 0; i < nonZeroPos.rows; i++)
    {
        // get calculated polyfit y value (width direction value.)
        Point inCurrent = nonZeroPos.at<Point>(i);
        int nonZeroX = inCurrent.x;
        int nonZeroY = inCurrent.y;

        int outFitLeft =
            (int)round(coeffsLR[0][0] + coeffsLR[0][1] * nonZeroY + coeffsLR[0][2] * pow(nonZeroY, 2));
        int outFitRight =
            (int)round(coeffsLR[1][0] + coeffsLR[1][1] * nonZeroY + coeffsLR[1][2] * pow(nonZeroY, 2));

        int widthLowLeft = outFitLeft - margin;
        int widthHighLeft = outFitLeft + margin;
        int widthLowRight = outFitRight - margin;
        int widthHighRight = outFitRight + margin;

        // In this condition, push back on the array
        if (nonZeroX >= widthLowLeft && nonZeroX < widthHighLeft)
        {
            leftLane_IDX.push_back(inCurrent);
        }

        if (nonZeroX >= widthLowRight && nonZeroX < widthHighRight)
        {
            rightLane_IDX.push_back(inCurrent);
        }
    }

    vector<vector<Point>> newLanePixelContainer = {leftLane_IDX, rightLane_IDX};

    return newLanePixelContainer;
}

void reCenterCurrentPos(vector<Point> pntIndexArray, int *currentXPos)
{
    /*
    parameter:
        INPUT
            pntIndexArray  : index array for x-coordinate from non-zero pixel array.
        I/O
            currentXPos     : window center position for x axis.
    */

    // check the length of pointIndexArray is bigger than minNumPix or not.
    if (pntIndexArray.size() > minNumPix)
    {
        // re-center window center x coordinates with mean values for the array.
        *currentXPos = mean_vectorArray(pntIndexArray);
    }
    else
    {
        // no need to process
    }
}

void winSearchImg(Mat preprocess,
                  int numWindow,
                  vector<int> xBase,
                  vector<vector<Rect>> **rectWindowInfo,
                  vector<vector<Point>> *leftPixelContainer,
                  vector<vector<Point>> *rightPixelContainer)
{
    // input        --> Mat preprocess, int numWindow, vector<int> xBase
    // return array --> rectWindowInfo, leftPixelContainer, rightPixelContainer

    Mat nonZeroPos;
    int window_height = preprocess.rows / numWindow;

    // nonZeroPos has non-zero pixel location info.
    findNonZero(preprocess, nonZeroPos);

    // create window for detecting line.
    // define variables...
    int win_yHigh;
    int win_yLow;
    int win_xLeft_low;
    int win_xRight_low;

    int leftX_base = xBase[0];
    int rightX_base = xBase[1];

    int margin = 80;
    int leftX_current = leftX_base;
    int rightX_current = rightX_base;

    // define each window box...
    // save on Vector variable.
    for (int i = 0; i < numWindow; i++)
    {
        win_yHigh = preprocess.rows - (i)*window_height;
        win_yLow = preprocess.rows - (i + 1) * window_height;
        win_xLeft_low = leftX_current - margin;
        win_xRight_low = rightX_current - margin;

        // (make array.)1. Rect object info for specific window.
        Rect curLeftWindow(win_xLeft_low, win_yLow, 2 * margin, window_height);
        Rect curRightWindow(win_xRight_low, win_yLow, 2 * margin, window_height);

        // return rectangle info.
        rectWindowInfo[0]->push_back({curLeftWindow, curRightWindow});

        // (make array.)2. found pixel index element for specific window.
        vector<Point> leftWindowPoint_IDX = getIndexArray_onWindow(nonZeroPos, curLeftWindow);
        vector<Point> rightWindowPoint_IDX = getIndexArray_onWindow(nonZeroPos, curRightWindow);
        leftPixelContainer->push_back(leftWindowPoint_IDX);
        rightPixelContainer->push_back(rightWindowPoint_IDX);

        /* 3. Updating if found the number of window pixel is bigger than min pixel, 
        re-center next window pos.    
        updating element : leftX_current, rightX_current
        */
        reCenterCurrentPos(leftWindowPoint_IDX, &leftX_current);
        reCenterCurrentPos(rightWindowPoint_IDX, &rightX_current);
    }
}

vector<Point> dimDownFrom2To1(vector<vector<Point>> twoDimVector)
{
    vector<Point> oneDimVector(twoDimVector[0]);

    for (int i = 0; i < twoDimVector.size() - 1; i++)
    {
        oneDimVector.insert(oneDimVector.end(), twoDimVector[i + 1].begin(), twoDimVector[i + 1].end());
    }

    return oneDimVector;
}

vector<double> polyFit_cpp(vector<double> xCoord, vector<double> yCoord, int polyOrder)
{
    /* return : coefficients of the polynomial equation.

     1. Do gaussian elimination.
     2. get coefficients for nth-order polynomial equation.
     3. build nth-order polynomial equation.

     INFORMATION : 
     gaussian elimination form.
     -> X * A = Y
     (X,Y is DATA Value. The target is A matrix, Coefficient of 2nd-order polynomial equation.)

     */

    // Setting X array(3 x 3) and Y-array(3 x 1)
    double xArr[polyOrder + 1][3];
    double yArr[polyOrder + 1][1];
    double aArr[polyOrder + 1][1];

    /* In order to Calculating a_matrix, Do gaussain elimination.
     ref : https://www.youtube.com/watch?v=2j5Ic2V7wq4 
    */

    // 1. init matrix.
    initGaussianMatrix(polyOrder, &xCoord, &yCoord, xArr, yArr);

    // 2. gaussian elimination. ?????? ?????? ?????? ?????? ?????? ????????? ?????? 0?????? ??????????????? ??????.
    makeZero_UnderDiagonalElement(xArr, yArr);

    // 3. gaussian elimination. Calculating a-array value.
    calcCoeffsValue(xArr, yArr, aArr);

    // transfer data from array to vector type.
    vector<double> rtnArr(sizeof(aArr) / sizeof(aArr[0]));

    for (int i = 0; i < sizeof(aArr) / sizeof(aArr[0]); i++)
    {
        rtnArr[i] = aArr[i][0];
    }

    return rtnArr;
}

double sumVecPow(vector<double> dataVec, int powOrder)
{
    double sum = 0.0;
    for (int i = 0; i < dataVec.size(); i++)
    {
        sum = sum + pow(dataVec[i], powOrder);
    }

    return sum;
}
double sumVecPowXY(vector<double> dataVecX, int powOrderX, vector<double> dataVecY, int powOrderY)
{
    double sum = 0.0;
    for (int i = 0; i < dataVecX.size(); i++)
    {
        sum = sum + pow(dataVecX[i], powOrderX) * pow(dataVecY[i], powOrderY);
    }

    return sum;
}

void makeZero_UnderDiagonalElement(double xArr[][3], double yArr[][1])
{
    /* 
    input/output : xArr, yArr
    
    Just for 2nd-order polynomial.

    */

    double r;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (i > j)
            {
                if (j == 0)
                {
                    // ?????? 0 ?????? ??????(j=0) -> ?????? ????????? 0?????? ??????.
                    r = xArr[i][j] / xArr[j][j];
                    xArr[i][j] = 0;
                    xArr[i][j + 1] = xArr[i][j + 1] - r * (xArr[i - 1][j + 1]);
                    xArr[i][j + 2] = xArr[i][j + 2] - r * (xArr[0][2]);
                    yArr[i][0] = yArr[i][0] - r * (yArr[0][0]);
                }
                if (j == 1)
                {
                    // ?????? 1 ?????? ?????? -> ?????? ????????? 0?????? ??????.
                    r = xArr[i][j] / xArr[j][j];
                    xArr[i][j] = 0;
                    xArr[i][j + 1] = xArr[i][j + 1] - r * (xArr[i - 1][j + 1]);
                    yArr[i][0] = yArr[i][0] - r * (yArr[1][0]);
                }
            }
        }
    }
}

void initGaussianMatrix(int polyOrder,
                        vector<double> *xCoord,
                        vector<double> *yCoord,
                        double xArr[][3],
                        double yArr[][1])
{
    /*
    INPUT   : xCoord, yCoord
    OUTPUT  : xArr, yArr


    Gaussian Elimination foramt.
    -> X*A = Y

    X,Y is data.
    Target is A.

    Actually xArr[][3] is not good for nomarlization form.
        
    */

    // row init.
    for (int i = 0; i < polyOrder + 1; i++)
    {
        for (int j = 0; j < polyOrder + 1; j++)
        {
            xArr[i][j] = sumVecPow(*xCoord, i + j);
        }
    }

    // Y column init.
    yArr[0][0] = sumVecPow(*yCoord, 1);
    for (int i = 1; i < polyOrder + 1; i++)
    {
        yArr[i][0] = sumVecPowXY(*xCoord, i, *yCoord, 1);
    }
}

void calcCoeffsValue(double xArr[][3], double yArr[][1], double aArr[][1])
{
    /*
    INPUT   : xArr, yArr that is getting from gaussian elimination.
    OUTPUT  : aArr (Coefficients array for 2nd-order polynomial equation.) 
        
    */

    aArr[2][0] = yArr[2][0] / xArr[2][2];
    aArr[1][0] = (yArr[1][0] - xArr[1][2] * aArr[2][0]) / xArr[1][1];
    aArr[0][0] = (yArr[0][0] - xArr[0][2] * aArr[2][0] - xArr[0][1] * aArr[1][0]) / xArr[0][0];
}

int calcPoly(double xIn, vector<double> polyCoeffs)
{
    return (int)round(polyCoeffs[0] + polyCoeffs[1] * xIn + polyCoeffs[2] * pow(xIn, 2));
}

void makeOneDirectionArray(
    vector<vector<Point>> pointContainer,
    int i_side,
    vector<double> *inCoord,
    vector<double> *outCoord)
{
    vector<Point> ptsContainer = pointContainer[i_side];

    // make each x and y array from point container.
    for (int i = 0; i < ptsContainer.size(); i++)
    {
        inCoord->push_back(ptsContainer[i].y);
        outCoord->push_back(ptsContainer[i].x);
    }
}

void calcLaneRadiusAndCenter(Mat img,
                             vector<vector<Point>> pixelPosXY,
                             double *leftRadius,
                             double *rightRadius,
                             double *centerOffset)
{
    // objective for centerOffset : ??? ????????? ????????? ????????? ????????? ????????? ??????????????? ???????????? ?????? ?????????
    // r??? ????????? ??? ?????? : https://github.com/SanhoLee/advanced-lane-finding/blob/main/resources/radius-of-curvature.png

    /* ???????????????, ?????? ???????????? ?????? ????????? ???????????? ?????? ??? ??????. ???????
     meters per pixel, lane line  : 10ft = 3.048 meters (img height in realworld?)
     meters per pixel, lane width : 12ft = 3.7 meters (lane width in realworld?)
    */

    // unit conversion from pixel to meter.
    double ymPerPix = 3.048 / 100; // cm?
    double xmPerPix = 3.7 / 378;   // ? 378??? ?????????  ????????????.

    vector<double> inCoordLeft;
    vector<double> outCoordLeft;
    vector<double> coeffLeft;

    vector<double> inCoordRight;
    vector<double> outCoordRight;
    vector<double> coeffRight;

    // multiply unit conversion factor.
    for (int i = 0; i < pixelPosXY[LEFT_LANE].size(); i++)
    {
        inCoordLeft.push_back(pixelPosXY[LEFT_LANE][i].y * ymPerPix);
        outCoordLeft.push_back(pixelPosXY[LEFT_LANE][i].x * xmPerPix);
    }

    for (int i = 0; i < pixelPosXY[RIGHT_LANE].size(); i++)
    {
        inCoordRight.push_back(pixelPosXY[RIGHT_LANE][i].y * ymPerPix);
        outCoordRight.push_back(pixelPosXY[RIGHT_LANE][i].x * xmPerPix);
    }

    // get polyfit coeffs with unit converted data.
    coeffLeft = polyFit_cpp(inCoordLeft, outCoordLeft, 2);
    coeffRight = polyFit_cpp(inCoordRight, outCoordRight, 2);

    // define calculating position for y direction.(bottom of img)
    int imgRows = img.rows;
    double yEval = (imgRows - 1) * ymPerPix;

    // calculating curvature radius
    double up = 0;
    double down = 0;

    up = pow(1 + pow((2 * coeffLeft[2] * yEval + coeffLeft[1]), 2), 1.5);
    down = fabs(2 * coeffLeft[2]);
    *leftRadius = up / down;

    up = pow(1 + pow((2 * coeffRight[2] * yEval + coeffRight[1]), 2), 1.5);
    down = fabs(2 * coeffRight[2]);
    *rightRadius = up / down;

    // calculating Center offset with car center position(img center = car center).
    double h = img.cols * xmPerPix;
    double carCenPos = (img.cols / 2) * xmPerPix;
    double leftInit = coeffLeft[0] + coeffLeft[1] * h + coeffLeft[2] * pow(h, 2);
    double rightInit = coeffRight[0] + coeffRight[1] * h + coeffRight[2] * pow(h, 2);
    double laneCenter = (leftInit + rightInit) / 2;

    *centerOffset = carCenPos - laneCenter;
    // centerOffset > 0 (+)  : car is on right side of lane.
    // centerOffset = 0      : car is on center of lane.
    // centerOffset < 0 (-)  : car is on left side of lane.
}