//主函数，显示校正后图像


#include "ScanningMethod.h"
#include"imgCalibrate.h"
#include"cv.h"
#include"cxcore.h"
#include"highgui.h"

#include<iostream>
using namespace std;

#define IMG_MAXNO 13 
#define IMG_DIR           "raw_image\\raw_"
#define CORRECT_IMG_FILETYP  ".jpg"
#define CORRECT_IMG_DIR      "correct_Image\\correct_Img_"

int main( int argc, char** argv )
{
	char strNumber[10],strImg[60], strCorrectImg[60];
	int iImgNo;
	
	for (iImgNo=1; iImgNo <= IMG_MAXNO; iImgNo++)
	{
		//拼接成相应的图片raw_image目录
		strcpy(strImg,IMG_DIR);
		itoa(iImgNo,strNumber,10);//把整数转换为字符串
		strcat(strImg,strNumber);
		strcat(strImg,CORRECT_IMG_FILETYP);
		cout<<strImg<<endl;

		//拼接成相应的图片correct_image目录
		strcpy(strCorrectImg,CORRECT_IMG_DIR);
		strcat(strCorrectImg,strNumber);
		strcat(strCorrectImg,CORRECT_IMG_FILETYP);
		//cout<<strCorrectImg<<endl;
		
		IplImage* pImg;
		if( (pImg = cvLoadImage( strImg, CV_LOAD_IMAGE_ANYCOLOR ) ) == 0 )
		{
			cout<<"没读出图片"<<endl;
			getchar();
			return 0;
		}

		IplImage* imgCalib = SPP( pImg);

		cvNamedWindow( "---------------校 正 图---------------", 1 );
		cvNamedWindow( "-------------------原 图-----------------", 1 );
		cvShowImage( "---------------校 正 图---------------", imgCalib );
		cvShowImage( "-------------------原 图-----------------", pImg );
		cvSaveImage(strCorrectImg,imgCalib);//将校正好的图片保存到相应的目录下
		cvWaitKey(0);
		cvDestroyWindow( "---------------校 正 图---------------" );
		cvDestroyWindow( "-------------------原 图-----------------" );
		cvReleaseImage( &imgCalib );
		cvReleaseImage( &pImg );
	}

	cout<<"........图片校正完毕！........."<<endl;
	return 0;
}