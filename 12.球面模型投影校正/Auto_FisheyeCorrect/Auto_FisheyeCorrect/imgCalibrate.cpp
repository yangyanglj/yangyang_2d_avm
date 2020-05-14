//使用球面透视投影模型法校正鱼眼图像畸变

#include"ScanningMethod.h"
#include"imgCalibrate.h"
#include"cv.h"
#include"cxcore.h"
#include<math.h>


/***********************************************
*作用		使用球面透视投影模型算法校正鱼眼图像
*参数
	img		输入图像
*放回		校正后的图像
***********************************************/

IplImage* SPP( IplImage* img)
{
	CvPoint2D32f UV0;

	float x0;//鱼眼图像UV平面的原点
	float y0;
	float r;					//鱼眼图像半径r

	IplImage* imgCalib = cvCreateImage( cvSize( VIEW_PLANE_WIDTH, VIEW_PLANE_HEIGHT),IPL_DEPTH_8U, 3);		//创建校正后图像大小

	IplImage* imgCircle = ScanningMethod( img, &x0, &y0, &r);		//标定鱼眼图像的中心和半径，并返回剪切后的图像

	UV0.x = 0;
	UV0.y = 0;

	CvPoint2D32f XY0 =UV_to_XY(UV0,r);


	for(int j=0; j<VIEW_PLANE_HEIGHT; j++)
		for( int i=0; i<VIEW_PLANE_WIDTH;i++)
		{
			CvPoint2D32f xy;
			CvPoint2D32f uv;
			xy.x = i - 1.0*VIEW_PLANE_WIDTH/2 + XY0.x;
			xy.y = 1.0*VIEW_PLANE_HEIGHT/2 - j - XY0.y;
			
			uv = XY_to_UV(xy,r);

			uv.x=uv.x+imgCircle->width/2;
			uv.y=imgCircle->height/2-uv.y;

			int iu = (int)uv.x;
			int iv = (int)uv.y;

			CvPoint3D32f f1;
			CvPoint3D32f f2;

			//双线性插值法
			f1.x = ((uchar*)(imgCircle->imageData + imgCircle->widthStep*iv))[iu*3] * (1-abs(uv.x-iu))+
				((uchar*)(imgCircle->imageData + imgCircle->widthStep*iv))[(iu+1)*3] * (1-abs(uv.x-(iu+1)));

			f1.y = ((uchar*)(imgCircle->imageData + imgCircle->widthStep*iv))[iu*3+1] * (1-abs(uv.x-iu))+
				((uchar*)(imgCircle->imageData + imgCircle->widthStep*iv))[(iu+1)*3+1] * (1-abs(uv.x-(iu+1)));

			f1.z = ((uchar*)(imgCircle->imageData + imgCircle->widthStep*iv))[iu*3+2] * (1-abs(uv.x-iu))+
				((uchar*)(imgCircle->imageData + imgCircle->widthStep*iv))[(iu+1)*3+2] * (1-abs(uv.x-(iu+1)));




			f2.x = ((uchar*)(imgCircle->imageData + imgCircle->widthStep*(iv+1)))[iu*3] * (1-abs(uv.x-iu))+
				((uchar*)(imgCircle->imageData + imgCircle->widthStep*(iv+1)))[(iu+1)*3] * (1-abs(uv.x-(iu+1)));

			f2.y = ((uchar*)(imgCircle->imageData + imgCircle->widthStep*(iv+1)))[iu*3+1] * (1-abs(uv.x-iu))+
				((uchar*)(imgCircle->imageData + imgCircle->widthStep*(iv+1)))[(iu+1)*3+1] * (1-abs(uv.x-(iu+1)));

			f2.z = ((uchar*)(imgCircle->imageData + imgCircle->widthStep*(iv+1)))[iu*3+2] * (1-abs(uv.x-iu))+
				((uchar*)(imgCircle->imageData + imgCircle->widthStep*(iv+1)))[(iu+1)*3+2] * (1-abs(uv.x-(iu+1)));




			((uchar*)(imgCalib->imageData + imgCalib->widthStep*j))[i*3] = f1.x*(1-abs(uv.y-iv))+f2.x*(abs(uv.y-iv));
			((uchar*)(imgCalib->imageData + imgCalib->widthStep*j))[i*3+1] = f1.y*(1-abs(uv.y-iv))+f2.y*(abs(uv.y-iv));
			((uchar*)(imgCalib->imageData + imgCalib->widthStep*j))[i*3+2] = f1.z*(1-abs(uv.y-iv))+f2.z*(abs(uv.y-iv));


		}

		cvReleaseImage(&imgCircle);
		return imgCalib;
	
}



/******************************************************
*作用：		由鱼眼图像坐标（u，v）计算校正后图像对应坐标（x,y)
*参数
	uv		输入鱼眼图像坐标(u,v)
	r		鱼眼图像半径
*放回		计算得到的(x,y)坐标

*球面透视投影方法----法只适用于圆形区域的鱼眼图像，并且要先标
定出鱼眼图像的变换球面半径以及光学中心。

*************************************************/
CvPoint2D32f UV_to_XY(CvPoint2D32f uv,float r)
{
 
	CvPoint2D32f xy;
	xy.x = 1.0*Z0*uv.x/sqrt(r*r-uv.x*uv.x-uv.y*uv.y);
	xy.y = 1.0*Z0*uv.y/sqrt(r*r-uv.x*uv.x-uv.y*uv.y);
	
	return xy;

}


/******************************************************
*作用：		由校正后图像坐标（x,y)计算鱼眼图像对应坐标（u，v）
*参数
	uv		输入校正后图像坐标(x,y)
	r		鱼眼图像半径
*放回		计算得到的(u,v)坐标
*************************************************/
CvPoint2D32f XY_to_UV(CvPoint2D32f xy, float r)
{
	CvPoint2D32f uv;

	uv.x = r*xy.x/sqrt(xy.x*xy.x+xy.y*xy.y+Z0*Z0);
	uv.y = r*xy.y/sqrt(xy.x*xy.x+xy.y*xy.y+Z0*Z0);

	return uv;

}