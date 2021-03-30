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

int rtnKey = 0;
int lowThres = 100;
int highThres = 120;

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

    cvtColor(img, imgGray, COLOR_BGR2GRAY);
    // imwrite(SAVE_FILE_BASE + "imgGray.png", imgGray);

    GaussianBlur(imgGray, imgBlur, Size(kernelSize, kernelSize), kernelSize, kernelSize);
    // imwrite(SAVE_FILE_BASE + "imgBlur.png", imgBlur);

    Canny(imgBlur, imgCanny1, lowThres, highThres);
    // imwrite(SAVE_FILE_BASE + "imgCanny1.png", imgCanny1);
    // cout << "low, high :: " << lowThres << "\t" << highThres << endl;

    // imshow("imgCanny", imgCanny1);

    // Mat kernel = getStructuringElement(MORPH_RECT, Size(kernelSize - 5, kernelSize - 5));
    // dilate(imgCanny1, imgDil, kernel);

    return imgCanny1;
}

void drawHoughLines(vector<Vec2f> lines)
{
    for (int i = 0; i < lines.size(); i++)
    {
        float rho = lines[i][0], theta = lines[i][1];
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

int main()
{
    // 이 값 설정으로 근처에 있는 서로 다른 직선을 구별한다.

    // Initialize rho and theta
    double rho_thres = 0;
    double theta_radian = 0;
    double theta_degree = 0;
    int ptr_votes_thres = 100;

    // unit change format
    double to_radian = (CV_PI / 180);
    double to_degree = 1 / to_radian;

    /*
    1 unit scale transform...
    rho 1 unit -> 0.25 px
    theta 1 unit -> 0.5 degree
    */
    int rho_unit = 1;
    int theta_unit = 1;

    namedWindow("Trackbar", WINDOW_NORMAL);
    resizeWindow("Trackbar", Size(700, 100));

    createTrackbar("HLine :: Rho ", "Trackbar", &rho_unit, 10);
    createTrackbar("HLine :: Theta ", "Trackbar", &theta_unit, 10);
    createTrackbar("HLine :: ptr_votes_thres ", "Trackbar", &ptr_votes_thres, 300);

    // float 값 두개를 가지는 lines 벡터 변수 선언.
    vector<Vec2f> lines;

    while (true)
    {
        // 이미지 초기화 !
        img = imread(IMG_FILE);
        isEmptyImg(img);

        /* 1. preprocessing img. */
        imgEdge = preProcessing(img);

        /* 2. Hough line Transform(normal version.)
          */

        rho_thres = (double)rho_unit * 0.25;
        theta_degree = (double)theta_unit * 0.5;
        theta_radian = theta_degree * to_radian;

        HoughLines(imgEdge, lines, rho_thres, theta_radian, ptr_votes_thres, 0, 0);
        drawHoughLines(lines);
        cout << "rho : " << rho_thres << "\t , degree : " << theta_degree << "\t , votes : " << ptr_votes_thres << endl;

        imshow("img", img);
        rtnKey = waitKey(1);
        if (rtnKey == 27)
        {
            return 0;
        }
    }
}