#include <iostream>
#include "gloabalVal.hpp"
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

        if (pt1.y > imgHeight * (1 - topPos))
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
    int ptr_votes_thres,
    int leftSide_Angle,
    int rightSide_Angle,
    double rho_thres,
    double theta_degree,
    int yFixed,
    double minLineLen,
    double maxGap)
{
    cout << "votes : " << ptr_votes_thres
         << "\t left Angle :" << leftSide_Angle
         << "\t right Angle :" << rightSide_Angle
         << "\t Rho : " << rho_thres
         << "\t theta(degree) : " << theta_degree
         << "\t yFixed : " << yFixed
         << "\t minLineLen : " << minLineLen
         << "\t maxGap : " << maxGap
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

Mat preProcessing(Mat img)
{
    Mat imgGray, imgBlur, imgDil;
    Mat imgCanny1;
    int kernelSize = 3;

    int lowThres = 100;
    int highThres = 120;

    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    GaussianBlur(imgGray, imgBlur, Size(kernelSize, kernelSize), kernelSize, kernelSize);
    Canny(imgBlur, imgCanny1, lowThres, highThres);

    return imgCanny1;
}
