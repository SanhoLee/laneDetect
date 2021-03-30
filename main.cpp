#include <iostream>
#include <typeinfo>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

Mat img, imgEdge;
String IMG_FILE = "data/cac07407-196cd6f8.jpg";
String SAVE_FILE_BASE = "data/output/";

int isEmptyImg(Mat img)
{
    if (img.empty())
    {
        cout << " FAILED::IMG LOADING " << endl;
        return -1;
    }
    else
    {
        cout << " SUCCESS::IMG LOADING " << endl;
        return 0;
    }
}

Mat preProcessing(Mat img)
{
    Mat imgGray, imgBlur, imgDil;
    Mat imgCanny1;
    int kernelSize = 3;

    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    imwrite(SAVE_FILE_BASE + "imgGray.png", imgGray);

    GaussianBlur(imgGray, imgBlur, Size(kernelSize, kernelSize), kernelSize, kernelSize);
    imwrite(SAVE_FILE_BASE + "imgBlur.png", imgBlur);

    Canny(imgBlur, imgCanny1, 40, 60);
    imwrite(SAVE_FILE_BASE + "imgCanny1.png", imgCanny1);

    imshow("imgCanny", imgCanny1);

    // Mat kernel = getStructuringElement(MORPH_RECT, Size(kernelSize - 5, kernelSize - 5));
    // dilate(imgCanny1, imgDil, kernel);

    return imgCanny1;
}

int main()
{
    img = imread(IMG_FILE);
    isEmptyImg(img);

    // 1. preprocessing img.
    imgEdge = preProcessing(img);

    // // 2. Hough line Transform
    // // float 값 두개를 가지는 lines 벡터 변수 선언.
    // vector<Vec2f> lines;

    // // 이 값 설정으로 근처에 있는 서로 다른 직선을 구별한다 ?
    // double rho_thres = 0.2;
    // double theta_thres = (CV_PI / 180);
    // int ptr_votes_thres = 20;

    // HoughLines(imgEdge, lines, rho_thres, theta_thres, ptr_votes_thres, 0, 0);

    // cout << "lines : " << lines.size() << endl;

    // for (size_t i = 0; i < lines.size(); i++)
    // {
    //     float rho = lines[i][0], theta = lines[i][1];
    //     Point pt1, pt2;
    //     double a = cos(theta), b = sin(theta);
    //     double x0 = a * rho, y0 = b * rho;
    //     pt1.x = cvRound(x0 + 1000 * (-b));
    //     pt1.y = cvRound(y0 + 1000 * (a));
    //     pt2.x = cvRound(x0 - 1000 * (-b));
    //     pt2.y = cvRound(y0 - 1000 * (a));
    //     line(img, pt1, pt2, Scalar(0, 0, 255), 3, LINE_AA);
    //     cout << "line " << i + 1 << "->> rho : " << lines[i][0] << "px, \t"
    //          << " theta : " << lines[i][1] * (180 / CV_PI) << endl;
    // }

    imshow("img", img);
    waitKey(0);

    return 0;
}