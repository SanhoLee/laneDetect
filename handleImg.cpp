#include <iostream>
#include "CV_header.hpp"
#include "handleImg.hpp"

using namespace cv;
using namespace std;

vector<Vec4i> filterBtmCtrPt(
    Mat img,
    vector<Vec4i> linePoints,
    double topPos,
    double leftPos,
    double rightPos)
{
    vector<Vec4i> linePointsRtn(linePoints.size());

    int imgHeight = img.size().height;
    int imgWidth = img.size().width;
    int cnt = 0;

    Point pt1;
    Point pt2;

    for (int i = 0; i < linePoints.size(); i++)
    {
        // Point 데이터 할당
        pt1.x = linePoints[i][0];
        pt1.y = linePoints[i][1];
        pt2.x = linePoints[i][2];
        pt2.y = linePoints[i][3];

        // top, left, right Boundary ...
        if (pt1.y > imgHeight * (1 - topPos) &&
            pt1.x > imgWidth * leftPos &&
            pt1.x < imgWidth * (1 - rightPos))
        {
            linePointsRtn[cnt][0] = pt1.x;
            linePointsRtn[cnt][1] = pt1.y;
            linePointsRtn[cnt][2] = pt2.x;
            linePointsRtn[cnt][3] = pt2.y;
            cnt++;
        }
    }
    return linePointsRtn;
}

void showCurValue(
    int hmin,
    int smin,
    int lmin,
    int hmax,
    int smax,
    int lmax,
    int lowT,
    int highT)
{
    cout << " hmin : " << hmin
         << "\t smin : " << smin
         << "\t lmin : " << lmin
         << "\t hmax : " << hmax
         << "\t smax : " << smax
         << "\t lmax : " << lmax
         << "\t lowT : " << lowT
         << "\t highT : " << highT

         << endl;
}

void drawHoughLines(Mat img, vector<Vec2f> lines, int leftSide_Angle, int rightSide_Angle, int yFixed)
{
    for (int i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];

        // angle criteria.
        double temp1 = double(90 - leftSide_Angle) * toRadian();
        double temp2 = double(90 + rightSide_Angle) * toRadian();

        // setting found pixel data.
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;

        // setting line Length from yFixed Position.
        double lineLen = (y0 - yFixed) / a;

        if (theta < temp1 && theta > 0)
        // rightside line...
        {
            pt1.x = x0 + lineLen * b;
            pt1.y = yFixed;
            pt2.x = cvRound(x0 + 10000 * (-b));
            pt2.y = cvRound(y0 + 10000 * (a));
        }
        else if (theta < CV_PI && theta > temp2)
        // leftside line...
        {
            pt1.x = x0 + lineLen * b;
            pt1.y = yFixed;
            pt2.x = cvRound(x0 - 10000 * (-b));
            pt2.y = cvRound(y0 - 10000 * (a));
        }
        // draw a line.
        line(img, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
    }
}

void drawLinesWithPoint(Mat img, vector<Vec4i> linePoints)
{
    for (int i = 0; i < linePoints.size(); i++)
    {
        line(
            img,
            Point(linePoints[i][0], linePoints[i][1]),
            Point(linePoints[i][2], linePoints[i][3]),
            Scalar(0, 0, 255),
            3,
            LINE_AA);
    }
}

int isEmptyImg(Mat img)
{
    if (img.empty())
    {
        cout << " FAILED::IMG LOADING " << endl;
        return -1;
    }
    else
    {
        // cout << " SUCCESS::IMG LOADING " << endl;
        return 0;
    }
}

Mat maskingImg(Mat img, Scalar lower, Scalar upper)
{
    /*
        .convert to HSV color space
        .masking with inRange Function.
    */

    Mat imgHSV, imgHLS;
    Mat maskImg;

    int lowThres = 100;
    int highThres = 120;

    cvtColor(img, imgHSV, COLOR_BGR2HLS);
    inRange(imgHSV, lower, upper, maskImg);

    return maskImg;
}

Mat ImgROI(Mat imgCanny, int yFixed_unit, int leftPnt_Pos_unit, int rightPnt_Pos_unit)
{
    /*
    leftPnt_Pos, rightPnt_Pos : Ratio from width center line.
    yFixed : Position on Vertical direction.(Ratio expression. 0 to 1.)
    */
    Point leftPoint;
    Point rightPoint;

    double yFixed = yFixed_unit * 0.01;
    double leftPnt_Pos = leftPnt_Pos_unit * 0.01;
    double rightPnt_Pos = rightPnt_Pos_unit * 0.01;

    int HEIGHT_ = imgCanny.size().height;
    int WIDTH_ = imgCanny.size().width;
    // create zero pixel value img.
    Mat zeros = Mat::zeros(Size(WIDTH_, HEIGHT_), CV_8UC1);
    Mat imgFiltered;

    // ROI Point vector
    vector<vector<Point>> ROI_point(1, vector<Point>(6));
    float ratio_y1 = 0.2;
    float ratio_y2 = 0.4;

    // polygon point setting.
    ROI_point[0][0] = Point(0, HEIGHT_);
    ROI_point[0][1] = Point(0, HEIGHT_ * (1 - ratio_y1));
    ROI_point[0][2] = Point((WIDTH_ / 2) * (1 - leftPnt_Pos), HEIGHT_ * (1 - ratio_y2));
    ROI_point[0][3] = Point((WIDTH_ / 2) * (1 + rightPnt_Pos), HEIGHT_ * (1 - ratio_y2));
    ROI_point[0][4] = Point(WIDTH_, HEIGHT_ * (1 - ratio_y1));
    ROI_point[0][5] = Point(WIDTH_, HEIGHT_);

    /* 
    Do fillPoly and bitwise_and process.
    white * black pixel --> 0
    white * white pixel --> 1 (printed on the img.) 
    */
    // make polygon white pixel.
    fillPoly(zeros, {ROI_point[0]}, Scalar(255, 255, 255), LINE_AA);
    // calculating bit multiply.
    bitwise_and(imgCanny, zeros, imgFiltered);

    return imgFiltered;
}