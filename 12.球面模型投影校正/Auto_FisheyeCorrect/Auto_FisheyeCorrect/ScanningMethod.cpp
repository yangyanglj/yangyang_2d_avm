//����ɨ���߱ƽ��㷨�궨����ͼ������ĺͰ뾶������׼Բ�任
//��׼Բ�任���֣�����ԭ�����õĲ��ã�û���ҵ���OPENCV����ν�ͼ������ԭ��������ͼ������

#include "ScanningMethod.h"

#include"cv.h"
#include"cxcore.h"



/****************************************************************
*����		�궨����ͼ������ĺͰ뾶
*����
	img		����ͼ��
	x0		�����������X����
	y0		�����������Y����
	r		��������뾶
*����		���к������ͼ��
****************************************************************/

IplImage* ScanningMethod( IplImage* img, float* x0, float* y0, float* r)
{
	IplImage* imgTemp;
	int n_left;
	int n_right;
	int n_top;
	int n_bottom;

	float n_max;
	float n_min;

	//Ѱ������ͼ������������
	for(int i = 0; i < img->width; i++)
	{
		ExtremeCol( img, &n_max, &n_min, i );
		if(( n_max - n_min ) > GRAY_DIFF_THR )
		{
			n_left = i;
			break;
		}
	}

	//Ѱ������ͼ������������
	for( int i = img->width - 1; i >= 0; i--)
	{

		ExtremeCol( img, &n_max, &n_min, i );
		if(( n_max - n_min ) > GRAY_DIFF_THR )
		{
			n_right = i;
			break;
		}		
	}

	//Ѱ������ͼ������������
	for(int i = 0; i < img->height; i++)
	{

		ExtremeRow(img, &n_max, &n_min, i );
		if(( n_max - n_min ) > GRAY_DIFF_THR )
		{
			n_top = i;
			break;
		}
	}

	//Ѱ������ͼ������������
	for( int i = img->height - 1; i >= 0; i--)
	{

		ExtremeRow(img, &n_max, &n_min, i );
		if(( n_max - n_min ) > GRAY_DIFF_THR )
		{
			n_bottom = i;
			break;
		}
	}

	if( n_left >= n_right ||  n_top >=n_bottom )
	{	
		return NULL;
	}
	else
	{
		
		imgTemp = cvCreateImage( cvSize( n_right - n_left, n_bottom - n_top ), IPL_DEPTH_8U, 3 );
		cvZero( imgTemp );

		cvSetImageROI( img, cvRect( n_left, n_top, n_right - n_left, n_bottom - n_top ));
		cvAdd( imgTemp, img, imgTemp, NULL );

		IplImage* imgCircle = Standard_circle( imgTemp );

		cvReleaseImage(&imgTemp);

		//��������ͼ����������Ͱ뾶
		*x0 = 1.0 * ( n_left + n_right ) / 2;
		*y0 = 1.0 * ( n_top + n_bottom ) / 2;
		float r1 = 1.0 * ( n_right - n_left ) / 2;
		float r2 = 1.0 * ( n_bottom - n_top ) / 2;
		*r = max( r1, r2 );
		return imgCircle;

	}
}

/*******************************************************************
*����		Ѱ��ͼ��ĳ�еļ�ֵ���������ֵ����Сֵ
*����
	img		����ͼ��
	maxRow	������������ֵ
	minRow	�����������Сֵ
	row		������Ҽ�ֵ������
*******************************************************************/

void ExtremeRow( IplImage* img, float* maxRow, float* minRow, int row)
{
	float maxTemp;
	float minTemp;

	float n_temp;
	maxTemp = RGB_to_gray( img, 0, row );			//�����ֵԤ��Ϊ���е�һ������
	minTemp = RGB_to_gray( img, 0, row );			//����СֵԤ��Ϊ���е�һ������

	for( int i = 0; i < img->width; i++ )
	{
		n_temp = RGB_to_gray( img, i, row );
		if( maxTemp < n_temp )
			maxTemp = n_temp;
		if( minTemp > n_temp )
			minTemp = n_temp;
	}

	*maxRow = maxTemp;
	*minRow = minTemp;

}



/*******************************************************************
*����		Ѱ��ͼ��ĳ�еļ�ֵ���������ֵ����Сֵ
*����
	img		����ͼ��
	maxRow	������������ֵ
	minRow	�����������Сֵ
	col		������Ҽ�ֵ������
*******************************************************************/

void ExtremeCol(IplImage* img, float* maxCol, float* minCol, int col )
{
	float maxTemp;
	float minTemp;

	float n_temp;
	maxTemp = RGB_to_gray( img, col, 0 );			//�����ֵԤ��Ϊ���е�һ������
	minTemp = RGB_to_gray( img, col, 0 );			//����СֵԤ��Ϊ���е�һ������

	for( int i = 0; i < img->height; i++ )
	{
		n_temp = RGB_to_gray( img, col, i );
		if( maxTemp < n_temp )
			maxTemp = n_temp;
		if( minTemp > n_temp )
			minTemp = n_temp;
	}

	*maxCol = maxTemp;
	*minCol = minTemp;

}


/**********************************************************
*����		�ù�ʽ I=0.59*r+0.11*g+0.3*b �������ص�����
*����
	img		����ͼ��
	x		���ص�������
	y		���ص�������
*�Ż�		��������I
**********************************************************/
float RGB_to_gray( IplImage* img, int x, int y )
{
	float n_pixel;
	n_pixel = 0.59 * ((uchar*)(img->imageData + img->widthStep*y))[x*3+2]
			  + 0.11 * ((uchar*)(img->imageData + img->widthStep*y))[x*3+1]
			  + 0.3 * ((uchar*)(img->imageData + img->widthStep*y))[x*3];

	return n_pixel;

}



//����Բ�任Ϊ��׼Բ
IplImage* Standard_circle(IplImage* img)
{
	float u =1.0*img->width /2;
	float v = 1.0*img->height/ 2;
	float beta  = 1.0 * img->width / img->height;
	IplImage* imgOut;
	if( img->width < img->height )

		imgOut = cvCreateImage( cvSize( min( img->width, img->height ), min( img->width, img->height)), IPL_DEPTH_8U, 3);
	else
		imgOut = cvCreateImage( cvSize( max( img->width, img->height ), max( img->width, img->height)), IPL_DEPTH_8U, 3);
	
	float a[] = { 1.0, 0, 0,
				0, beta, 0,
				0, 0, 1.0 };
				

	CvMat* K = cvCreateMat( 3,3,CV_32FC1);
	cvSetData(K,a,K->step);


	cvWarpPerspective( img, imgOut, K, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll( 255 ));


	cvReleaseMat(&K);

	return imgOut;

}