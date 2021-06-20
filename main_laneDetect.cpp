#include <iostream>
#include "CV_header.hpp"
#include "preprocImg.hpp"
#include "calcLaneImg.hpp"

int main(int argc, char **argv)
{

    Mat img, imgCombined;
    Mat invMatx;

    vector<Point2f> roadBoundaryPtsArr;

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
    roadBoundaryPtsArr = calcLaneImg(imgCombined);
    // drawLane(roadBoundaryPtsArr);

    imshow("imgConbined", imgCombined * 255);
    waitKey(0);

    return 0;
}