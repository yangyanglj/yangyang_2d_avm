//利用扫描线逼近算法标定鱼眼图像的中心和半径，及标准圆变换
//标准圆变换部分，坐标原点设置的不好，没有找到在OPENCV中如何将图像坐标原点设置在图像中心

#include "ScanningMethod.h"

#include"cv.h"
#include"cxcore.h"



/****************************************************************
*作用		标定鱼眼图像的中心和半径
*参数
	img		输入图像
	x0		【输出】中心X坐标
	y0		【输出】中心Y坐标
	r		【输出】半径
*返回		剪切后的鱼眼图像
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

	//寻找鱼眼图像区域左切线
	for(int i = 0; i < img->width; i++)
	{
		ExtremeCol( img, &n_max, &n_min, i );
		if(( n_max - n_min ) > GRAY_DIFF_THR )
		{
			n_left = i;
			break;
		}
	}

	//寻找鱼眼图像区域右切线
	for( int i = img->width - 1; i >= 0; i--)
	{

		ExtremeCol( img, &n_max, &n_min, i );
		if(( n_max - n_min ) > GRAY_DIFF_THR )
		{
			n_right = i;
			break;
		}		
	}

	//寻找鱼眼图像区域上切线
	for(int i = 0; i < img->height; i++)
	{

		ExtremeRow(img, &n_max, &n_min, i );
		if(( n_max - n_min ) > GRAY_DIFF_THR )
		{
			n_top = i;
			break;
		}
	}

	//寻找鱼眼图像区域下切线
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

		//计算鱼眼图像中心坐标和半径
		*x0 = 1.0 * ( n_left + n_right ) / 2;
		*y0 = 1.0 * ( n_top + n_bottom ) / 2;
		float r1 = 1.0 * ( n_right - n_left ) / 2;
		float r2 = 1.0 * ( n_bottom - n_top ) / 2;
		*r = max( r1, r2 );
		return imgCircle;

	}
}

/*******************************************************************
*作用		寻找图像某行的极值，包括最大值和最小值
*参数
	img		输入图像
	maxRow	【输出】行最大值
	minRow	【输出】行最小值
	row		输入查找极值的行数
*******************************************************************/

void ExtremeRow( IplImage* img, float* maxRow, float* minRow, int row)
{
	float maxTemp;
	float minTemp;

	float n_temp;
	maxTemp = RGB_to_gray( img, 0, row );			//将最大值预设为该行第一个像素
	minTemp = RGB_to_gray( img, 0, row );			//将最小值预设为该行第一个像素

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
*作用		寻找图像某列的极值，包括最大值和最小值
*参数
	img		输入图像
	maxRow	【输出】列最大值
	minRow	【输出】列最小值
	col		输入查找极值的列数
*******************************************************************/

void ExtremeCol(IplImage* img, float* maxCol, float* minCol, int col )
{
	float maxTemp;
	float minTemp;

	float n_temp;
	maxTemp = RGB_to_gray( img, col, 0 );			//将最大值预设为该列第一个像素
	minTemp = RGB_to_gray( img, col, 0 );			//将最小值预设为该列第一个像素

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
*作用		用公式 I=0.59*r+0.11*g+0.3*b 计算像素的亮度
*参数
	img		输入图像
	x		像素的行坐标
	y		像素的列坐标
*放回		像素亮度I
**********************************************************/
float RGB_to_gray( IplImage* img, int x, int y )
{
	float n_pixel;
	n_pixel = 0.59 * ((uchar*)(img->imageData + img->widthStep*y))[x*3+2]
			  + 0.11 * ((uchar*)(img->imageData + img->widthStep*y))[x*3+1]
			  + 0.3 * ((uchar*)(img->imageData + img->widthStep*y))[x*3];

	return n_pixel;

}



//将椭圆变换为标准圆
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