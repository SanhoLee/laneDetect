#include <iostream>
#include "calcLaneImg.hpp"

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
    int window_height = imgCombined.rows / 10;
    Mat nonZeroPos;
    findNonZero(imgCombined, nonZeroPos);
    // -> nonZeroPos has non-zero pixel location info.

    // create window for detecting line.
    // define variables...
    int win_yHigh;
    int win_yLow;

    int win_xLeft_low;
    int win_xLeft_high;

    int win_xRight_low;
    int win_xRight_high;

    int margin = 80;
    int leftX_current = leftX_base;
    int rightX_current = rightX_base;

    // define each window box...
    // 여기서 high, low는 축방향 크고 작음이 아니라, 픽셀 인덱스 값의 크고 작음을 의미한다. 그렇기 때문에 y high 이면 화면상 아래 방향일수록 high 이며, x high 일 경우는 화면상에서 오른쪽에 있을 수록 high 값을 나타낸다.

    win_yHigh = imgCombined.rows - (0) * window_height;
    win_yLow = imgCombined.rows - (0 + 1) * window_height;

    win_xLeft_low = leftX_current - margin;
    win_xLeft_high = leftX_current + margin;
    win_xRight_low = rightX_current - margin;
    win_xRight_high = rightX_current + margin;

    // 변수에 할당 후 프린트 하면, 정상적으로 값이 출력된다.
    int temp = imgCombined.at<uint8_t>(nonZeroPos.at<Point>(10910));
    cout << temp << endl;

    // size : colSize x rowSize [1 x 100xx]

    vector<int> leftWindowPoint_X_idx;
    vector<int> leftWindowPoint_Y_idx;

    for (int i = 0; i < nonZeroPos.rows; i++)
    {
        int nonZeroX = nonZeroPos.at<Point>(i).x;
        int nonZeroY = nonZeroPos.at<Point>(i).y;
        Point onePt = {0, 0};
        onePt = nonZeroPos.at<Point>(i);

        // When non-zero position is on spcified Window Area.
        if (nonZeroX >= win_xLeft_low &&
            nonZeroX < win_xLeft_high &&
            nonZeroY >= win_yLow &&
            nonZeroY < win_yHigh)
        {
            leftWindowPoint_X_idx.push_back(onePt.x);
            leftWindowPoint_Y_idx.push_back(onePt.y);
        }
    }

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