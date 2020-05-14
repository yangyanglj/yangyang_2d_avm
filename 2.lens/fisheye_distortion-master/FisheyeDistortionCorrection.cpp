#include "FisheyeDistortionCorrection.h"

#include <QImage>
#include <QDebug>
#include <qmath.h>
#include <QFile>

//#define PARABOLIC
#define CIRCEL
//#define ELLIPSE
FisheyeDistortionCorrection::FisheyeDistortionCorrection()
{
    Initialize();
}

FisheyeDistortionCorrection *FisheyeDistortionCorrection::getInstance() {
   static FisheyeDistortionCorrection correction;
   return &correction;
}

void FisheyeDistortionCorrection::Initialize() {
    SetPictureSize(0,0);
    SetOpticalCenterPoint(0, 0);
    SetRotation(0);
    SetCrop(0, 0, 0, 0);
    Set2rdCurveCoff(0,0);
    SetFileLocation(QString(""));
}

void FisheyeDistortionCorrection::SetPictureSize(int width, int height) {
    mWidth = width;
    mHeight = height;
    qDebug("PictureSize: %dx%d", width, height);
}

void FisheyeDistortionCorrection::SetFileLocation(QString filepath) {
    mFilePath = filepath;
    qDebug() << "FilePath: " << filepath << endl;
}


QImage FisheyeDistortionCorrection::DoImageRotate(QImage *image, int angleValue)
{
    QMatrix matrix;
    matrix.rotate(angleValue);
    QImage transfrom = image->transformed(matrix, Qt::FastTransformation);
    transfrom.save("rotage.jpg");
    qDebug() << "transform size: "<< transfrom.width() << "x" << transfrom.height() << endl;
    return transfrom.scaled(image->size());
}

void FisheyeDistortionCorrection::SetOpticalCenterPoint(int x, int y)
{
    mOpticalCenterX = x;
    mOpticalCenterY = y;
    qDebug("OpticalCenterPoint = %dx%d", mOpticalCenterX, mOpticalCenterY);
}

void FisheyeDistortionCorrection::Set2rdCurveCoff(int hBase, int vBase)
{
    mHorizontalBase = hBase;
    mVerticalBase   = vBase;
    qDebug("Set 2rd Curve Coff: %d, %d", mHorizontalBase, mVerticalBase);
}

void FisheyeDistortionCorrection::SetRotation(int rotation)
{
    mRotation = rotation;
    qDebug("Rotation= %d", rotation);
}

void FisheyeDistortionCorrection::SetCrop(int x, int y, int w, int h)
{
    mCropX = x;
    mCropY = y;
    mCropW = w;
    mCropH = h;
    qDebug("SetCrop: %d, %d, %d, %d", mCropX, mCropY, mCropW, mCropH);
}

QImage FisheyeDistortionCorrection::GetDefaultImage()
{
    QImage image(mFilePath);
    if (image.isNull())
    {
        qDebug() << "bad image input";
        return image;
    }
    return image.convertToFormat(QImage::Format_RGB888);
}

double FisheyeDistortionCorrection::GetArchLensOfCircel(double a, double b, double r, int x)
{
    (void)b;
    // original pos( a, b), ref pos (a, 0).
    return ((asin(qAbs(a-x) / r))  * r);
}

double FisheyeDistortionCorrection::GetAngelOfTwoLines(double k1, double k2)
{
    if (k1 * k2 > -1.05 && k1 * k2 < -0.95)
        return M_PI_4;
    double tanangel = qAbs((k1 - k2) / ( 1 + k1 * k2));
    double angle = atan(tanangel);
    if (k1 * k2 < 0 && k1 * k2 > -1.0)
        angle = M_PI - angle;
    return angle;
}

void FisheyeDistortionCorrection::Process5(QImage *oriImage, QImage *output)
{
    const int width     = oriImage->width();
    const int height    = oriImage->height();
    // the optical center and radius should be calibrated alone.
    const int oc_x      = (width -1) / 2;
    const int oc_y      = (height -1) / 2;
    const int max_arc   = oc_x;

    // radius1 * PI / 2 =  max_arc
    const int radius1   = static_cast<int>(max_arc / M_PI_2);
    const int width1    = radius1 * 2;
    const int height1   = radius1 * 2;
    const int oc_x1     = radius1;
    const int oc_y1     = radius1;

    QImage final(width1, height1, QImage::Format_RGB888);
    for (int y1 = 0; y1 < height1; ++y1)
    {
        for (int x1 = 0; x1 < width1; ++x1)
        {
            double k    = (x1 - oc_x1) / static_cast<double>(y1 - oc_y1);
            int dist1   = GetDistance(x1, y1, oc_x1, oc_y1);
            // k        = (x - oc_x) / static_cast<double>(y - oc_y)
            // int arc  = GetDistance(x, y, oc_x, oc_y);
            // angle    = arc/max_arc * M_PI_2;
            // dist1    = radius1 * (1 - cos(angle)).
            // we can calculate the x, y.
            //int arc     = static_cast<int>(asin(dist1/static_cast<double>(radius1)) * max_arc / M_PI_2);
            int arc     = static_cast<int>(acos( 1 - dist1/static_cast<double>(radius1)) * max_arc / M_PI_2);
            //arc         = static_cast<int>(qPow(arc/static_cast<double>(max_arc), 0.01) * arc);

            int y = 0;
            if ( y1 < oc_y1)
            {
                y = static_cast<int>(oc_y - arc / qSqrt(k * k + 1));
            }
            else
            {
                y = static_cast<int>(oc_y + arc/qSqrt(k * k + 1));
            }
            int x = static_cast<int>( (y - oc_y) * k + oc_x);

            if (x < 0) x = 0;
            if (x > width -1) x = width-1;

            if (y < 0) y = 0;
            if (y > height -1) y = height -1;
            final.setPixel(x1, y1, oriImage->pixel(x, y));
        }
    }
    *output = final;
}

