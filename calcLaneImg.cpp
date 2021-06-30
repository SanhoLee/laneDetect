#include <iostream>
#include <numeric>
#include "calcLaneImg.hpp"
#include "common.hpp"

// minimum pixel for left and right window search.
int minNumPix = 40;
int numWindow = 10;

vector<Point2f> calcLaneImg(Mat imgCombined)
{
    /*  
        description :
            Do Window Search.
            find 2nd Polynomial Coefficient and get x, y pixel for detected lanes.
        
        return  : 
            Detected lane Boundary Points for Array.
            (It is calculated by 2nd polynomial equation.)
    */
    vector<Point2f> laneBoundary_coord;

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

    vector<double> xCoord = {0.0, 3.0, 5.0, 7.0, 10.0};
    vector<double> yCoord = {10, 11.6, 24.1, 55.7, 103.9};

    vector<double> polyCoeffs;

    polyCoeffs = polyFit_cpp(xCoord, yCoord, 2);

    /* 6. array for x, y direction. */

    // squreInfo(vector) : Including left and right Rectange Info. structure? just vector?
    // It will be used for window searching
    // return all squre info for array type of it.
    // squareInfo = findLinePos(imgCombined);

    // determining each side curvature for 2nd polynomial equation.
    // return the coefficient for the 2nd polynomial equation.
    // polyCoeffs_left = determineCurvature(squreInfo_left);
    // polyCoeffs_right = determineCurvature(squreInfo_right);

    // x_left = Mat();
    // x_right = Mat();

    // y_left = calcPolyNomial(2, polyCoeffs_left);
    // y_right = calcPolyNomial(2, polyCoeffs_left);

    // laneBoundary_coord = {};

    return laneBoundary_coord;
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

    Mat test(preprocess.rows, preprocess.cols, CV_8UC3, Scalar(255, 255, 255));

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

     first. make funtion for 2nd-order polynomial equation.
     second. normalizing function.

     */

    // initializing a matrix.
    vector<double> a_matrix;
    double a0 = 0.0, a1 = 0.0, a2 = 0.0;

    // Calculating row 1
    double N = (double)xCoord.size();
    double sigX = sumVec(xCoord);
    double sigX2 = sumVecPow(xCoord, 2);

    // Calculating row 2
    // sigX;
    // sigX2;
    double sigX3 = sumVecPow(xCoord, 3);

    // Calculating row 3
    // sigX2;
    // sigX3;
    double sigX4 = sumVecPow(xCoord, 4);

    // Calculating right column 1
    double sigY = sumVec(yCoord);
    double sigXsigY = sumVecPowXY(xCoord, 1, yCoord, 1);
    double sigX2sigY = sumVecPowXY(xCoord, 2, yCoord, 1);

    // temp return.
    return xCoord;
}

double sumVec(vector<double> dataVec)
{
    double sum = 0.0;
    for (int i = 0; i < dataVec.size(); i++)
    {
        sum = sum + dataVec[i];
    }

    return sum;
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