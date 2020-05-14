#include<opencv2/opencv.hpp>
#include<vector>
#include <iostream>
 
using namespace std;
using namespace cv;
 
vector<Point2f> imagePoints1, imagePoints2;
 
Mat ref_win, src_win;
int pcount = 0;
 
void on_mouse1(int event, int x, int y, int flags, void *ustc) //event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号  
{
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处打点
	{
		Point  p = Point(x, y);
		circle(ref_win, p, 1, Scalar(0, 0, 255), -1);
		imshow("基准图", ref_win);
		imagePoints1.push_back(p);   //将选中的点存起来
		cout << "基准图: " << p << endl;
		pcount++;
		cout << "ponit num:" << pcount << endl;
	}
}
 
void on_mouse2(int event, int x, int y, int flags, void *ustc) //event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号  
{
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处打点
	{
		Point  p = Point(x, y);
		circle(src_win, p, 1, Scalar(0, 0, 255), -1);
		imshow("待配准图", src_win);
		imagePoints2.push_back(p);   //将选中的点存起来
		cout << "待配准图: " << p << endl;
	}
}
 
int main()
{
	Mat ref = imread("top_output.jpg");  //基准图
	Mat src = imread("ledt_output.jpg");  //待配准图
 
	ref_win = ref.clone();
	src_win = src.clone();
 
	namedWindow("待配准图");
	namedWindow("基准图");
	imshow("待配准图", src_win);
	imshow("基准图", ref_win);
	setMouseCallback("待配准图", on_mouse2);
	setMouseCallback("基准图", on_mouse1);
 
	waitKey();
	string str;
	printf("往下执行？\n");
	cin >> str;
 
 
	//求变换矩阵
	Mat homo = findHomography(imagePoints2, imagePoints1, CV_RANSAC);
 
	Mat imageTransform1;
	warpPerspective(src, imageTransform1, homo, Size(ref.cols, ref.rows));   //变换
	imshow("transform", imageTransform1);
	imshow("基准图打点", ref_win);
	imshow("待配准图打点", src_win);
	imshow("变换图", imageTransform1);
 
	imwrite("result.jpg", imageTransform1);
	imwrite("src_p.jpg", src_win);
	imwrite("ref_p.jpg", ref_win);
 
	waitKey();
	return 0;
}
