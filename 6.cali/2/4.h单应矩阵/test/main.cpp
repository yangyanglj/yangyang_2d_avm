#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

struct userdata {
	Mat im;
	vector<Point2f> points;
};

void mouseHandle(int event, int x, int y, int flags,void* ptr)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		userdata *data = (userdata*)ptr;
		circle(data->im, Point(x, y), 3, Scalar(0, 0, 255), 3, CV_AA);
		imshow("dst", data->im);
		if (data->points.size() < 4)
		{
			data->points.push_back(Point2f(x, y));
		}
	}
}

int main()
{
	Mat Image = imread("left.jpg");
	//imshow("Image", Image);
	Mat result = Mat::zeros(400, 300, CV_8UC3);

	vector<Point2f> obj;
	obj.push_back(Point2f(0, 0));
	obj.push_back(Point2f(300, 0));
	obj.push_back(Point2f(300, 400));
	obj.push_back(Point2f(0, 400));

	Mat dst = Image.clone();
	
	userdata data;
	data.im = dst;
	imshow("dst", dst);
	setMouseCallback("dst", mouseHandle, &data);
	waitKey(0);

	Mat H = findHomography(data.points, obj, CV_RANSAC);
	warpPerspective(Image, result, H, result.size());
	imshow("result", result);
	imwrite("result.jpg",result);
	waitKey(0);
}