void FisheyeDistortionCorrection::Process4(QImage *oriImage, QImage *hImage)
{
    const int width     = oriImage->width();
    const int height    = oriImage->height();
    
    // the optical center and radius should be calibrated alone.
    const int optical_center_x  = (width -1) / 2;
    const int optical_center_y  = (height -1) / 2;
    const int radius            = optical_center_x;
    
    qDebug(" image size = %dx%d", width, height);
    qDebug(" image optical center point = %dx%d", optical_center_x, optical_center_y);
    

    const int width1            = AlignTo(static_cast<int>(radius * M_PI), 2);
    const int height1           = width1;
    const int optical_center_x1 = (width1-1) / 2;
    const int optical_center_y1 = (height1-1) / 2;
    const int radius1           = optical_center_x1;
    //const int max_arc       = x1_width / 2;
    //const int width_center  = max_arc -1;
    //const int height_center = max_arc -1;
    qDebug(" new Image size = %dx%d",width1, height1);
    qDebug(" new image optical center point = %dx%d", optical_center_x1, optical_center_y1);

    QPoint **array;
    Create2DArray(array, height1, width1);

    int dist_max    = optical_center_x;//GetDistance(optical_center_x, 0, optical_center_x, optical_center_y);
    int dist1_max   = optical_center_x1;//GetDistance(optical_center_x1, 0, optical_center_x1, optical_center_y1);
    qDebug("dist_max = %d, dist1_max1 = %d", dist_max, dist1_max);

    for (int h1 = 0; h1 < height1; h1++)
    {
        for (int w1 = 0; w1 < width1; w1++)
        {
            int dist1 = GetDistance(w1, h1, optical_center_x1, optical_center_y1);
            if (dist1 > dist1_max)
            {
                array[h1][w1].setX(0);
                array[h1][w1].setY(0);
            }
            else
            {
                int x1 = w1;
                int y1 = h1;
                if ( y1 == optical_center_y1)
                {
                    array[h1][w1].setX(0);
                    array[h1][w1].setY(0);
                }
                else
                {
                    double k1        = (x1 - optical_center_x1) / static_cast<double>(y1 - optical_center_y1);
                    //k1            = (x - optical_center_x) / (y - optical_center_y);
                    // cos(dist/dist_max * M_PI_2) = (dist1_max - dist1) / dist1_max

                    //int distance    = static_cast<int>(acos((dist1_max - dist1) / static_cast<double>(dist1_max)) * dist_max / M_PI_2);

                    // distance1 = sin(dist/dist_max * M_PI_2) * radius
                    // int distance = static_cast<int>(asin(dist1 / static_cast<double>(radius)) * dist_max / M_PI_2);

                    // dist1_max - dist1 = cos(dist /dist_max * M_PI_2)*radius
                    int distance = static_cast<int>(acos((dist1_max - dist1) / static_cast<double>(radius1)) * dist_max / M_PI_2);
                    // distance     = GetDistance(x, y, optical_center_x, optical_center_y);
                    // int distance = static_cast<int>(dist1/static_cast<float>(dist1_max) * radius);
                    // here we can calculate the x, y.
                    int x = 0;
                    int y = 0;
                    if (y1 <optical_center_y1)
                    {
                        //x = optical_cen+-_x - distance/(1+k);
                        y = static_cast<int>(optical_center_y - distance/qSqrt(1+k1*k1));

                    }
                    else
                    {
                        y = static_cast<int>(optical_center_y + distance/qSqrt(1+k1*k1));
                    }

                    x = static_cast<int>(k1 * ( y - optical_center_y) + optical_center_x);

                    if (x < 0 || x > width || y < 0 || y > height)
                    {
                        array[h1][w1].setX(0);
                        array[h1][w1].setY(0);
                    }
                    else
                    {
                        array[h1][w1].setX(x);
                        array[h1][w1].setY(y);
                    }
                    if (x1 == 200)
                    {
                        qDebug("k1 = %f, x = %d, y = %d, x1 = %d, y1 = %d,dist = %d, dist1 =%d",
                               k1, x, y, x1, y1, distance, dist1);
                    }
                }

            }
            
        }
    }
    QImage finalImage(width1,height1,QImage::Format_RGB888);
    for (int y = 0; y < height1; y++)
    {
        for (int x = 0; x < width1; x++)
        {
            finalImage.setPixel(x, y, oriImage->pixel(array[y][x]));
        }
    }
    *hImage = finalImage;
}

int FisheyeDistortionCorrection::GetDistance(int x, int y, int x1, int y1)
{
    return static_cast<int>(qSqrt((x - x1) * (x -x1) + (y - y1) * (y - y1)));
}

int FisheyeDistortionCorrection::GetDistance2(int x, int y, int x1, int y1)
{
    return static_cast<int>((x1 - x1) * (x - x1) + (y - y1) * (y - y1));
}

template<typename T>
void FisheyeDistortionCorrection::Create2DArray(T **&array, int height, int width)
{
    array = new T * [height];
    for (int h = 0; h < height; ++h)
    {
        array[h] = new T [width];
    }
}

template <typename T>
void FisheyeDistortionCorrection::Destroy2DArray(T **&array, int height)
{
    if (array != NULL)
    {
        for (int h = 0; h < height; h++)
        {
            delete [] array[h];
        }
    }
    delete[] array;
}
// here, we suspect the standard equation of the circle satisfied the our requirement.
// (x -a) * (x -a) + (y -b) * (y -b) = r * r;
void FisheyeDistortionCorrection::Process3(QImage *oriImage, QImage *rotateImage, QImage *hImage, QImage *vImage, QImage *smoothImage, QImage *strecthImage)
{


    const int width     = oriImage->width();
    const int height    = oriImage->height();

    if (width != mWidth || height != mHeight)
    {
        qDebug("mismatch: set size: %dx%d, image size: %dx%d", mWidth, mHeight, width, height);
        return;
    }
    qDebug("original image size = %dx%d", width, height);


    /**
     * we need do the rotate before the lend distortion correction.
     * our calculate ldc based on the iamge coordinate system.
     * we separate the correction with the horizontal base on image x aix.
     * and the vertical base on image y aix.
     * so, we sould make sure the image distortion without angle shift.
     **/

    *rotateImage = DoImageRotate(oriImage, mRotation);
    qDebug("rotate Image size: %d, %d", rotateImage->width(), rotateImage->height());

    const int opticalCenterW        = (mOpticalCenterX == 0) ? ((width -1) / 2) : mOpticalCenterX;
    const int opticalCenterH        = (mOpticalCenterY == 0) ? ((height -1) / 2) : mOpticalCenterY;
    const int maxVerticalArcLength  = AlignTo(static_cast<int>(opticalCenterH * M_PI), 2);
    const int maxHorizontalArcLengh = AlignTo(static_cast<int>(opticalCenterW * M_PI), 2);
    qDebug("optical center point: %dx%d", opticalCenterW, opticalCenterH);
    qDebug("maxVerticalArcLength = %d, maxHorizontalArcLength = %d",
           maxVerticalArcLength, maxHorizontalArcLengh);

    /**
     * do horizontal correction.
     * suspect the opitial pointer: (opticalCenterW, opticalCenterW).
     * the coordinate system: x aix <---> width; y aix <---> height.
     */

    QPoint *mappedX             = new QPoint[maxHorizontalArcLengh * height];
    double *arcLength           = new double[opticalCenterW];
    const int verticalBase      = (mVerticalBase == 0) ? height / 4: mVerticalBase;

    for (int h = 0; h <= opticalCenterH; ++h)
    {
        /**
         * the euqtion should locate on these three points.
         * then, we can calculate the coff: a , b , r
         * here, the coffH is tuneable value according the the h
         **/

        // h / (height / 2) = (coffH - hBase) / (height/2 - hBase).

        // Notice: here plus 1 is to avoid the circel equation error at the critical status,
        double coffH    = h * (opticalCenterH - verticalBase + 1) / static_cast<double>(opticalCenterH) + verticalBase;

        if (std::abs(h - coffH) < 1)
        {
            qDebug("h = %d, coffH = %lf", h , coffH);
            h = h +1;
            //continue;
        }
        double a = opticalCenterW;
        double b = (h + coffH - a * a / static_cast<double>(h- coffH)) / 2;
        double r = pow((h - b) * (h - b), 0.5);

        if ( h + 10 > opticalCenterH)
        {
            qDebug("a = %lf, b = %lf, c = %lf", a, b, r);
        }

        for (int arc = 0; arc < opticalCenterW; arc++)
        {
            arcLength[arc] = GetArchLensOfCircel(a, b, r, arc);
            //qDebug("arcLengthDeltaX = %lf", arcLength[arc]);
        }

        int start       = 0;
        int curr        = 0;
        int baseX       = h * maxHorizontalArcLengh;
        int baseXFlip   = (height -1 - h) * maxHorizontalArcLengh;

        for (int w = 0; w < width; ++w)
        {
            int x0                  = w;
            int y0                  = Range(static_cast<int>(b - pow (pow(r, 2) - pow( x0 - a, 2), 0.5)), 0, height-1);
            int x0Flip              = w;
            int y0Flip              = Range(height - 1 - y0, 0, height-1);
            int arc                 = (w < opticalCenterW) ? w : ( 2 * opticalCenterW - w - 1);
            double arcLengthx       = arcLength[arc];

            if (h == 0 && w == 0)
            {
                qDebug("arctan = %lf", asin(a/r));
                qDebug("a = %f, b = %f, r = %f", a, b, r);
                qDebug("the maxHorizontalArcLengh = %d, arcLength = %f", maxHorizontalArcLengh, arcLengthx);
            }


            // do arcLengthx compensation.

            // arcLenghx = 1.6 * arcLengthx;

            // it will better for strength
            arcLengthx = (1 + 0.6 * pow(arcLengthx/arcLength[0], 3)) * arcLengthx;
            if ( x0 < opticalCenterW )
            {
                curr = maxHorizontalArcLengh / 2 - static_cast<int>(arcLengthx);
            }
            else
            {
                curr = maxHorizontalArcLengh / 2 + static_cast<int>(arcLengthx);
            }
            curr = Range(curr, 0, maxHorizontalArcLengh - 1);
            if (curr < start) curr = start;
            if (h == opticalCenterH || h == opticalCenterH -1 )
            {
                //qDebug("start =%d curr = %d, x = %d, y = %d", start, curr, x0, y0);
                //curr = maxHorizontalArcLengh - 1;
            }


            //qDebug() << "baseX = "<< baseX << endl;
            //qDebug() << "start = "<< start <<", curr = "<< curr << endl;
            for (int k = start; k < curr; ++k)
            {
                mappedX[baseX + k].setX(x0);
                mappedX[baseX + k].setY(y0);
                if (baseX != baseXFlip)
                {
                    mappedX[baseXFlip + k].setX(x0Flip);
                    mappedX[baseXFlip + k].setY(y0Flip);
                }

            }
            start = curr;
        }
    }

    QImage horizonCorrection(maxHorizontalArcLengh, height, QImage::Format_RGB888);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < maxHorizontalArcLengh; x++)
        {
            horizonCorrection.setPixel(x, y, rotateImage->pixel(mappedX[y* maxHorizontalArcLengh + x]));
        }
    }
    *hImage = horizonCorrection;

    qDebug("Horizontal Correction Done");


    //============================== do veritical strength ==============================
