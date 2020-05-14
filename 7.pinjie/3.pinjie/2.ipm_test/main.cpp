#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
	//读入四幅图片
	Mat image_1 = imread("./left.jpg");
	Mat image_2 = imread("./right.jpg");
	Mat image_3 = imread("./black.jpg");
	Mat image_4 = imread("./top.jpg");

	//归一化为相同的大小：320*240
	Size sz = Size(320, 240);
	
	//创建连接后存入的图像
	//Mat result(image_1.size(),CV_8UC3,Scalar(255,255,255));
	Mat result(720,1280,CV_8UC1);

	float k1 = (float)result.rows/result.cols;
	float k2 = (float)(-result.rows)/result.cols;
	for(int i = 0;i <result.rows;i++ )
	{
	   
	   for(int j = 0;j <result.cols;j++ )
	   {
		if( (i ==result.rows/3 && j >= result.cols/3 && j<=result.cols/3*2)  || (i ==result.rows/3 *2 && j >= result.cols/3 && j<=result.cols/3*2) || ( j == result.cols/3 && i >= result.rows/3 && i <= result.rows/3*2) || ( j == result.cols/3*2 && i >= result.rows/3 && i <= result.rows/3*2) )
		//if(i==result.rows/3 || i==result.rows/3*2 || j==result.cols/3 || j==result.cols/3*2 ||
//( (float)(k1*j) == (float)i ) || ( (float)(k2*j + result.rows) == (float)i )) 
		result.at<uchar>(i,j)=255;
	   }
	}

	for(int i = 0;i <result.rows;i++ )
	{  
	   for(int j = 0;j <result.cols;j++ )
	   {
		if( (float)(k1*j) >= (float)i && (float)(k1*j) <= (float)(i+1) && (j<=result.cols/3 || j>= result.cols/3*2) || ( (float)(k2*j+result.rows) >= (float)i &&  (float)(k2*j+result.rows) <= (float)(i+1) && (j<=result.cols/3 || j>= result.cols/3*2) ) )
		result.at<uchar>(i,j)=255;
	   }
	}
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

