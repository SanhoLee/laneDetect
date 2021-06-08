#include <iostream>
#include "CV_header.hpp"
#include "preprocImg.hpp"

int main(int argc, char **argv)
{

    Mat img, imgCombined;
    Mat invMatx;

    /* 
    pipeline     :: preprocessing img.
    Combined img :: best img after preprocessing from unwarping to color channel combinineg.
     */

    // img = imread("data/straight_lines1.jpg");
    img = imread("data/straight_lines2.jpg");
    // img = imread("data/bdd_data/b1d7b3ac-344d229e.jpg");
    if (img.empty())
    {
        cout << " ERROR :: IMG READ FAILED." << endl;
        return -1;
    }

    imgCombined = preprocImg(img, &invMatx);

    return 0;
}