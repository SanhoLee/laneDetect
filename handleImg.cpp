#include <iostream>
#include "gloabalVal.hpp"
#include "CV_header.hpp"
#include "handleImg.hpp"

using namespace cv;
using namespace std;

// unit change format
double to_radian = (CV_PI / 180);
double to_degree = (180 / CV_PI);

void drawHoughLines(Mat img, vector<Vec2f> lines, int leftSide_Angle, int rightSide_Angle)
{
    for (int i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
        double temp1 = double(90 - leftSide_Angle) * to_radian;
        double temp2 = double(90 + rightSide_Angle) * to_radian;
        if (theta < temp1 && theta > 0 || theta < CV_PI && theta > temp2)
        {
            cout << "criteria angle 1 : " << temp1 * to_degree
                 << "\t criteria angle 2 : " << temp2 * to_degree << endl;

            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            pt1.x = cvRound(x0 + 1000 * (-b));
            pt1.y = cvRound(y0 + 1000 * (a));
            pt2.x = cvRound(x0 - 1000 * (-b));
            pt2.y = cvRound(y0 - 1000 * (a));
            line(img, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
        }
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
