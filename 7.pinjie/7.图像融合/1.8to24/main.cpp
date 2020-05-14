#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
/*
线性混合运算　带权值
	F(x,y) = alpha*f(x,y) (1-alpha)*g(x,y)
alpha属于0-1,代表两张图混合时候哪张图占的比例更大。f(x,y),g(x,y)代表两张图，也可以是两个区域。混合
运算的两个对象。f(x,y)代表混合后生成的图片。
*/
using namespace std;
using namespace cv;

int main()
{
	//读入四幅图片
	
	    Mat imgGray= imread("ronghe.bmp",0);
    Mat imgRGB;
    cvtColor(imgGray, imgRGB, COLOR_GRAY2RGB);
    imshow("imgRGB",imgRGB);
    cout<<"imgRGB.depth is :"<<imgRGB.depth()<<endl;
	waitKey(0);

	return 0;
}

