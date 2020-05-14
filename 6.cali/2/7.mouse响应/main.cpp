#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<stdio.h>
#include <string>
#include <iostream>
using namespace cv;
using namespace std;

Mat src;
Mat dst;
vector<Point> vp2f;

void on_mouse(int event, int x, int y, int flags, void* ustc)
{

    static Point pre_pt ;
    char temp_1[20];
    int i = 0;
    if (event == CV_EVENT_LBUTTONDOWN && vp2f.size()<4)
    {
        dst.copyTo(src);
        pre_pt = Point(x, y);
        sprintf(temp_1,"x:%d,y:%d",x,y);
        putText(src,temp_1,Point(x,y),FONT_HERSHEY_SIMPLEX,0.5,Scalar(150,55,245));
	circle(src, Point(x,y), 3, Scalar(0, 0, 255),-1);
	vp2f.push_back(Point(x,y));
        imshow("src", src);
        src.copyTo(dst);//确保画线操作是在src上进行
    }

}

int main()
{
    //注意：这一步必须要有，不然进行不了鼠标操作
    namedWindow("src", WINDOW_AUTOSIZE);//WINDOW_AUTOSIZE:系统默认,显示自适应
    src = imread("yy_black.JPG", 1);//1:为原图颜色,0:为灰度图，黑白颜色
    src.copyTo(dst);

    setMouseCallback("src", on_mouse, 0);

    imshow("src", src);
	
    waitKey(0);
    for(int i=0;i<vp2f.size();i++)
    {
	cout<<"!!!"<<endl;
	cout<<"vp2f: "<<vp2f[i]<<endl;
    }
    return 0;
}


