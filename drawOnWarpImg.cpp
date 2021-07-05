#include <iostream>

#include "common.hpp"
#include "drawOnWarpImg.hpp"

void drawOnWarpImg(Mat imgBinary, vector<vector<double>> coeffsLR)
{
    cout << "drawOnWarpImg function :: " << endl;

    Mat dstImg;

    Mat channel_B(3, 4, CV_8UC1, 100);
    Mat channel_G(3, 4, CV_8UC1, 50);
    Mat channel_R(3, 4, CV_8UC1, 30);

    vector<Mat> channels = {channel_B, channel_G, channel_R};
    merge(channels, dstImg);

    cout << "dst Img : " << endl;
    cout << dstImg << endl;
    // make uint 8U img.
    // give pixel value(255,255,255) to each index of mat.
}