#if 1
    /**
     *  the vertical correction.
     *  equation: y = a*x*x + bx + c.
     **/
    QImage verticalCorrection(hImage->width(), hImage->height(), QImage::Format_RGB888);
    double base_offset      = (mHorizontalBase == 0) ? width / 4.0: mHorizontalBase;
    double center_offset    = hImage->width() / 2;

    for (int w = 0; w < hImage->width() / 2; ++w)
    {
        double offset = (w / center_offset) * (center_offset - base_offset) + base_offset;
        double x0   = height/2.0;
        double y0   = w;

        double x1   = 0;
        double y1   = offset;

        double x2   = height;
        double y2   = offset;

        double c    = offset;
        double a    = (y0 - c) / (x0 * x0 - x0*x2);
        double b    = -a*x2;
        if (fabs(w -c) < 0.1) continue;
        for (int h = 0; h < hImage->height(); ++h)
        {
            int x = h;
            int y = static_cast<int>(a * x * x + b * x + c);

            int w1 = y;
            int h1 = x;
            if (w1 > hImage->width() -1)
                w1 = hImage->width() -1;

            verticalCorrection.setPixel(w, h, hImage->pixel(w1, h1));
            verticalCorrection.setPixel(hImage->width() - w -1, h, hImage->pixel(hImage->width() - w1 -1, h1));
        }
    }
    *vImage = verticalCorrection;
#else
    /**
     * the verital correcion.
     * every column will satisfy the same standard circel equation.
     * (x -a ) * (x - a) + (y - b)* (y - b) = r * r;
     *
     * (0, 0), (opticalCenterH, coffW), ( 2* opticalCenterH, 0).
     **/
    int coff = 350;
    double a = opticalCenterH;
    double b = (coff - a * a / (double) coff) / 2.0f;
    double r = pow(a * a + b * b, 0.5f);
    qDebug() << "standard circel equation for vertical correction: a = "<< a << ",b = " << b << ", r = " << r << endl;
    // get the max arc length from (0, 0) to (2 * poticalCenterH, 0).

    // y = a1* x + b1 , line through the (0, 0) and (a, b).
    //float a1 = b / a;
    //float b1 = 0;
    double k1 = b / a; // k1 = a1;

    // y = a2 * x + b2, line thougth the (2*poticalCenterH, 0) and (a, b)
    //float a2 = b / ( a - 2 * opticalCenterH);
    //float b2 = -a2 * 2 * opticalCenterH;
    double k2 = b / ( a - 2 * opticalCenterH); // k2 = a2;

    // arch = angle / 2PI * 2PI * R;
    int   arcH = (int)(qAbs(GetAngelOfTwoLines(k1, k2) * r));
    qDebug() << "k1 = " << k1 << ",k2 = " << k2 << endl;
    qDebug() << "arcH = " << arcH;
    QImage verticalCorrection(maxHorizontalArcLengh, arcH, QImage::Format_RGB888);
    int *mappedH = new int[arcH];
    int curr = 0;

    for (int h = 0; h < height; h++)
    {
        int x3 = h;
        int y3 = b + pow( r * r - ( x3 - a) * (x3 - a), 0.5f);
        // y = a3 * x + b2, line thougth the (x3, y3). (a, b)
        int arcLength = 0;
        if ( qAbs(x3 - a) < 0.1)
        {
            arcLength = curr;
            qDebug() << "a == x3 = " << x3 << ",arcLength = " << arcLength << endl;
        }
        else
        {
            double k3  = ( b- y3) / (float)(a - x3);
            double angle = qAbs(GetAngelOfTwoLines(k1, k3));
            arcLength = (int)(angle * r);
            qDebug() << "h = " << h << "k3 = " << k3 << "angle = " << angle <<",arcLength " << arcLength << endl;
            if (arcLength > arcH)
            {
                qDebug() << "bad arcLength= " << arcLength << endl;
                arcLength = arcH;
            }
        }
        qDebug() << "curr " << curr <<  endl;
        if (curr  > arcLength)
        {
            qDebug() << "should not goto here"<< endl;
            arcLength = curr;
        }
        for (int index = curr; index < arcLength; ++index)
        {
            mappedH[index] = h;
        }

        curr = arcLength;
    }
    for (int index = 0; index < arcH; ++index)
    {
        qDebug() << "mappedH " << mappedH[index] << endl;
    }
#if 1
    for (int h = 0; h < arcH ; h++)
    {
        for (int w = 0; w < maxHorizontalArcLengh; ++w)
        {
            verticalCorrection.setPixel(w, h, hImage->pixel(w, mappedH[h]));
        }
    }
#endif
    *vImage = verticalCorrection;
#endif
    //*vImage = horizonCorrection;


    int cropX0  = mCropX;
    int cropY0  = mCropY;
    int cropW   = mCropW;
    int cropH   = mCropH;
    qDebug("cropX =%d, cropY = %d, cropW = %d, cropH = %d", cropX0, cropY0, cropW, cropH);
    //*smoothImage = horizonCorrection.copy(cropX0,cropY0, cropW, cropH);

    //*smoothImage    = hImage->scaled(width, height, Qt::KeepAspectRatio, Qt::FastTransformation);
    //*strecthImage   = vImage->scaled(width, height, Qt::KeepAspectRatio, Qt::FastTransformation);
    *smoothImage      = hImage->copy(cropX0, cropY0, cropW, cropH);
    *strecthImage     = vImage->copy(cropX0, cropY0, cropW, cropH);
    qDebug("strecth image wxh = %dx%d", strecthImage->width(), strecthImage->height());
}


