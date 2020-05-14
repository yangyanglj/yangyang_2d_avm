//利用扫描线逼近算法标定鱼眼图像的中心和半径


#include"cv.h"
#include"cxcore.h"




#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

//极限亮度差阈值
#define GRAY_DIFF_THR 40
 
IplImage* ScanningMethod( IplImage* img, float* x0, float* y0, float* r);
void ExtremeRow( IplImage* img, float* maxRow, float* minRow, int row);
void ExtremeCol(IplImage* img, float* maxCol, float* minCol, int col );
float RGB_to_gray( IplImage* img, int x, int y );
IplImage* Standard_circle(IplImage* img);