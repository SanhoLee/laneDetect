#include <iostream>
#include "gloabalVal.hpp"
#include "CV_header.hpp"
#include "handleImg.hpp"

using namespace cv;
using namespace std;

void drawHoughLines(Mat img, vector<Vec2f> lines, int leftSide_Angle, int rightSide_Angle)
{
    for (int i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
        // double temp1 = double(90 - leftSide_Angle) * to_radian;
        double temp1 = double(90 - leftSide_Angle) * toRadian();
        double temp2 = double(90 + rightSide_Angle) * toRadian();

        double verticalPosY = img.size().height;
        if (theta < temp1 && theta > 0 || theta < CV_PI && theta > temp2)
        // {
        //     cout << "criteria angle 1 : " << temp1 * toDegree()
        //          << "\t criteria angle 2 : " << temp2 * toDegree() << endl;

        //     Point pt1, pt2;
        //     double a = cos(theta), b = sin(theta);
        //     double x0 = a * rho, y0 = b * rho;
        //     pt1.x = cvRound(x0 + 1000 * (-b));
        //     pt1.y = cvRound(y0 + 1000 * (a));
        //     pt2.x = cvRound(x0 - 1000 * (-b));
        //     pt2.y = cvRound(y0 - 1000 * (a));
        //     line(img, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
        // }
        {
            cout << "criteria angle 1 : " << temp1 * toDegree()
                 << "\t criteria angle 2 : " << temp2 * toDegree() << endl;

            Point pt1, pt2;
            double a = cos(theta), b = sin(theta);
            double x0 = a * rho, y0 = b * rho;
            double len = 0;

            // pt1.y = cvRound(y0 + 1000 * (a));
            // find x position with fixed vertical position.
            pt1.x = 0;
            pt1.y = verticalPosY / 2;

            if (y0 < pt1.y)
            {
                len = (pt1.y - b) / a;
                pt1.x = cvRound(x0 + len * (-a));
            }

            pt2.x = cvRound(x0 - 1000 * (-b));
            pt2.y = cvRound(y0 - 1000 * (a));
            // pt2.x = 500;
            // pt2.y = 500;
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
