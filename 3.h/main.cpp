#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

int main()
{
	//读取图片
	Mat Src = imread("BACK_0.bmp") ;
	//imshow("Src",Src);
	cout<< Src.size()<<endl;
	int nbottom = 0;
	int ntop = 0;
	int nright = 0;
	int nleft = 0;
 
	//遍历寻找上边界
	int nflag = 0;
	for (int i=0 ;i< Src.rows -1;i++)
	{
		for (int j=0; j<Src.cols -1; j++)
		{
			uchar I = 0.59*Src.at<Vec3b>(i,j)[0] + 0.11*Src.at<Vec3b>(i,j)[1] + 0.3*Src.at<Vec3b>(i,j)[2];
			if (I > 20)
			{
				I = 0.59*Src.at<Vec3b>(i+1,j)[0] + 0.11*Src.at<Vec3b>(i+1,j)[1] + 0.3*Src.at<Vec3b>(i+1,j)[2];
				if (I > 20)
				{
					ntop = i;
					nflag = 1;
					break;
				}
			}
		}
		if (nflag ==1)
		{
			break;
		}
	}
	//遍历寻找下边界
	nflag = 0;
	for (int i= Src.rows-1;i > 1;i--)
	{
		for (int j=0; j < Src.cols -1; j++)
		{
			uchar I = 0.59*Src.at<Vec3b>(i,j)[0] + 0.11*Src.at<Vec3b>(i,j)[1] + 0.3*Src.at<Vec3b>(i,j)[2];
			if (I > 20)
			{
				I = 0.59*Src.at<Vec3b>(i-1,j)[0] + 0.11*Src.at<Vec3b>(i-1,j)[1] + 0.3*Src.at<Vec3b>(i-1,j)[2];
				if (I > 20)
				{
					nbottom = i;
					nflag = 1;
					break;
				}
			}
		}
		if (nflag ==1)
		{
			break;
		}
	}
	//遍历寻找左边界
	nflag = 0;
	for (int j=0; j<Src.cols -1; j++)
	{
		for (int i=0 ;i< Src.rows ;i++)
		{
			uchar I = 0.59*Src.at<Vec3b>(i,j)[0] + 0.11*Src.at<Vec3b>(i,j)[1] + 0.3*Src.at<Vec3b>(i,j)[2];
			if (I > 20)
			{
				I = 0.59*Src.at<Vec3b>(i,j+1)[0] + 0.11*Src.at<Vec3b>(i,j+1)[1] + 0.3*Src.at<Vec3b>(i,j+1)[2];
				if (I > 20)
				{
					nleft = j;
					nflag = 1;
					break;
				}
			}
		}
		if (nflag ==1)
		{
			break;
		}
	}
	//遍历寻找右边界
	nflag = 0;
	for (int j=Src.cols -1; j >0; j--)
	{
		for (int i= 0;i <Src.rows ;i++)
		{
			uchar I = 0.59*Src.at<Vec3b>(i,j)[0] + 0.11*Src.at<Vec3b>(i,j)[1] + 0.3*Src.at<Vec3b>(i,j)[2];
			if (I > 20)
			{
				I = 0.59*Src.at<Vec3b>(i,j-1)[0] + 0.11*Src.at<Vec3b>(i,j-1)[1] + 0.3*Src.at<Vec3b>(i,j-1)[2];
				if (I > 20)
				{
					nright = j;
					nflag = 1;
					break;
				}
			}
		}
		if (nflag ==1)
		{
			break;
		}
	}
	cout<< ntop<<endl;
	cout<< nbottom<<endl;
	cout<< nleft << endl;
	cout<< nright <<endl;
 
	//根据边界值来获得直径
	int d = min(nright-nleft,nbottom-ntop);
 
	Mat imgRoi ;
	imgRoi = Src(Rect( nleft, ntop, d, d ));
	imwrite("aa.jpg", imgRoi);
 
	Mat dst( imgRoi.size(), CV_8UC3, Scalar(255,255,255));
 
	//建立映射表
	Mat map_x,map_y;
	map_x.create( imgRoi.size(), CV_32FC1 );
	map_y.create( imgRoi.size(), CV_32FC1 );
	for (int j=0; j< d-1;j++)
	{
		for (int i=0; i< d-1; i++ )
		{
			map_x.at<float>(i,j) = static_cast<float>( d/2.0 + i/2.0*cos(1.0*j/d*2*CV_PI));
			map_y.at<float>(i,j) = static_cast<float>( d/2.0 + i/2.0*sin(1.0*j/d*2*CV_PI));
		}
	}
 
	remap(imgRoi, dst, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0,0));
	//重设大小
	resize(dst, dst, Size(), 2.0, 1.0);
 
	imwrite("bb.jpg",dst);
	waitKey();
	return 0;
}