// here, we suspect the standard equation of the circle satisfied the our requirement. 
// (x -a) * (x -a) + (y -b) * (y -b) = r * r;
void FisheyeDistortionCorrection::Process2(QImage *oriImage, QImage *rotateImage, QImage *hImage, QImage *vImage, QImage *smoothImage, QImage *strecthImage)
{
    const int width     = oriImage->width();
    const int height    = oriImage->height();

    if (width != mWidth || height != mHeight)
    {
        qDebug("mismatch: set size: %dx%d, image size: %dx%d", mWidth, mHeight, width, height);
        return;
    }
    qDebug("original image size = %dx%d", width, height);

    *rotateImage = DoImageRotate(oriImage, mRotation);
    qDebug("rotate Image size: %d, %d", rotateImage->width(), rotateImage->height());

    const int opticalCenterW        = (mOpticalCenterX == 0) ? ((width -1) / 2) : mOpticalCenterX;
    const int opticalCenterH        = (mOpticalCenterY == 0) ? ((height -1) / 2) : mOpticalCenterY;
    const int maxVerticalArcLength   = AlignTo(opticalCenterH * M_PI, 2);
    const int maxHorizontalArcLengh = AlignTo(opticalCenterW * M_PI, 2);
    qDebug("optical center point: %dx%d", opticalCenterW, opticalCenterH);
    qDebug("maxVerticalArcLength = %d, maxHorizontalArcLength = %d",
           maxVerticalArcLength, maxHorizontalArcLengh);

    /**
     * do horizontal correction.
     * suspect the opitial pointer: (opticalCenterW, opticalCenterW).
     * the coordinate system: x aix <---> width; y aix <---> height.
     */

    QPoint *mappedX             = new QPoint[maxHorizontalArcLengh * height];
    float *arcLength            = new float[opticalCenterW];
    const int verticalBase      = (mVerticalBase == 0) ? height / 4: mVerticalBase;

    for (int h = 0; h < opticalCenterH; ++h)
    {
        /**
         * the euqtion should locate on these three points.
         * then, we can calculate the coff: a , b , r
         * here, the coffH is tuneable value according the the h
         **/

        // h / (height / 2) = (coffH - hBase) / (height/2 - hBase).
        float coffH    = h * (opticalCenterH - verticalBase) / (opticalCenterH) + verticalBase;

        if (h == coffH) break;
        float a = opticalCenterW;
        float b = (h + coffH - a * a / (float)(h- coffH)) / 2;
        float r = pow((h - b) * (h - b), 0.5f);
        
        //qDebug("a = %f, b = %f, c = %f", a, b, c);
        for (int arc = 0; arc < opticalCenterW; arc++)
        {
            arcLength[arc] = GetArchLensOfCircel(a, b, r, arc);
            //qDebug("arcLengthDeltaX = %f", arcLengthDeltaX[arc]);
        }

        int start = 0;
        int curr  = 0;
        for (int w = 0; w < width; ++w)
        {
            int x0                  = w;
            int y0                  = Range(b - pow (pow(r, 2) - pow( x0 - a, 2), 0.5f), 0, height-1);
            int x0Flip              = w;
            int y0Flip              = Range(height - 1 - y0, 0, height-1);

            int arc = (w < opticalCenterW) ? w : ( 2 * opticalCenterW - w);

            float arcLenghx          = arcLength[arc];

            if (h == 0 && w == 0)
            {
                qDebug("arctan = %lf", asin(a/r));
                qDebug("a = %f, b = %f, r = %f", a, b, r);
                qDebug("the maxHorizontalArcLengh = %d, arcLength = %f", maxHorizontalArcLengh, arcLenghx);
            }

            if ( x0 < opticalCenterW ) {
                curr = maxHorizontalArcLengh / 2 - (int)arcLenghx;
            }
            else
            {
                curr = maxHorizontalArcLengh / 2 + (int)arcLenghx;
            }

            curr = Range(curr, 0, maxHorizontalArcLengh - 1);

            int baseX       = h * maxHorizontalArcLengh;
            int baseXFlip   = (height -1 - h) * maxHorizontalArcLengh;

            //qDebug() << "baseX = "<< baseX << endl;
            //qDebug() << "start = "<< start <<", curr = "<< curr << endl;

            for (int k = start; k < curr; ++k)
            {
                mappedX[baseX + k].setX(x0);
                mappedX[baseX + k].setY(y0);

                mappedX[baseXFlip + k].setX(x0Flip);
                mappedX[baseXFlip + k].setY(y0Flip);
            }

            start = curr;
        }
    }

    qDebug("Horizontal Correction");
    QImage horizonCorrection(maxHorizontalArcLengh, height, QImage::Format_RGB888);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < maxHorizontalArcLengh; x++)
        {
            horizonCorrection.setPixel(x, y, rotateImage->pixel(mappedX[y* maxHorizontalArcLengh + x]));
        }
    }
    *hImage = horizonCorrection;

    /**
     * do horizontal correction, two-order curve.
     * suspect the opitial pointer: (w/2, h/2).
     * the coordinate system: x aix <---> height, y aix <---> width.
     * the equation: y = a*x^x + b*x + c.
     * three point should be:
     *  (0, coffW), (height / 2, y'), (height, coffW).
     * here, the y' should be changed according to the peak of the curve.
     *       the coffW is the dynamic change according the picture view.
     */

#if 1

    QPoint *mappedY             = new QPoint[maxHorizontalArcLengh * maxVerticalArcLength];
    float *arcLengthDeltaY      = new float[opticalCenterH];
    int horizontalBase          = (mHorizontalBase == 0) ? maxHorizontalArcLengh / 8 : mHorizontalBase;
    for (int w = 0; w < maxHorizontalArcLengh / 2; ++w)
    {
        /**
         * the equation should locate these three points.
         * (0, coffW), (opticalCenterH, w), (2 * opticalCenterH,  coffW)
         **/
        //  coffW / (width / 2 - baseW) = w / (width / 2)
        int coffW       = w * (maxHorizontalArcLengh / 2 - horizontalBase) / (maxHorizontalArcLengh / 2) + horizontalBase;

        float a = opticalCenterH;
        float b = (w + coffW - a * a / (float)(w -coffW)) / 2;
        float r = pow((w - b) * (w - b), 0.5f);

        //qDebug("a = %f, b = %f, c = %f", a, b, c);
        for (int arc = 0; arc < opticalCenterH; arc++)
        {
            arcLength[arc] = GetArchLensOfCircel(a, b, r, arc);
        }

        int start = 0;
        int curr  = 0;
        for (int h = 0; h < height; ++h)
        {
            int x0                  = h;
            int y0                  = Range(b - pow (pow(r, 2) - pow( x0 - a, 2), 0.5f), 0, width-1);
            int x0Flip              = h;
            int y0Flip              = maxHorizontalArcLengh - 1 - y0;

            int arc = (x0 < opticalCenterH) ? x0 : ( 2 * opticalCenterH - x0);
            arc     = Range(arc, 0, maxVerticalArcLength-1);
            int arcLengthY = arcLength[arc];
            if ( h == 0 && w == 0)
            {
                qDebug("a = %f, b = %f, r = %f", a, b, r);
                qDebug("the maxVerticalArcLength= %d, arcLength = %d", maxVerticalArcLength, arcLengthY);
            }

            if ( x0 < opticalCenterH)
            {
                curr = maxVerticalArcLength / 2 - (int)arcLengthY;
            }
            else
            {
                curr = maxVerticalArcLength / 2 + (int)arcLengthY;
            }

            curr = Range(curr, 0, maxVerticalArcLength -1);
#if 1
            for ( int k = start; k < curr; ++k)
            {
                int offset = k * maxHorizontalArcLengh + w;
                int offsetFlip = k * maxHorizontalArcLengh + (maxHorizontalArcLengh -1 - w);
                mappedY[offset].setX(y0);
                mappedY[offset].setY(x0);
                mappedY[offsetFlip].setX(y0Flip);
                mappedY[offsetFlip].setY(x0Flip);
            }
#endif
            start = curr;
        }
    }
    QImage verticalCorrection(maxHorizontalArcLengh, maxVerticalArcLength, QImage::Format_RGB888);
    for (int y = 0; y < maxVerticalArcLength; y++)
    {
        for (int x = 0; x < maxHorizontalArcLengh; x++)
        {
            verticalCorrection.setPixel(x, y, hImage->pixel(mappedY[y * maxHorizontalArcLengh + x]));
        }
    }
    *vImage = verticalCorrection;
