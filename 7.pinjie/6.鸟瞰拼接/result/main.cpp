#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	//读入四幅图片
	Mat image_1 = imread("result.jpg");
	
	cvtColor(image_1,image_1,CV_BGR2GRAY);
	//imshow("xx",image_4);
	waitKey(0);
	//创建连接后存入的图像
	//Mat result(image_1.size(),CV_8UC3,Scalar(255,255,255));
	Mat result(720,1280,CV_8UC1);
	for(int i = 0;i <result.rows;i++ )
	{
	   //&& (j < (float)((i-p4)/k4) ) 
	   for(int j = 0;j <result.cols;j++ )
	   {
		
		result.at<uchar>(i,j)=0;
	   }

         }
	
#if 1//top
	Point a1,a3,a8,a6;
	a1.x=468;a1.y=327;
	a3.x=509;a3.y=349;
	a8.x=644;a8.y=349;
	a6.x=685;a6.y=327;
#endif
#if 0
	Point a1,a3,a8,a6;
	a1.x=454;a1.y=312;
	a3.x=502;a3.y=336;
	a8.x=654;a8.y=336;
	a6.x=712;a6.y=309;
#endif


#if 1//left
	Point b2,b4;
	b2.x=509;
	b2.y=489;
	b4.x=468;
	b4.y=510;
#endif

#if 1 //black
	Point b5,b7;
	b5.x = 644;
	b5.y = 489;
	b7.x = 685;
	b7.y = 510;
#endif

#if 1//right
	Point r6,r8,r5,r7;
	r6.x = 797;r6.y=312;
	r8.x = 729;r8.y=349;
	r5.x=758;r5.y=521;
	r7.x=881;r7.y=602;
	
#endif
	float k3 = (float) ( a3.y - a1.y) / (a3.x - a1.x) ;//图像坐标系反的，向下为正
	float k4 = -(float) (a8.y - a6.y ) / (a6.x - a8.x) ;

	float k4_2 = (float)(b4.y - b2.y) / (b4.x - b2.x);//不要自己加正负
 	float k5_7 = (float)(b7.y - b5.y) / (b7.x - b5.x);

	float kr6_8 = -(float) (r8.y - r6.y ) / (r6.x - r8.x) ;
	float kr5_7 = (float)(r7.y - r5.y) / (r7.x - r5.x);

	float p3 = (a1.y - k3*a1.x);//截距 这里之前写错了导致负号
        float p4 = (a6.y - k4*a6.x);
	float p4_2 = (b4.y - k4_2*b4.x);
	float p5_7 = (b7.y - k5_7*b7.x);
	float pr6_8 = (r8.y - kr6_8*r8.x);
	float pr5_7 = (r7.y - kr5_7*r7.x);

	
	cout<<"p4:"<<k5_7<<" k4_2::"<<p5_7<<endl;

	Point t1,t2;//上半区域截止
	t1.x=527;t1.y=366;
	t2.x=627;t2.y=366;
#if 0
	for(int i = 0;i <result.rows;i++ )
	{
	   //&& (j < (float)((i-p4)/k4) ) 
	   for(int j = 0;j <result.cols;j++ )
	   {
#if 1
		if(i < 366 && (j > (float)((i-p3)/k3 ))   && (j < (float)((i-p4)/k4) ))
		result.at<uchar>(i,j)=image_4.at<uchar>(i,j);
		//负斜率　大于小于　　上下方颠倒
		if(j < (float)((i-p4_2)/k4_2 ) && j < (float)((i-p3)/k3 ) && j <527 )
		{
		//result.at<uchar>(i,j)=255;
		result.at<uchar>(i,j)=image_1.at<uchar>(i,j);
		}
#endif
		//j > (float)((i-p4_2)/k4_2 ) && i > 454 &&
		if( j > (float)((i-p4_2)/k4_2 ) && i > 460 && j < (float)((i-p5_7)/k5_7 ) )
		{
		//result.at<uchar>(i,j)=255;
		result.at<uchar>(i,j)=image_3.at<uchar>(i,j);
		}
		//j > (float)((i-p5_7)/k5_7 )
		//if( j > 627 && j > (float)((i-pr6_8)/kr6_8) && j > (float)((i-pr5_7)/kr5_7 ) )
		if( j > 627 && (j > (float)((i-p4)/k4) ) && j > (float)((i-p5_7)/k5_7 ) )
		{
		    result.at<uchar>(i,j)=image_2.at<uchar>(i,j);
		}
	   }

         }
#endif
#if 0
	Mat mask;
	Rect r1(0,0,result.cols,result.rows/3);
	mask = Mat::zeros(result.size(),CV_8UC1);
	mask(r1).setTo(255);
#endif
	//显示四幅图像连接后的图像
	imshow("result", result);
	imwrite("result.jpg",result);
	waitKey(0);

	return 0;
}

