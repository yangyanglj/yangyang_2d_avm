#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <stdio.h>  
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
 
Mat org, dst, img, tmp;
 
void on_mouse(int event, int x, int y, int flags, void *ustc)
{
	static Point pre_pt = 	Point(-1, -1);
	static Point cur_pt = Point(-1, -1);
	char temp_1[20];
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		sprintf(temp_1,"x:%d,y:%d",x,y);
		org.copyTo(img); 
		pre_pt = Point(x, y);
		
		putText(img,temp_1,pre_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(150,55,245));
		circle(img, pre_pt, 1, Scalar(225, 0, 225), 7, 8);
	}
	else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//摁下左键，flags为1 
	{
		img.copyTo(tmp);
		cur_pt = Point(x, y);
		rectangle(tmp, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);
		if(flags)
		{
			sprintf(temp_1,"x:%d,y:%d",x,y);
			putText(tmp,temp_1,cur_pt,FONT_HERSHEY_SIMPLEX,0.5,Scalar(150,55,245));
			circle(tmp, cur_pt, 1, Scalar(225, 0, 225), 7, 8);
		}
		imshow("img", tmp);
	}
	else if (event == CV_EVENT_LBUTTONUP) 
	{
		
		org.copyTo(img);
		cur_pt = Point(x, y);
		rectangle(img, pre_pt, cur_pt, Scalar(0, 255, 0, 0), 1, 8, 0);
		imshow("img", img);
		img.copyTo(tmp);
		int width = abs(pre_pt.x - cur_pt.x);
		int height = abs(pre_pt.y - cur_pt.y);
		if (width == 0 || height == 0)
		{
			return;
		}

		dst = org(Rect(min(cur_pt.x, pre_pt.x), min(cur_pt.y, pre_pt.y), width, height));
		namedWindow("dst");
		imshow("dst", dst);
		imwrite("dst.jpg",dst);
	}
}
int main()
{
	org = imread("top.jpg");
	org.copyTo(img);
	namedWindow("img");
	setMouseCallback("img", on_mouse, 0); 
	imshow("img", img);
	waitKey(0);
}
