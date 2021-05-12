#include <iostream>
#include "gloabalVal.hpp"
#include "preprocImg.hpp"

int main(int argc, char **argv)
{

    Mat img, imgCombined;
    Mat invMatx;

    /* 
    pipeline     :: preprocessing img.
    Combined img :: best img after preprocessing from unwarping to color channel combinineg.
     */

    imgCombined = preprocImg(img, &invMatx);

    return 0;
}