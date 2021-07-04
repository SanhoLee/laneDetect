#include <iostream>
#include <numeric>
#include "calcLaneImg.hpp"
#include "common.hpp"

// minimum pixel for left and right window search.
int minNumPix = 40;
int numWindow = 10;

vector<vector<double>> calcLaneImg(Mat imgCombined)
{
    /*  
        description :
            Do Window Search.
            find 2nd Polynomial Coefficient and get x, y pixel for detected lanes.
        
        return  : 
            Detected lane Boundary Points for Array.
            (It is calculated by 2nd polynomial equation.)
    */

    // rectangle window info for each side of lane.
    // container for each side window pixel
    vector<vector<Rect>> rectWindowInfo;
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

    // (todos..)From non-zero Mat, get x and y coordinate for preprocessed pixel position.

    vector<double> xCoordLeft;
    vector<double> yCoordLeft;
    vector<double> xCoordRight;
    vector<double> yCoordRight;

    vector<double> polyCoeffsLeft;
    vector<double> polyCoeffsRight;

    // make each x and y array from point data.
    for (int i = 0; i < leftPixelPosXY_OneDim.size(); i++)
    {
        xCoordLeft.push_back(leftPixelPosXY_OneDim[i].x);
        yCoordLeft.push_back(leftPixelPosXY_OneDim[i].y);
    }

    for (int i = 0; i < rightPixelPosXY_OneDim.size(); i++)
    {
        xCoordRight.push_back(rightPixelPosXY_OneDim[i].x);
        yCoordRight.push_back(rightPixelPosXY_OneDim[i].y);
    }

    polyCoeffsLeft = polyFit_cpp(xCoordLeft, yCoordLeft, 2);
    polyCoeffsRight = polyFit_cpp(xCoordRight, yCoordRight, 2);

    // 또는 그냥 다항식 계수 리턴?
    vector<vector<double>> coeffsLR = {polyCoeffsLeft, polyCoeffsRight};

    return coeffsLR;
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
                  vector<vector<Rect>> *rectWindowInfo,
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
        rectWindowInfo->push_back({curLeftWindow, curRightWindow});

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

    // Mat test(preprocess.rows, preprocess.cols, CV_8UC3, Scalar(255, 255, 255));

    // for (int i = 0; i < numWindow; i++)
    // {
    //     rectangle(test, rectWindowInfo[0][i][0], Scalar(255, 0, 0), 3, LINE_AA);
    //     rectangle(test, rectWindowInfo[0][i][1], Scalar(0, 0, 255), 3, LINE_AA);
    // }

    // imshow("test", test);
    // waitKey(0);
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

    // 2. gaussian elimination. 대각 행렬 요소 왼쪽 아래 요소를 모두 0으로 만들어주는 작업.
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
                    // 컬럼 0 번째 일때(j=0) -> 해당 요소를 0으로 만듬.
                    r = xArr[i][j] / xArr[j][j];
                    xArr[i][j] = 0;
                    xArr[i][j + 1] = xArr[i][j + 1] - r * (xArr[i - 1][j + 1]);
                    xArr[i][j + 2] = xArr[i][j + 2] - r * (xArr[0][2]);
                    yArr[i][0] = yArr[i][0] - r * (yArr[0][0]);
                }
                if (j == 1)
                {
                    // 컬럼 1 번째 일때 -> 해당 요소를 0으로 만듬.
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