#endif
    int cropX0  = mCropX;
    int cropY0  = mCropY;
    int cropW   = mCropW;
    int cropH   = mCropH;
    qDebug("cropX =%d, cropY = %d, cropW = %d, cropH = %d", cropX0, cropY0, cropW, cropH);
    *smoothImage = verticalCorrection.copy(cropX0,cropY0, cropW, cropH);
    *strecthImage = smoothImage->scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    qDebug("strecth image wxh = %dx%d", strecthImage->width(), strecthImage->height());
}


// here, we suspect the second-order curve (y = a*x*x + b* x + c) satisfied the our requirement.
void FisheyeDistortionCorrection::Process1(QImage *oriImage, QImage *rotateImage, QImage *hImage, QImage *vImage, QImage *smoothImage, QImage *strecthImage)
{
    const int width     = oriImage->width();
    const int height    = oriImage->height();

    if (width != mWidth || height != mHeight)
    {
        qDebug("mismatch: set size: %dx%d, image size: %dx%d", mWidth, mHeight, width, height);
        return;
    }
    qDebug("original image size = %dx%d", width, height);

    *rotateImage = DoImageRotate(oriImage, mRotation);
    qDebug("rotate Image size: %d, %d", rotateImage->width(), rotateImage->height());

    const int opticalCenterW        = (mOpticalCenterX == 0) ? ((width -1) / 2) : mOpticalCenterX;
    const int opticalCenterH        = (mOpticalCenterY == 0) ? ((height -1) / 2) : mOpticalCenterY;
    const int maxVerticalArcLength   = AlignTo(opticalCenterH * M_PI, 2);
    const int maxHorizontalArcLengh = AlignTo(opticalCenterW * M_PI, 2);
    qDebug("optical center point: %dx%d", opticalCenterW, opticalCenterH);
    qDebug("maxVerticalArcLength = %d, maxHorizontalArcLength = %d",
           maxVerticalArcLength, maxHorizontalArcLengh);

    /**
     * do horizontal correction, two-order curve.
     * suspect the opitial pointer: (opticalCenterW, opticalCenterW).
     * the coordinate system: x aix <---> width; y aix <---> height.
     * the equation: y = a*x^x + b*x + c.
     * three point should be:
     *  (0, coffH), (opticalCenterW, y'), (opticalCenterW * 2, coffH).
     * here, the y' should be changed according to the peak of the curve.
     */

    QPoint *mappedX             = new QPoint[maxHorizontalArcLengh * height];
    float *arcLengthDeltaX      = new float[opticalCenterW];
    const int verticalBase       = (mVerticalBase == 0) ? height / 4: mVerticalBase;

    for (int h = 0; h < opticalCenterH; ++h)
    {
        /**
         * the euqtion should locate on these three points.
         * (0, coffH), (opticalCenterW, h), (opticalCenterW * 2, coffH)
         * 1: coffH = a * 0 * 0 + b * 0 + c
         * 2: h = a * (opticalCenterW) * (opticalCenterW) + b * (opticalCenterW) + c
         * 3: coffH = a * opticalCenterW * opticalCenterW * 4 + b * opticalCenterW * 2 + c.
         * then, we can calculate the coff: a , b , c.
         * here, the coffH is tuneable value according the the h
         **/

        // h / (height / 2) = (coffH - hBase) / (height/2 - hBase).
        float coffH    = h * (opticalCenterH - verticalBase) / (opticalCenterH) + verticalBase;

        float a         = (coffH - h) / (float)opticalCenterW / (float) opticalCenterW;
        float b         = -a * opticalCenterW * 2;
        float c         = coffH;
        //qDebug("a = %f, b = %f, c = %f", a, b, c);
        for (int arc = 0; arc < opticalCenterW; arc++)
        {
            arcLengthDeltaX[arc] = GetArchLens(a, b, c, arc, arc + 1);
            //qDebug("arcLengthDeltaX = %f", arcLengthDeltaX[arc]);
        }

        int start = 0;
        int curr  = 0;
        for (int w = 0; w < width; ++w)
        {
            int x0                  = w;
            int y0                  = a * x0 * x0 + b * x0 + c;
            int x0Flip              = w;
            int y0Flip              = height - 1 - y0;
            float arcLengthTotalX   = 0.0f;
            for (int arc = (w < opticalCenterW) ? w : opticalCenterW * 2 - w; arc < opticalCenterW; ++arc)
            {
                if (arc < 0) continue;
                arcLengthTotalX += arcLengthDeltaX[arc];
            }

            if (h == 0 && w == 0)
            {
                int arcLength = GetArchLens(a, b, c, x0, y0, width / 2, h);
                qDebug("a = %f, b = %f, c = %f", a, b, c);
                qDebug("the max arcLengthTotalX= %f, arcLength = %d", arcLengthTotalX, arcLength);
            }
            if ( x0 < opticalCenterW ) {
                curr = maxHorizontalArcLengh / 2 - (int)arcLengthTotalX;
            }
            else
            {
                curr = maxHorizontalArcLengh / 2 + (int)arcLengthTotalX;
            }
            Range(curr, 0, maxHorizontalArcLengh - 1);
            int baseX       = h * maxHorizontalArcLengh;
            int baseXFlip   = (height -1 - h) * maxHorizontalArcLengh;
            for (int k = start; k < curr; ++k)
            {
                mappedX[baseX + k].setX(x0);
                mappedX[baseX + k].setY(y0);
                mappedX[baseXFlip + k].setX(x0Flip);
                mappedX[baseXFlip + k].setY(y0Flip);
            }
            start = curr;
        }
    }

    QImage horizonCorrection(maxHorizontalArcLengh, height, QImage::Format_RGB888);
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < maxHorizontalArcLengh; x++)
        {
            horizonCorrection.setPixel(x, y, rotateImage->pixel(mappedX[y* maxHorizontalArcLengh + x]));
        }
    }
    *hImage = horizonCorrection;

    /**
     * do horizontal correction, two-order curve.
     * suspect the opitial pointer: (w/2, h/2).
     * the coordinate system: x aix <---> height, y aix <---> width.
     * the equation: y = a*x^x + b*x + c.
     * three point should be:
     *  (0, coffW), (height / 2, y'), (height, coffW).
     * here, the y' should be changed according to the peak of the curve.
     *       the coffW is the dynamic change according the picture view.
     */

