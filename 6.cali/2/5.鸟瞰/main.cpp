#include<opencv2/opencv.hpp>  
#include<iostream>

using namespace std;
using namespace cv;
int main()
{

	int b = 0, g = 0, r = 255;


	cv::Mat image = cv::imread("img1.jpg");
	
	
	cv::resize(image, image, cv::Size(1280, 720));//640,480

	int h = image.size[0];
	int w = image.size[1];

	//cv::namedWindow("window1");
	//cv::imshow("window1", image);
	//cv::waitKey(0);

	cv::Point2f src[] = { cv::Point2f(0, 0), cv::Point2f(w - 1, 0), cv::Point2f(w - 1, h - 1),cv::Point2f(0,h-1) };	
	//cv::Point2f dst[] = { cv::Point2f(0.25*w, 0), cv::Point2f(0.75*(w-1), 0), cv::Point2f(w - 1, h - 1), cv::Point2f(0, h - 1) };
	
	//cv::Point2f dst[] = { cv::Point2f(0.25, 0), cv::Point2f(w-1, 0), cv::Point2f(w - 300, h - 300), cv::Point2f(w-(w - 300), h - 300) };

	cv::Point2f dst[] = { cv::Point2f(0.25, 0), cv::Point2f(w-1, 0), cv::Point2f(w/3*2, h/3), cv::Point2f(w/3, h/3) };

	cv::Mat warp_mat = cv::getPerspectiveTransform(src, dst);


	cv::Mat resultimage;
	cv::warpPerspective(image, resultimage, warp_mat, cv::Size(w,h), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar());
	
	// 透视变换
    //cv::warpPerspective(image,resultimage,warp_mat,cv::Size(w,h),cv::INTER_LINEAR | cv::WARP_INVERSE_MAP);
	
	 resize(resultimage, resultimage, Size(resultimage.rows, resultimage.rows ));
	cv::namedWindow("window2");
	cv::imshow("window2", resultimage);
	
	imwrite("resultimage.jpg",resultimage);
	
	//cv::destroyWindow("window1");
	//cv::destroyWindow("window2");

	cv::waitKey(0);
	return 0;
}

