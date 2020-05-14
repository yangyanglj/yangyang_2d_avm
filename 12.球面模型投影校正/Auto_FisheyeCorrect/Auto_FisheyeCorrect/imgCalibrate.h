//使用球面透视投影模型法校正鱼眼图像畸变



#include"cv.h"
#include"cxcore.h"



//投影平面宽
#define VIEW_PLANE_WIDTH 400

//投影平面高
#define VIEW_PLANE_HEIGHT 300



#define Z0 120


IplImage* SPP( IplImage* img);



CvPoint2D32f UV_to_XY(CvPoint2D32f uv,float r);

CvPoint2D32f XY_to_UV(CvPoint2D32f xy, float r);