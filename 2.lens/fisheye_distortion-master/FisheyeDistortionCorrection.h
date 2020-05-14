#ifndef FisheyeDistortionCorrection_H
#define FisheyeDistortionCorrection_H

#include <QString>
#include <QImage>

typedef struct CorrectionBinData
{
    qint32 magic_number;
    qint32 major_version;
    qint32 minor_version;
    qint32 width_in;
    qint32 height_in;
    qint32 width_out;
    qint32 height_out;
    void** ppMap;
} CorrectionBinData_t;

class FisheyeDistortionCorrection
{
public:
    static  FisheyeDistortionCorrection * getInstance();
    void    SetPictureSize(int width, int height);
    void    SetFileLocation(QString path);
    void    SetOpticalCenterPoint(int x, int y);
    void    Set2rdCurveCoff(int hBase, int vBase);
    void    SetRotation(int rotation);
    void    SetCrop(int x, int y, int w, int h);
    void    Process(QImage *ori_image, QImage *h_image,
            QImage *v_image, QImage *smooth_image, QImage *strecth_image);
    void    Process1(QImage *oriImage, QImage *hImage,
            QImage *rotateImage, QImage *vImage, QImage *smoothImage, QImage *strecthImage);
    void    Process2(QImage *oriImage, QImage *hImage,
            QImage *rotateImage, QImage *vImage, QImage *smoothImage, QImage *strecthImage);
    void    Process3(QImage *oriImage, QImage *hImage,
            QImage *rotateImage, QImage *vImage, QImage *smoothImage, QImage *strecthImage);

    void    Process4(QImage *oriImage, QImage *hImage);
    
    void    Process5(QImage *oriImage, QImage *output);


    QImage  GetDefaultImage();
    QImage  DoImageRotate(QImage *image, int angleValue);

    QImage  GenerateSampleImage(int width, int height);
    void    GenerateMappingFileBin(QString path, QImage *final, int width_in, int height_in);

    QImage  GetImageByBinData(QString path, QImage *input);

    /*
     * GetArchLens() : get the arc length between the (x0, y0) and (x1, y1).
     * a, b, c are the two-order curve cofficients.
     * the first point : x0, y0.
     * the second point: x1, y1.
     **/
    int     GetArchLens(float a, float b, float c, int x0, int y0, int x1, int y1);

    float   GetArchLens(float a, float  b, float c, int x0, int x1);

    double  GetArchLensOfCircel(double a, double b, double r, int x);

    double  GetAngelOfTwoLines(double k1, double k2);

    int     AlignTo(int value, int k);

    template<typename T>
    void    Create2DArray(T **&array, int height, int width);

    template<typename T>
    void    Destroy2DArray(T **&array, int height);

    int     GetDistance(int x, int y, int x1, int y1);

    int     GetDistance2(int x, int y, int x1, int y1);

    int     MyMin(int a, int b)
    {
        return (a > b) ? b : a;
    }

    int     MyMax(int a, int b)
    {
        return (a > b) ? a : b;
    }

    int     Range(int value, int min, int max)
    {
        if ( value < min) return min;
        if ( value > max) return max;
        return value;
    }
private:
    FisheyeDistortionCorrection();
    void Initialize();

    QString     mFilePath;
    int         mWidth;
    int         mHeight;
    int         mOpticalCenterX;
    int         mOpticalCenterY;
    int         mRotation;
    int         mHorizontalBase;
    int         mVerticalBase;
    int         mCropX;
    int         mCropY;
    int         mCropW;
    int         mCropH;
};

#endif // FisheyeDistortionCorrection_H
