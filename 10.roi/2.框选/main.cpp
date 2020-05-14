//https://blog.csdn.net/shizheng_Li/article/details/99332635
#include<opencv2/opencv.hpp>
 
using namespace std;
using namespace cv;
 
Mat org;
Mat img;
Mat imgROI;
Mat roiResize;
Mat tranPart;

vector<Point> vp2f;
int foo = 20;
int fooResize = 2 * foo;
 
void on_Mouse(int event, int x, int y, int flag, void*ustc) {
	char temp_1[20];
	int num[10];
	Mat img_temp = img.clone();
	Point2f kk[4];
	int i = 0;
		if(event == CV_EVENT_LBUTTONDOWN && vp2f.size()<4)
		{
		sprintf(temp_1,"x:%d,y:%d",x,y);
		//sprintf(temp_1,"%d",vp2f.size()+1);
		putText(img,temp_1,Point(x,y),FONT_HERSHEY_SIMPLEX,0.5,Scalar(150,55,245));
		circle(img, Point(x,y), 3, Scalar(0, 0, 255),-1);
		kk[i++].x = x;
		kk[i++].y = y;
		if(i > 0 && i < 4)
		{
		   if(kk[i].y != kk[++i].y)
		   {
			kk[++i].y = max(kk[i].y,kk[++i].y);
		   }
		}
		vp2f.push_back(Point(kk[i++].x,kk[i++].y));
		
		}

		imshow("img", img_temp);
	
}
 
int main() {
	org = imread("top.jpg");
	img = org.clone();
	
	namedWindow("img");
	imshow("img", img);
	setMouseCallback("img", on_Mouse, 0);	
	waitKey(0);
	for(int i=0;i<vp2f.size();i++)
	{
	    cout<<"(x,y):"<<vp2f[i].x<<","<<vp2f[i].y<<endl;
	}
}
