#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
 
using namespace std;
using namespace cv;
 
bool draw;
Mat src;//原始图像  
Mat roi;//ROI图像
Point cursor;//初始坐标   
Rect rect;//标记ROI的矩形框
 
/*
	void onMouse(int event, int x, int y, int flags, void *param);
	第一个参数，鼠标操作时间的整数代号，在opencv中，event鼠标事件总共有10中，从0-9依次代表如下:
	EVENT_MOUSEMOVE      =0,    //滑动
	EVENT_LBUTTONDOWN    =1,    //左键点击
	EVENT_RBUTTONDOWN    =2,    //右键点击
	EVENT_MBUTTONDOWN    =3,    //中间点击
	EVENT_LBUTTONUP      =4,    //左键释放
	EVENT_RBUTTONUP      =5,    //右键释放
	EVENT_MBUTTONUP      =6,    //中间释放
	EVENT_LBUTTONDBLCLK  =7,    //左键双击
	EVENT_RBUTTONDBLCLK  =8,    //右键双击
	EVENT_MBUTTONDBLCLK  =9     //中间释放
	第二个参数，代表鼠标位于窗口的(x，y)坐标位置，窗口左上角默认为原点，向右为x轴，向下为y轴；
	第三个参数，代表鼠标的拖拽事件，以及键盘鼠标联合事件，总共有32种事件，这里不再赘述。
	第四个参数，函数参数的编号。
*/
void onMouse(int event, int x, int y, int flags, void *param)
{
	Mat img = src.clone();
	switch (event)
	{
		//按下鼠标左键
	case CV_EVENT_LBUTTONDOWN:
		//点击鼠标图像时，清除之前ROI图像的显示窗口  
		cvDestroyWindow("ROI");
		//存放起始坐标  
		cursor = Point(x, y);
		//circle(img, Point(x,y), 3, Scalar(0, 0, 255),-1);
		//初始化起始矩形框  
		rect = Rect(x, y, 0, 0);
		draw = true;
		break;
 
		//松开鼠标左键      
	case CV_EVENT_LBUTTONUP:
		if (rect.height > 0 && rect.width > 0)
		{
			//将img中的矩形区域复制给roi，并显示在SignROI窗口 
			roi = img(Rect(rect.x, rect.y, rect.width, rect.height));
			rectangle(img, rect, Scalar(0, 0, 255), 2);
			namedWindow("SignROI");
			imshow("SignROI", img);
 
			//将画过矩形框的图像用原图像还原  
			src.copyTo(img);
			imshow("SrcImage", img);
 
			//显示ROI图像
			namedWindow("ROI");
			imshow("ROI", roi);
			imwrite("roi.jpg",roi);
			waitKey(0);
		}
		draw = false;
		break;
 
		//移动光标
	case CV_EVENT_MOUSEMOVE:
		if (draw)
		{
			//用MIN得到左上点作为矩形框的起始坐标，如果不加这个，画矩形时只能向一个方向进行  
			rect.x = MIN(x, cursor.x);
			rect.y = MIN(y, cursor.y);
			rect.width = abs(cursor.x - x);
			rect.height = abs(cursor.y - y);
			//防止矩形区域超出图像的范围  
			rect &= Rect(0, 0, src.cols, src.rows);
		}
		break;
	}
}
 
int main()
{
	src = imread("top.jpg");
	if (src.data == 0)
	{
		cout << "图片不存在" << endl;
		return -1;
	}
	namedWindow("SrcImage");
	imshow("SrcImage", src);
	/*
		void setMouseCallback(const string& winname, MouseCallback onMouse, void* userdata=0 );
		Parameters:
		第一个参数，windows视窗名称，对名为winname的视窗进行鼠标监控；
		第二个参数，鼠标响应处理函数，监听鼠标的点击，移动，松开，判断鼠标的操作类型，并进行响应的函数处理；
		第三个参数，鼠标响应处理函数的ID，与鼠标相应处理函数相匹配就行，暂时只用到默认为0的情况。
	*/
	setMouseCallback("SrcImage", onMouse, NULL);
	waitKey();
	return 0;
}
