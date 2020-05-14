//����������ʾУ����ͼ��


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
		//ƴ�ӳ���Ӧ��ͼƬraw_imageĿ¼
		strcpy(strImg,IMG_DIR);
		itoa(iImgNo,strNumber,10);//������ת��Ϊ�ַ���
		strcat(strImg,strNumber);
		strcat(strImg,CORRECT_IMG_FILETYP);
		cout<<strImg<<endl;

		//ƴ�ӳ���Ӧ��ͼƬcorrect_imageĿ¼
		strcpy(strCorrectImg,CORRECT_IMG_DIR);
		strcat(strCorrectImg,strNumber);
		strcat(strCorrectImg,CORRECT_IMG_FILETYP);
		//cout<<strCorrectImg<<endl;
		
		IplImage* pImg;
		if( (pImg = cvLoadImage( strImg, CV_LOAD_IMAGE_ANYCOLOR ) ) == 0 )
		{
			cout<<"û����ͼƬ"<<endl;
			getchar();
			return 0;
		}

		IplImage* imgCalib = SPP( pImg);

		cvNamedWindow( "---------------У �� ͼ---------------", 1 );
		cvNamedWindow( "-------------------ԭ ͼ-----------------", 1 );
		cvShowImage( "---------------У �� ͼ---------------", imgCalib );
		cvShowImage( "-------------------ԭ ͼ-----------------", pImg );
		cvSaveImage(strCorrectImg,imgCalib);//��У���õ�ͼƬ���浽��Ӧ��Ŀ¼��
		cvWaitKey(0);
		cvDestroyWindow( "---------------У �� ͼ---------------" );
		cvDestroyWindow( "-------------------ԭ ͼ-----------------" );
		cvReleaseImage( &imgCalib );
		cvReleaseImage( &pImg );
	}

	cout<<"........ͼƬУ����ϣ�........."<<endl;
	return 0;
}