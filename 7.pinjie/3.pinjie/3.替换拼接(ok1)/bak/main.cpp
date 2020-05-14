#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
 
using namespace cv;
using namespace std;
 
struct userdata {
	Mat im;
	vector<Point2f> points;
};
 
 
void mouseHandler(int event, int x, int y, int flags, void* data_ptr)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		userdata *data = ((userdata *)data_ptr);
		circle(data->im, Point(x, y), 3, Scalar(0, 255, 255), 5, CV_AA);
		imshow("Image", data->im);
		if (data->points.size() < 4)
		{
			data->points.push_back(Point2f(x, y));
		}
	}
 
}
 
 
 
int main(int argc, char** argv)
{
 
	// Read in the image.
	//Mat im_src = imread("first-image.jpg");
	Mat im_src = imread("result.jpg");
	Size size = im_src.size();
 
	// Create a vector of points.
	vector<Point2f> pts_src;
	pts_src.push_back(Point2f(0, 0));
	pts_src.push_back(Point2f(size.width - 1, 0));
	pts_src.push_back(Point2f(size.width/3*2, size.height/3));
	pts_src.push_back(Point2f(size.width/3, size.height/3));
 
 
 	
	// Destination image
	//Mat im_dst = imread("times-square.jpg");
	Mat im_dst = imread("top.jpg");
 
 
	// Set data for mouse handler
	Mat im_temp = im_dst.clone();

	userdata data;
	data.im = im_temp;
 	
 
	//show the image
	imshow("Image", im_temp);
 	
	cout << "Click on four corners of a billboard and then press ENTER" << endl;
	//set the callback function for any mouse event
	setMouseCallback("Image", mouseHandler, &data);
	
	waitKey(0);
 
	Mat H = findHomography(pts_src, data.points,0);

	Mat logo;
	warpPerspective(im_src, logo,H,im_dst.size());

	//阈值化
	Mat logo1;
	threshold(logo,logo1,0,255,1);
 
	im_dst.copyTo(logo,logo1);//把im_dst和logo1做 与 运算后再和logo相加
 	
	imwrite("final.jpg",logo);
 
	// Display image.
 
 
	imshow("Image", logo);

	waitKey(0);
 
	return 0;
}
