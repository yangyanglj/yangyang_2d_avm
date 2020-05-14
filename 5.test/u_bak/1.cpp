// Undistortion.cpp: 定义控制台应用程序的入口点。
//
 
#include<opencv2/opencv.hpp>
#include<math.h>
using namespace cv;
 
//matlab 单相机标定结果，相机A
double k[3] = { -0.0903,0.7409,0.0 };//径向畸变 k1,k2,k3
double p[2] = { -0.0012,0.0013 };	//切向畸变 p1,p2
double intrinsic[3][3] = { {3449.6,0,0},{6.3538,3449.63,0},{668.4023,472.6526,1} };
void undistortion(Mat input, Mat &output);		//去畸变
 
int main()
{
	Mat picture = imread("BACK_0.bmp");
	Mat srcImage;
	cvtColor(picture, srcImage, CV_BGR2GRAY);
	imshow("【原图片】", srcImage);
	//a blank image of the same size
	Mat dstImage = Mat::zeros(srcImage.rows, srcImage.cols, CV_8UC1);
 
	undistortion(srcImage, dstImage);
	imshow("【去畸变后】", dstImage);
 
	Mat diffImage = (srcImage - dstImage)*50;
	imshow("【变化图-放大50倍的效果】", diffImage);
	waitKey();
	return 0;
}
 
void undistortion(Mat input, Mat &output)
{
	double fx = intrinsic[0][0];
	double fy = intrinsic[1][1];	//焦距
	double cx = intrinsic[2][0];
	double cy = intrinsic[2][1];	//光心位置
	double k1 = k[0];
	double k2 = k[1];
	double k3 = k[2];		//径向畸变系数
	double p1 = p[0];
	double p2 = p[1];		//切向畸变系数
	double r;		//畸变图 距离中心点的距离
	double x1, y1, x2, y2;	//中间变量
	for (int i = 0; i < input.cols; i++)
	{
		for (int j = 0; j < input.rows; j++) {
			//①像素坐标系到 相机坐标系
			x1 = (i - cx) / fx;
			y1 = (j - cy) / fy;
 
			//②在相机坐标系下进行去畸变操作
			r = x1 * x1 + y1 * y1;
			x2 = x1 * (1 + k1 * pow(r, 2) + k2 * pow(r, 4)) + 2 * p1*x1*y1 + p2 * (pow(r, 2) + 2 * pow(x1, 2));
			y2 = y1 * (1 + k1 * pow(r, 2) + k2 * pow(r, 4)) + 2 * p2*x1*y1 + p1 * (pow(r, 2) + 2 * pow(y1, 2));
 
			//③在去畸变操作后，重新转换到 像素坐标系
			x2 = x2 * fx + cx;
			y2 = y2 * fy + cy;
 
			//④并用源图像的像素值对新图像的像素点进行插值
			if (y2 > 0 && y2 < input.rows - 1 && x2>0 && x2 < input.cols - 1) {	//防止越界
				double h = y2;
				double w = x2;
				double result = (floor(w + 1) - w)*(floor(h + 1) - h)*input.at<uchar>(floor(h), floor(w)) +
					(floor(w + 1) - w)*(h - floor(h))*input.at<uchar>(floor(h + 1), floor(w)) +
					(w - floor(w))*(floor(h + 1) - h)*input.at<uchar>(floor(h), floor(w + 1)) +
					(w - floor(w))*(h - floor(h))*input.at<uchar>(floor(h + 1), floor(w + 1));		//双线性内插
				output.at<uchar>(j, i) = floor(result);
				//std::cout << output.at<uchar>(j, i) - input.at<uchar>(j, i) << "\t";
			}
		}
	}
}