#if 1

    QPoint *mappedY             = new QPoint[maxHorizontalArcLengh * maxVerticalArcLength];
    float *arcLengthDeltaY      = new float[opticalCenterH];
    int horizontalBase          = (mHorizontalBase == 0) ? maxHorizontalArcLengh / 8 : mHorizontalBase;
    for (int w = 0; w < maxHorizontalArcLengh / 2; ++w)
    {
        /**
         * the equation should locate these three points.
         * (0, coffW), (opticalCenterH, w), (2 * opticalCenterH,  coffW)
         * 1: coffW = a * 0 * 0 + b * 0 + c
         * 2: w = a * height / 2 * height / 2 + b * height / 2 + c
         * 3: coffW = a * height * height + b * height + c
         **/
        //  coffW / (width / 2 - baseW) = w / (width / 2)
        int coffW       = w * (maxHorizontalArcLengh / 2 - horizontalBase) / (maxHorizontalArcLengh / 2) + horizontalBase;
        float a         =  ( coffW - w) / (float) opticalCenterH / (float) opticalCenterH;
        float b         =  -2 * a * opticalCenterH;
        float c         = coffW;
        //qDebug("a = %f, b = %f, c = %f", a, b, c);
        for (int arc = 0; arc < opticalCenterH; arc++)
        {
            arcLengthDeltaY[arc] = GetArchLens(a, b, c, arc, arc + 1);
        }

        int start = 0;
        int curr  = 0;
        for (int h = 0; h < height; ++h)
        {
            int x0                  = h;
            int y0                  = a * x0 * x0 + b * x0 + c;
            int x0Flip              = h;
            int y0Flip              = maxHorizontalArcLengh - 1 - y0;
            float arcLengthTotalY    = 0.0f;
            for (int arc = (x0 < opticalCenterH) ? x0 : (2 * opticalCenterH - x0); arc < opticalCenterH; ++arc)
            {
                if (arc < 0) continue;
                arcLengthTotalY += arcLengthDeltaY[arc];
            }
            if ( h == 0 && w == 0)
            {
                int arcLength = GetArchLens(a, b, c, x0, y0, height / 2, w);
                qDebug("a = %f, b = %f, c = %f", a, b, c);
                qDebug("the max arcLengthTotalY= %f, arcLength = %d", arcLengthTotalY, arcLength);
            }

            if ( x0 < opticalCenterH)
            {
                curr = maxVerticalArcLength / 2 - (int)arcLengthTotalY;
            }
            else
            {
                curr = maxVerticalArcLength / 2 + (int)arcLengthTotalY;
            }

            curr = Range(curr, 0, maxVerticalArcLength -1);
#if 1
            for ( int k = start; k < curr; ++k)
            {
                int offset = k * maxHorizontalArcLengh + w;
                int offsetFlip = k * maxHorizontalArcLengh + (maxHorizontalArcLengh -1 - w);
                mappedY[offset].setX(y0);
                mappedY[offset].setY(x0);
                mappedY[offsetFlip].setX(y0Flip);
                mappedY[offsetFlip].setY(x0Flip);
            }
#endif
            start = curr;
        }
    }
    QImage verticalCorrection(maxHorizontalArcLengh, maxVerticalArcLength, QImage::Format_RGB888);
    for (int y = 0; y < maxVerticalArcLength; y++)
    {
        for (int x = 0; x < maxHorizontalArcLengh; x++)
        {
            verticalCorrection.setPixel(x, y, hImage->pixel(mappedY[y * maxHorizontalArcLengh + x]));
        }
    }
    *vImage = verticalCorrection;
#endif
    int cropX0  = mCropX;
    int cropY0  = mCropY;
    int cropW   = mCropW;
    int cropH   = mCropH;
    qDebug("cropX =%d, cropY = %d, cropW = %d, cropH = %d", cropX0, cropY0, cropW, cropH);
    *smoothImage = verticalCorrection.copy(cropX0,cropY0, cropW, cropH);
    *strecthImage = smoothImage->scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    qDebug("strecth image wxh = %dx%d", strecthImage->width(), strecthImage->height());
}

float FisheyeDistortionCorrection::GetArchLens(float a, float b, float c, int x0, int x1)
{
    float arcLength = 0.0f;
    const float deltax = 0.1f;
    float fx0 = x0;
    float fx1 = x1;
    do
    {
        arcLength += qSqrt(1 + qPow(2 * a * fx0 + b, 2)) * deltax;
        fx0 += deltax;
    }
    while ( fx0 < fx1);
    return arcLength;
}

