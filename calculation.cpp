#include <iostream>
#include "calculation.hpp"
#include "gloabalVal.hpp"

using namespace std;

vector<lineElement> calcLineElement(vector<Vec2f> lines, int leftSide_Angle, int rightSide_Angle)
{
    // Description : Calculating slope, yItcp for 1st line equation...
    vector<lineElement> lineElems(lines.size());
    // vector<lineElement> lineElems;
    // int j = 0, k = 0;

    for (int i = 0; i < lines.size(); i++)
    {
        double rho = lines[i][0], theta = lines[i][1];
        double temp1 = double(90 - leftSide_Angle) * toRadian();
        double temp2 = double(90 + rightSide_Angle) * toRadian();

        // check the slope value
        if (theta < temp1 && theta > 0 || theta < CV_PI && theta > temp2)
        {
            double y0 = rho * sin(theta), x0 = rho * cos(theta);
            double slope = tan(CV_PI / 2 - theta);
            double yItcp = y0 - slope * x0;

            lineElems[i].slope = slope;
            lineElems[i].yItcp = yItcp;
        }
    }
    return lineElems;
}

vector<pointsElement> getLinePoints(vector<lineElement> lineElems, Mat img, double topPosY)
{
    /*
     Description : Calculating and getting the Points for drawing on Img.
     One top point and bottom one point are set for drawing the line.

     Input      : lineElems     ([i][slope], [i][yItcp]),
                  img           (for calculation width and height of img.)
                  topPosY       (0~1.0, height ratio for pixel position.)
     Output     : two points    (topPoint: at fixed y pixel position, bottomPoint : bottom of img position)
    */
    vector<pointsElement> pntElems(lineElems.size());
    int topYPos = img.size().height * (1 - topPosY);
    int btmYPos = img.size().height + 10;

    for (int i = 0; i < lineElems.size(); i++)
    // using 1st line equation..
    {
        pntElems[i].topPt.y = topYPos;
        pntElems[i].topPt.x = (topYPos - lineElems[i].yItcp) / lineElems[i].slope;

        pntElems[i].bottomPt.y = btmYPos;
        pntElems[i].bottomPt.x = (btmYPos - lineElems[i].yItcp) / lineElems[i].slope;
    }

    return pntElems;
}