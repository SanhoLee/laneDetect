#include <iostream>
#include "CV_header.hpp"
#include "preprocImg.hpp"
#include "calcLaneImg.hpp"
#include "drawOnWarpImg.hpp"

int main(int argc, char **argv)
{

    Mat img, imgCombined;
    Mat invMatx;

    vector<vector<double>> coeffsLR;
    vector<vector<Rect>> rectWindowInfo;
    vector<vector<Point>> pixelPosXY;

    /* 
    pipeline     :: preprocessing img.
    Combined img :: best img after preprocessing from unwarping to color channel combinineg.
     */

    // img = imread("data/straight_lines1.jpg");
    img = imread("data/straight_lines2.jpg");
    if (img.empty())
    {
        cout << " ERROR :: IMG READ FAILED." << endl;
        return -1;
    }

    imgCombined = preprocImg(img, &invMatx);
    calcLaneImg(imgCombined, &rectWindowInfo, &pixelPosXY, &coeffsLR);

    cout << "coeffsLR size : " << coeffsLR.size() << endl;
    cout << "pixelPosXY size : " << pixelPosXY.size() << endl;
    cout << "rectWindowInfo size : " << rectWindowInfo.size() << endl;

    // drawOnWarpImg(imgCombined, coeffsLR);

    // on later, draw lane on original img...

    return 0;
}