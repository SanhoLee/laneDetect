#include <iostream>
#include "common.hpp"
#include "CV_header.hpp"
#include "struct_drawData.h"
#include "preprocImg.hpp"
#include "calcLaneImg.hpp"
#include "drawOnWarpImg.hpp"

int main(int argc, char **argv)
{

    Mat img, imgBinary, imgShow;
    Mat invMatx;

    drawDataInfo drawDataSet;

    // img = imread("data/straight_lines1.jpg");
    img = imread("data/straight_lines2.jpg");
    if (img.empty())
    {
        cout << " ERROR :: IMG READ FAILED." << endl;
        return -1;
    }

    imgBinary = preprocImg(img, &invMatx);
    drawDataSet = calcImg(imgBinary);
    imgShow = drawAll(img, imgBinary, invMatx, drawDataSet);

    imshow("IMG SHOW", imgShow);
    waitKey(0);

    // Mat outImg = drawLane(img);
    // drawOnWarpImg(imgBinary);
    // drawPolygonAndFill(imgBinary);

    return 0;
}