int FisheyeDistortionCorrection::GetArchLens(float a, float b, float c, int x0, int y0, int x1, int y1)
{
    float arcLength = 0.0f;
    if ( x0 == x1) return arcLength;

    // two-order curve is: y = a * x^2 + b * x + c;
    // the derivative equation: y = 2 * a * x + b.
    const float delatx = 0.2f;
    float fx0 = (x0 < x1) ? x0 : x1;
    float fy0 = (x0 < x1) ? y0 : y1;
    float fx1 = (x0 < x1) ? x1 : x0;
    float fy1 = (x0 < x1) ? y1 : y0;

    do
    {
        arcLength += qSqrt(1 + qPow(2 * a * fx0 + b, 2))  * delatx;
        fx0 += delatx;
    } while ( fx0 < fx1 + delatx);

    return arcLength;
}
void FisheyeDistortionCorrection::Process(QImage *ori_image,
                                            QImage *h_image,
                                            QImage *v_image,
                                            QImage *smooth_image,
                                            QImage *strecth_image)
{
    QImage image(mFilePath);
    if (image.isNull()) {
        qDebug() << "bad image input";
        return ;
    }
    *ori_image= image.convertToFormat(QImage::Format_RGB888);
    const int w = ori_image->width();
    const int h = ori_image->height();
    qDebug() << "width = " << w;
    qDebug() << "height = " << h;

    *h_image = *ori_image;
#ifdef PARABOLIC
    // the parabolic is: y = a * (x -k) * (x -k) + c;
    // the parabolic will locate at :
    // point 1(0, coff_c);
    // point 2(w-1, coff_c);

    int y_end = h/2;
    int x_end = w;
    //suspect the coff_a coff_c changed by linerality by y index.
    //when y_index = 0; a = coff_a
    //when y_index = x_end; a = 0;

    // here the coff_a and coff_c is tunable.
    float coff_a = 0.0015f; //coff_value/ coff_range;

    // here the coff_c should be [0, y_end]
    float coff_c = 0;
    float k = w / 2.0f;

    for (int y = 0; y < y_end; y++) {
        float a = coff_a * ( 1 - y /(float) y_end);
        float c = y;
        qDebug() << "a = " << a;
        qDebug() << "c = " << c;
        for (int x = 0; x < x_end; x++) {
            int y1 = a * (x -k)* (x -k) + c;
            if (y1 > y_end) y1 = y_end;
            newRgb.setPixel(x,y, rgb.pixel(x, y1));
        }
    }
#elif defined(CIRCEL)
    // the correction is circle equation.
    // (x - a)^(x - a)  + (y - b)^(y - b) = r * r.
    float a = 0.0f;
    float b = 0.0f;
    float r = 0.0f;
    int x_end = 0.0f;
    int y_end = 0.0f;

    // for horizontal orientation:
    // the optical point is: (a, b), here the a is fixed value:
    //          a = w/2.0f;
    // and then suspect the circel will go through these three point:
    //          (0, coffH), (w, coffH), (a, y); ( y should be 0~b).
    // and resove the equation, we can get the b and r.

    a = (w-1) / 2.0f;

    // then:
    // a^a + (coffH-b)^(coffH-b) = r *r
    // (y-b) ^ (y -b) = r*r

    // here, the coff range is tuneable according to the eyefish picture.
    float hBase = 200;
    x_end = w;
    y_end = (h-1) / 2.0;

    // suspec the coffH match the function: coff = a_h* x^(gamma) + b_h
    // will go through this to point(0, hBase/h_end), ( 1, 1)
    float b_h = hBase/(float)y_end;
    float gamma_h = 1/1.12;
    QColor pre;
    QColor pre1;
    for (int y = 0; y < y_end; y++) {
#if 0
        float coffH = y+ hBase - y * hBase / y_end;
#else
        float a_h = 1 - b_h;
        float coffH = (a_h * pow(y/(float)y_end, gamma_h) + b_h) * y_end;
#endif
        b = (y * y - coffH * coffH - a * a) / (float)(2.0 * ( y - coffH));
        r = sqrt((y-b) * (y-b));
        qDebug() << "a = " << a << ", b = " << b << ",r = " << r;
        for (int x = 0; x < x_end; x++) {
            // y = sqrt(r*r-(x-a)*(x-a));
            int y1 = (int)(b- sqrt(r * r - (x-a)*(x-a)) + 0.5f) ;
            if (y1 < 0) y1 = 0;
            if (y1 > y_end) y1= y_end;
            QColor color = ori_image->pixelColor(x,y1);
            if (x == 0) {
                pre = color;
                pre1 = color;
            }
            color.setRed((color.red() + pre.red() + pre1.red()) / 3 + 0.5f);
            color.setBlue((color.blue() + pre.blue() + pre1.blue()) / 3 + 0.5f);
            color.setGreen((color.green() + pre.green() + pre1.green()) / 3 + 0.5f);

            //h_image->setPixel(x, y, ori_image->pixel(x, y1));
            h_image->setPixelColor(x, y, color);
            pre1 = pre;
            pre = ori_image->pixelColor(x, y1);

            // for horizontal orientation, the picture are symmetric.
            // the symmetric aix is y = y_end;
            int y_mirror = h - y-1;
            if (y_mirror > h) y_mirror = h;
            if (y_mirror < 0) y_mirror = 0;
            int y_mirror1 = h - y1-1;
            if (y_mirror1 > h) y_mirror1 = h;
            if (y_mirror1 < 0) y_mirror1 = 0;
            h_image->setPixel(x, y_mirror, ori_image->pixel(x, y_mirror1));

        }
    }

    *v_image = *h_image;
    // for vertical orientation:
    // the circle point is: (a, b), here the b is fixed value:
    //          b = h / 2.0f;
    // and then suspect the circel will go through these three point:
    //         (x, b), (coff_v, 0) (coff, h)
    // and resove the equation, we can get the b and r.
    b = (h-1) / 2.0f;

    float vBase = -200;
    x_end = (w-1) / 2.0f;
    y_end = h;

    // suspec the coff_v match the function: coff = a_v* x^(gamma) + b_v
    // will go through this to point(0, coff_v/x_end), ( 1, 1)
    float gamma_v = 1/2.4f;
    float b_v = vBase/(float)x_end;
    float a_v = 1 - b_v;
    for (int x = 0; x < x_end; x++) {
#if 0
        float coff_v = vBase - vBase * x / x_end + x;
        //float coff_v = x + vBase;
#else
        float coff_v = (a_v * pow (x/(float)x_end, gamma_v) + b_v) * x_end;

#endif
        if (coff_v > x_end) coff_v = x_end;
        float a = (x * x - coff_v * coff_v - b * b) / (2.0f * (x - coff_v));
        float r2 = (x -a) * (x - a);
        qDebug() <<"x =" << x << "coff_v = " << coff_v;
        qDebug() << "a = " << a << ", b = " << b << ",r = " << r;
        for (int y = 0; y < y_end; y++) {
            int x1 = int(a - sqrt(r2 - (y-b) * (y-b)) + 0.5f);
            if (x1 < 0) x1 = 0;
            if (x1 > x_end) x = x_end;
            if (coff_v < x) x1 = x;
            QColor color = h_image->pixelColor(x1, y);
            if (y == 0) {
                pre1 = color;
                pre = color;
            }
            color.setRed((color.red() + pre.red() + pre1.red()) / 3 + 0.5f);
            color.setBlue((color.blue() + pre.blue() + pre1.blue()) / 3 + 0.5f);
            color.setGreen((color.green() + pre.green() + pre1.green()) / 3 + 0.5f);

            //v_image->setPixel(x, y, h_image->pixel(x1, y));
            v_image->setPixelColor(x, y, color);
            pre1 = pre;
            pre = h_image->pixelColor(x1, y);
            // for vertical oriention, the picture still are symmetric
            int x_mirror = w-x-1;
            int x1_mirror = w-x1-1;
            v_image->setPixel(x_mirror, y, h_image->pixel(x1_mirror, y));
        }
    }
    v_image->save("v_image.jpg");
#elif defined(ELLIPSE)

#endif



    // improve the sawtooth.
    *smooth_image = *ori_image;
#if 0
    QImage gray = v_image->convertToFormat(QImage::Format_Grayscale8);
    int min_threshold = 60;
    int max_threshold = 140;
    int kernel_w = 3;
    int kernel_h = 3;
    int pixel[kernel_w][kernel_h];
    for (int y = 0; y < h-kernel_h; y++) {
        for (int x = 0; x < w-kernel_w; x++) {
            for (int y1 = 0; y1 < kernel_h ; y1++) {
                for (int x1 = 0; x1 < kernel_w; x1++) {
                    if (gray.pixelColor(x1+x, y1+y).value() > max_threshold) {
                        pixel[x1][y1] = 1;
                    } else if (gray.pixelColor(x1+x, y1+y).value() < min_threshold) {
                        pixel[x1][y1] = 0;
                    } else {
                        pixel[x1][y1] = -2;
                    }
                }
            }
            bool need_filter = false;
            do {
                int b0 = pixel[0][0] + pixel[0][1] + pixel[0][2];
                int b1 = pixel[0][2] + pixel[1][2] + pixel[2][2];
                int b2 = pixel[2][0] + pixel[2][1] + pixel[2][2];
                int b3 = pixel[0][0] + pixel[1][0] + pixel[2][0];
                if (pixel[1][1] == 0) {
                    if(b0+b1 == 6 || b1+b2 == 6 || b2+b3== 6 || b3+b0 == 6) {
                        need_filter = true;
                        break;
                    }

                }
                if (pixel[1][1] == 1) {
                    if (b0+b1 == 0 || b1+b2 == 0 || b2+b3== 0 || b3+b0 == 0){
                        need_filter = true;
                        break;
                    }
                }
            } while(0);
            if (need_filter == true) {
                QColor color;
                color.setRed((v_image->pixelColor(x,y).red()
                             + v_image->pixelColor(x+1, y).red()
                             + v_image->pixelColor(x+2, y).red()
                             + v_image->pixelColor(x,y+1).red()
                             + v_image->pixelColor(x+1,y+1).red()
                             + v_image->pixelColor(x+2,y+1).red()
                             + v_image->pixelColor(x, y+2).red()
                             + v_image->pixelColor(x+1, y+2).red()
                             + v_image->pixelColor(x+2, y+2).red())/9);
                color.setGreen((v_image->pixelColor(x,y).green()
                             + v_image->pixelColor(x+1, y).green()
                             + v_image->pixelColor(x+2, y).green()
                             + v_image->pixelColor(x,y+1).green()
                             + v_image->pixelColor(x+1,y+1).green()
                             + v_image->pixelColor(x+2,y+1).green()
                             + v_image->pixelColor(x, y+2).green()
                             + v_image->pixelColor(x+1, y+2).green()
                             + v_image->pixelColor(x+2, y+2).green())/9);
                color.setBlue((v_image->pixelColor(x,y).blue()
                             + v_image->pixelColor(x+1, y).blue()
                             + v_image->pixelColor(x+2, y).blue()
                             + v_image->pixelColor(x,y+1).blue()
                             + v_image->pixelColor(x+1,y+1).blue()
                             + v_image->pixelColor(x+2,y+1).blue()
                             + v_image->pixelColor(x, y+2).blue()
                             + v_image->pixelColor(x+1, y+2).blue()
                             + v_image->pixelColor(x+2, y+2).blue())/9);
#if 0
                //debug
                color.setRed(255);
                color.setGreen(64);
                color.setBlue(64);

#endif
                smooth_image->setPixelColor(x+1, y+1, color);
#if 0
                for (int y1 = 0; y1 < 3; y1++) {
                    for (int x1 = 0; x1 < 3; x1++) {
                        smooth_image->setPixelColor(x+x1, y+y1, color);
                    }
                }
#endif
            }
        }
    }
//#else
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int x1 = x;
            int x2= x+1;
            int y1 = y;
            int y2 = y+1;
            int x3 = x-1;
            int y3 = y-1;

            if (x2 >= w) x2 = w -1;
            if (y2 >= h) y2 = h -1;
            if (x3 < 0) x3 = 0;
            if (y3 < 0) y3 = 0;
            QColor color;
            color.setRed((v_image->pixelColor(x1, y1).red()
                          + v_image->pixelColor(x1,y2).red()
                          + v_image->pixelColor(x1, y3).red()
                          + v_image->pixelColor(x2, y1).red()
                          + v_image->pixelColor(x2, y2).red()
                          + v_image->pixelColor(x2, y3).red()
                          + v_image->pixelColor(x3,y1).red()
                          + v_image->pixelColor(x3, y2).red()
                          + v_image->pixelColor(x3, y3).red()) / 9);
            color.setGreen((v_image->pixelColor(x1, y1).green()
                            + v_image->pixelColor(x1,y2).green()
                            + v_image->pixelColor(x1, y3).green()
                            + v_image->pixelColor(x2, y1).green()
                            + v_image->pixelColor(x2, y2).green()
                            + v_image->pixelColor(x2, y3).green()
                            + v_image->pixelColor(x3,y1).green()
                            + v_image->pixelColor(x3, y2).green()
                            + v_image->pixelColor(x3, y3).green()) / 9);
            color.setBlue((v_image->pixelColor(x1, y1).blue()
                           + v_image->pixelColor(x1,y2).blue()
                           + v_image->pixelColor(x1, y3).blue()
                           + v_image->pixelColor(x2, y1).blue()
                           + v_image->pixelColor(x2, y2).blue()
                           + v_image->pixelColor(x2, y3).blue()
                           + v_image->pixelColor(x3,y1).blue()
                           + v_image->pixelColor(x3, y2).blue()
                           + v_image->pixelColor(x3, y3).blue()) / 9);
            smooth_image->setPixelColor(x, y, color);
        }
    }
