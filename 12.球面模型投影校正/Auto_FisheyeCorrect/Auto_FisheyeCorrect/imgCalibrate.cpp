//ʹ������͸��ͶӰģ�ͷ�У������ͼ�����

#include"ScanningMethod.h"
#include"imgCalibrate.h"
#include"cv.h"
#include"cxcore.h"
#include<math.h>


/***********************************************
*����		ʹ������͸��ͶӰģ���㷨У������ͼ��
*����
	img		����ͼ��
*�Ż�		У�����ͼ��
***********************************************/

IplImage* SPP( IplImage* img)
{
	CvPoint2D32f UV0;

	float x0;//����ͼ��UVƽ���ԭ��
	float y0;
	float r;					//����ͼ��뾶r

	IplImage* imgCalib = cvCreateImage( cvSize( VIEW_PLANE_WIDTH, VIEW_PLANE_HEIGHT),IPL_DEPTH_8U, 3);		//����У����ͼ���С

	IplImage* imgCircle = ScanningMethod( img, &x0, &y0, &r);		//�궨����ͼ������ĺͰ뾶�������ؼ��к��ͼ��

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

			//˫���Բ�ֵ��
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
*���ã�		������ͼ�����꣨u��v������У����ͼ���Ӧ���꣨x,y)
*����
	uv		��������ͼ������(u,v)
	r		����ͼ��뾶
*�Ż�		����õ���(x,y)����

*����͸��ͶӰ����----��ֻ������Բ�����������ͼ�񣬲���Ҫ�ȱ�
��������ͼ��ı任����뾶�Լ���ѧ���ġ�

*************************************************/
CvPoint2D32f UV_to_XY(CvPoint2D32f uv,float r)
{
 
	CvPoint2D32f xy;
	xy.x = 1.0*Z0*uv.x/sqrt(r*r-uv.x*uv.x-uv.y*uv.y);
	xy.y = 1.0*Z0*uv.y/sqrt(r*r-uv.x*uv.x-uv.y*uv.y);
	
	return xy;

}


/******************************************************
*���ã�		��У����ͼ�����꣨x,y)��������ͼ���Ӧ���꣨u��v��
*����
	uv		����У����ͼ������(x,y)
	r		����ͼ��뾶
*�Ż�		����õ���(u,v)����
*************************************************/
CvPoint2D32f XY_to_UV(CvPoint2D32f xy, float r)
{
	CvPoint2D32f uv;

	uv.x = r*xy.x/sqrt(xy.x*xy.x+xy.y*xy.y+Z0*Z0);
	uv.y = r*xy.y/sqrt(xy.x*xy.x+xy.y*xy.y+Z0*Z0);

	return uv;

}