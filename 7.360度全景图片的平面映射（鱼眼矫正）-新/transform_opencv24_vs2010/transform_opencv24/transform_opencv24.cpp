#include <iostream>
#include <string>  
#include <io.h>
#include <math.h>
#include <fstream>
#include <opencv2/opencv.hpp> 
#include "opencv2/stitching/stitcher.hpp"
#include <opencv2/highgui/highgui.hpp>  

using namespace std;  
using namespace cv;  
float r=1304;

float change(int x,int y,float xDim,float z)
{
	float tt=(xDim-x)/z;
//	float l=atan(xDim/z)-atan(tt);
	float l=120*3.1415926/360-atan(tt);
	float result=l*r;
	return result;	
}


float change1(int x,int y,float xDim,float yDim,float z)
{
	float tt=(yDim-y)/(sqrt((xDim-x)*(xDim-x)+z*z));
	float l=atan(tt);
	float result=2048-l*r;
	return result;
}
float change2(int x,int y,float xDim,float yDim,float z)
{
	float tt1=(y-yDim)/(sqrt((xDim-x)*(xDim-x)+z*z));
//	float tt2=752.00/(sqrt((752.00-x)*(752.00-x)+565504.0000));
	float l=atan(tt1);
	float result=l*r+2048;
	return result;
}




int comeon(IplImage* srcImg,vector<float>& directions, float angle, int xDim, int yDim,vector<IplImage*>& results)  
{  
	Mat src(srcImg); 
//	int width=src.cols;  
//	int heigh=src.rows;  
//	RNG rng;
//	vector<Mat> result;
	
	Mat img(xDim+1,yDim+1,CV_8UC3);
	float z=xDim/(2*tan(angle*3.1415926/360));
	TickMeter tm;     
	tm.start(); 
	for(int i=0;i<directions.size();i++)
	{
		
		for (int y=0; y<yDim; y++)  
		{  

			uchar* P1  = img.ptr<uchar>(y); 
			uchar* P0  = src.ptr<uchar>(y);		
			for (int x=0; x<xDim; x++)  
			{
				float c;
				if(y<=(int)(yDim/2))
					c=change1(x,y,xDim*1.0/2.0,yDim*1.0/2.0,z);
				else
					c=change2(x,y,xDim*1.0/2.0,yDim*1.0/2.0,z);
				int d=(int)c;

				P0  = src.ptr<uchar>(c); 
			
				float a;
				a=change(x,y,xDim*1.0/2.0,z);
				a=a+directions[i];

				int   b=(int)a;
		

				float B=P0[3*b]*(1-a+b)+P0[3*(b+1)]*(a-b);  
				float G=P0[3*b+1]*(1-a+b)+P0[3*(b+1)+1]*(a-b);  
				float R=P0[3*b+2]*(1-a+b)+P0[3*(b+1)+2]*(a-b);  

				P0  = src.ptr<uchar>(c+1);
				float B1=P0[3*b]*(1-a+b)+P0[3*(b+1)]*(a-b);  
				float G1=P0[3*b+1]*(1-a+b)+P0[3*(b+1)+1]*(a-b);  
				float R1=P0[3*b+2]*(1-a+b)+P0[3*(b+1)+2]*(a-b);

				B=B*(1-c+d)+B1*(c-d);
				G=G*(1-c+d)+G1*(c-d);
				R=R*(1-c+d)+R1*(c-d);

				P1[3*x] = (uchar)B;  
				P1[3*x+1] = (uchar)G;  
				P1[3*x+2] = (uchar)R;
			}
		}

		IplImage *res;
		res =(_IplImage*) malloc(sizeof(_IplImage));
		*res=IplImage(img);

		IplImage* tempimg = (IplImage*)cvClone(res);
		results.push_back(tempimg);

	}
	tm.stop();  
    cout<<"process time="<<tm.getTimeMilli()<<endl; 
	return 1;
} 

void main(int argc, char** argv)
{

	IplImage* srcImg = cvLoadImage("1.jpg", 1);
	vector<float> v1;
	for(int i=0;i<8192;i=i+512)
		v1.push_back((float)i);

	
	vector<IplImage*> results1;
	IplImage *test;
	
  	comeon(srcImg,v1,130.0,1400,1200.0,results1);
	
	cvNamedWindow("asdf",1);
	for(int i = 0; i < (int)v1.size(); i++)
	{
		test = results1.at(i);
		cvShowImage("asdf",test);
		cvWaitKey(0);
//		cvSaveImage("results1.jpg",test,0);
 		
	}
}
