//ʹ������͸��ͶӰģ�ͷ�У������ͼ�����



#include"cv.h"
#include"cxcore.h"



//ͶӰƽ���
#define VIEW_PLANE_WIDTH 400

//ͶӰƽ���
#define VIEW_PLANE_HEIGHT 300



#define Z0 120


IplImage* SPP( IplImage* img);



CvPoint2D32f UV_to_XY(CvPoint2D32f uv,float r);

CvPoint2D32f XY_to_UV(CvPoint2D32f xy, float r);