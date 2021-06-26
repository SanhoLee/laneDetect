#include <iostream>
#include "calcLaneImg.hpp"
#include "common.hpp"

// minimum pixel for left and right window search.
int minNumPix = 40;

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

    /* 1. get half down space. */
    Mat halfdown = halfDownImg(imgCombined);

    /* 2. find sum value from each column pixel data. */
    vector<int> sumArray = sumColElm(halfdown);

    /* 3. get left and right X coordinate for base position. */
    int leftX_base = getLeftX_base(sumArray);
    int rightX_base = getRightX_base(sumArray);

    /* 4. window search squre data */
    int numWindow = 10;
    int window_height = imgCombined.rows / numWindow;
    Mat nonZeroPos;
    findNonZero(imgCombined, nonZeroPos);
    // -> nonZeroPos has non-zero pixel location info.

    // container for each side window pixel
    vector<vector<int>> leftLanePixelContainer;
    vector<vector<int>> rightLanePixelContainer;

    // create window for detecting line.
    // define variables...
    int win_yHigh;
    int win_yLow;

    int win_xLeft_low;
    int win_xRight_low;

    int margin = 80;
    int leftX_current = leftX_base;
    int rightX_current = rightX_base;

    cout << "full width : " << imgCombined.cols << endl;
    cout << "full height : " << imgCombined.rows << endl;
    cout << "leftXbase : " << leftX_base << endl;
    cout << "rightXbase : " << rightX_base << endl;

    // define each window box...
    // save on Vector variable.
    vector<vector<Rect>> rectWindowInfo;
    for (int i = 0; i < numWindow; i++)
    {
        win_yHigh = imgCombined.rows - (i)*window_height;
        win_yLow = imgCombined.rows - (i + 1) * window_height;
        win_xLeft_low = leftX_current - margin;
        win_xRight_low = rightX_current - margin;

        // define current window by Rect class.
        // syntax --> Rect(x,y,width, height)
        // funtion 1.
        // (make array.)1. Rect object info for specific window.
        Rect curLeftWindow(win_xLeft_low, win_yLow, 2 * margin, window_height);
        Rect curRightWindow(win_xRight_low, win_yLow, 2 * margin, window_height);
        rectWindowInfo.push_back({curLeftWindow, curRightWindow});

        // get index for non-zero elements on each side of windows.
        // funtion 2.
        // (make array.)2. found pixel index element for specific window.
        vector<int>
            leftWindowPoint_IDX_X = getIndexArray_onWindow(nonZeroPos, curLeftWindow);
        vector<int> rightWindowPoint_IDX_X = getIndexArray_onWindow(nonZeroPos, curRightWindow);
        leftLanePixelContainer.push_back(leftWindowPoint_IDX_X);
        rightLanePixelContainer.push_back(rightWindowPoint_IDX_X);

        /* Updating if found the number of window pixel is bigger than min pixel, 
        re-center next window pos.    
        updating element : leftX_current, rightX_current
        */
        reCenterCurrentPos(leftWindowPoint_IDX_X, &leftX_current);
        reCenterCurrentPos(rightWindowPoint_IDX_X, &rightX_current);
    }

    Mat test(imgCombined.rows, imgCombined.cols, CV_8UC3, Scalar(255, 255, 255));
    for (int i = 0; i < numWindow; i++)
    {
        // left window
        rectangle(test, rectWindowInfo[i][0], Scalar(0, 0, 255), 5, LINE_AA);
        // right window
        rectangle(test, rectWindowInfo[i][1], Scalar(0, 0, 255), 5, LINE_AA);
    }

    imshow("test", test);
    waitKey(0);

    // return array --> rectWindowInfo, leftLanePixelContainer, rightLanePixelContainer

    /* 5. coefficient for the 2nd polynomial equation */
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

int mean_vectorArray(vector<int> target_vector)
{
    int sum = 0;
    int mean = 0;

    // 1. sum
    for (int i = 0; i < target_vector.size(); i++)
    {
        sum = sum + target_vector[i];
    }

    // 2. divide by the size of vector.
    mean = (int)sum / target_vector.size();

    return mean;
}

vector<int> getIndexArray_onWindow(Mat nonZeroPos, Rect windowBox)
{
    // commonly use for left and right side winodw.
    vector<int> windowPoint_IDX_X;

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
            windowPoint_IDX_X.push_back(onePt.x);
        }
    }

    return windowPoint_IDX_X;
}

void reCenterCurrentPos(vector<int> pntXIndexArray, int *currentXPos)
{
    /*
    parameter:
        INPUT
            pntXIndexArray  : index array for x-coordinate from non-zero pixel array.
        I/O
            currentXPos     : window center position for x axis.
    */

    // check the length of pointIndexArray is bigger than minNumPix or not.
    if (pntXIndexArray.size() > minNumPix)
    {
        // re-center window center x coordinates with mean values for the array.
        *currentXPos = mean_vectorArray(pntXIndexArray);
    }
    else
    {
        // no need to process
    }
}