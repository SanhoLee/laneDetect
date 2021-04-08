#include "calculation.hpp"
#include "gloabalVal.hpp"

vector<lineElement> calcLineElement(vector<Vec2f> lines)
{
    // Description : Calculating slope, yItcp for 1st line equation...
    vector<lineElement> lineElems(lines.size());

    for (int i = 0; i < lines.size(); i++)
    {
        double rho = lines[i][0], theta = lines[i][1];

        float y0 = rho * sin(theta), x0 = rho * cos(theta);
        float slope = tan(CV_PI / 2 - theta);
        float yItcp = y0 - slope * x0;
        lineElems[i].slope = slope;
        lineElems[i].yItcp = yItcp;
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
    // not yet...

    // pntElems[0].topPt;
    // pntElems[0].bottomPt;

    return pntElems;
}