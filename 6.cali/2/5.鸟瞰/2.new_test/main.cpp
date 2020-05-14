#include<opencv2/opencv.hpp>  
#include<iostream>

using namespace std;
using namespace cv;

int main(int argc,char** argv)
{
	Point2f pts_src[4], pts_dst[4];
	Mat warp_matrix;
	Mat src, dst;
	int offset_x, offset_y;
 
	src = imread("yy_top.JPG",1);
	dst = imread("result.jpg");
	int h = src.size[0];
	int w = src.size[1];
	//映射关系 (310,281)->(300,200)		(193,14)->(300,50)			(928,14)->(800,50)		(744,281)->(800,200)
	offset_x = 100;//为正，横向拉伸，为负，横向压缩
	offset_y = 0;//为正，纵向拉伸，为负，纵向压缩
 
	pts_src[0].x = 103;
	pts_src[0].y = 330;
	pts_src[1].x = 103;
	pts_src[1].y = 321;
	pts_src[2].x = 242;
	pts_src[2].y = 321;
	pts_src[3].x = 242;
	pts_src[3].y = 330;
 
	pts_dst[0].x = 586;
	pts_dst[0].y = 346;
	pts_dst[1].x = 714;
	pts_dst[1].y = 351;
	pts_dst[2].x = 749;
	pts_dst[2].y = 409;
	pts_dst[3].x = 544;
	pts_dst[3].y = 406;
 
	
 
	//计算变换矩阵
	warp_matrix=getPerspectiveTransform( pts_src, pts_dst);
	//透视变换
	//warpPerspective( src, dst ,warp_matrix);
 	warpPerspective(src, dst, warp_matrix, cv::Size(w,h), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
	imshow("xxx",dst);

 
	waitKey(0);
 
	
 
	return 0;
}