#endif

    //according to the optical center point. we need do strection.
    const float strection_w_ratio               = M_PI_2;
    const float strection_h_ratio               = M_PI_2 * 0.8;
    const int strection_width                   = AlignTo(w * strection_w_ratio * 2, 2);
    const int strection_height                  = AlignTo(h * strection_h_ratio * 2, 2);
    const int ocw                               = w / 2;
    const int och                               = h / 2;
    const int ocsw                              = strection_width / 2;
    const int ocsh                              = strection_height / 2;

    qDebug("wxh=(%d, %d), strech wxh=(%d, %d)", w, h, strection_width, strection_height);
    QImage strection(strection_width, strection_height, QImage::Format_RGB888);
    for (int y = ocsh; y > 0; --y)
    {
        int y1 = MyMin(h-1, qPow(y / (float)ocsh, 4.0) * och);
        for (int x = ocsw; x > 0; --x)
        {
            int x1 = MyMin(w-1, qPow(x / (float)ocsw, 4.0) * ocw);

            strection.setPixel(x, y, smooth_image->pixel(x1, y1));

            strection.setPixel(2 * ocsw - x, y, smooth_image->pixel(2 * ocw - 1 - x1, y1));

            strection.setPixel(x, 2 * ocsh - y, smooth_image->pixel(x1, 2 * och - 1 - y1));

            strection.setPixel(2 * ocsw - x, 2 * ocsh - y, smooth_image->pixel(2 * ocw - 1 -x1, 2 * och - 1 - y1));
        }
    }
    *strecth_image = strection.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

int FisheyeDistortionCorrection::AlignTo(int value, int k)
{
    int delat  = value % k;
    if (delat == 0) return value;
    return value + k - delat;
}

QImage FisheyeDistortionCorrection::GenerateSampleImage(int width, int height)
{
    QImage sample(width, height, QImage::Format_RGB888);

    const int width_max    = static_cast<int>(pow(2, 12));
    const int height_max   = static_cast<int>(pow(2, 12));
    if (width > width_max || height > height_max)
    {
        qDebug("image size is too big. current size: %dx%d, max support size:%dx%d",
               width, height, width_max, height_max);
        return sample;
    }
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            /**
             * @brief rgbValue will store the x, y value for the low 24-bit. 12 bit for x, 12 bit for y.
             * so, here, if width > 2^12 or height > 2^12, this method will not valid.
             */
            QRgb rgbValue = (0xffu << 24) | ((x & 0xfffu) << 12) | (y & 0xfffu);
            sample.setPixel(x, y, rgbValue);
        }
    }
    return sample;
}

QDataStream &operator<<(QDataStream &out, const CorrectionBinData_t &binData) {
    out << binData.magic_number
        << binData.major_version
        << binData.minor_version
        << binData.width_in
        << binData.height_in
        << binData.width_out
        << binData.height_out;
    return out;

}

QDataStream &operator>>(QDataStream &in, CorrectionBinData_t &binData) {
    in  >> binData.magic_number
        >> binData.major_version
        >> binData.minor_version
        >> binData.width_in
        >> binData.height_in
        >> binData.width_out
        >> binData.height_out;
    return in;
}
void FisheyeDistortionCorrection::GenerateMappingFileBin(
        QString path,
        QImage *final,
        int width_in,
        int height_in)
{

    int width_out   = final->width();
    int height_out  = final->height();
    QPoint **mapping;
    Create2DArray(mapping, height_out, width_out);
    for (int y = 0; y < height_out; y++)
    {
        for (int x = 0; x < width_out; x++)
        {
            QRgb rgb = final->pixel(x, y);
            mapping[y][x].setX((rgb>>12) & (0xfff));
            mapping[y][x].setY(rgb & 0xfff);
        }
    }
    //
    QFile file(path);
    file.open(QIODevice::WriteOnly|QIODevice::Truncate);
    QDataStream out(&file);

    QString version("Lens Distortion Correction. Version: 1.0");

    CorrectionBinData_t binData;
    binData.magic_number    = 0x44444;
    binData.major_version   = 1;
    binData.minor_version   = 0;
    binData.width_in        = width_in;
    binData.height_in       = height_in;
    binData.width_out       = width_out;
    binData.height_out      = height_out;
    binData.ppMap           = reinterpret_cast<void **>(mapping);

    out << binData;

    for (int y = 0; y < height_out; y++)
    {
        for (int x = 0; x < width_out; x++)
        {
            out << qint16(mapping[y][x].x());
            out << qint16(mapping[y][x].y());
        }
    }

    out.commitTransaction();
    file.flush();
    file.close();
    Destroy2DArray(mapping, height_out);
}

QImage  FisheyeDistortionCorrection::GetImageByBinData(QString path, QImage *input)
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    CorrectionBinData_t binData;

    in >> binData;

    qDebug("magic_number = %d, major_version = %d, minor_version = %d, width_in = %d, height_in = %d, width_out = %d, height_out=%",
           binData.magic_number,
           binData.major_version,
           binData.minor_version,
           binData.width_in,
           binData.height_in,
           binData.width_out,
           binData.height_out);
    // check bin file validation.
    Q_ASSERT(binData.magic_number == 0x44444);
    Q_ASSERT(binData.major_version == 1);
    Q_ASSERT(binData.minor_version == 0);


    if (binData.width_out > 0 && binData.height_out > 0)
    {
        QImage output(binData.width_out, binData.height_out, QImage::Format_RGB888);
        qint16 x_input = 0, y_input = 0;
        for (int y = 0; y < binData.height_out; y++)
        {
            for (int x = 0; x < binData.width_out; x++)
            {
                in >> x_input >> y_input;
                output.setPixel(x, y, input->pixel(x_input, y_input));
            }
        }
        return output;
    }
    return QImage();
}
