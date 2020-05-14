#include "ImageTransformation.h"
#include <iostream>

using namespace std;
// Default constructor
ImageTransformation::ImageTransformation():width(0),height(0),minThreshold(0),maxThreshold(0)
{}

// Constructor
ImageTransformation::ImageTransformation(Mat& sourceImage)
{
/*
static_cast一般将枚举类型转换为整形，或者整形转换为浮点型，也可以用来将指向父类的指针转换成指向
子类的指针，做此转换前，需要确定转换的数据是目标类型的数据，因为static_cast不做运行时候的类型检查
*/
	width = static_cast<int>(sourceImage.size().width);
	height = static_cast<int>(sourceImage.size().height);
	minThreshold = 0.66 * 72;	//0.66*MeanThreshold
	maxThreshold = 1.33 * 72;	//1.33*MeanThreshold
}

// Getter Method returning the width attribute.
int& ImageTransformation::getWidth()
{
	return width;
}

// Getter Method returning the height attribute.
int& ImageTransformation::getHeight()
{
	return height;
}

// Applying IPM 应用IPM
void ImageTransformation::applyIPM(Mat& sourceImage, Mat& detinationImage, Mat& homography)
{
	// Warp source image to destination 将源图像变形到目标
	
	//imshow("detinationImage-0",detinationImage);
	warpPerspective(sourceImage, detinationImage, homography, Size(width /5, height /5));
	//imshow("sourceImage",sourceImage);
	imshow("detinationImage",detinationImage);
	imwrite("detinationImage.jpg",detinationImage);
	//waitKey(0);
}

// Applying reverse IPM
vector<Vec2f> ImageTransformation::applyReverseIPM(Mat& inverseHomography, vector<Vec2f>& linePointsIPM)
{
	
	vector<Vec2f> linePointsSourceImage;
	
	perspectiveTransform(linePointsIPM, linePointsSourceImage, inverseHomography);

	return linePointsSourceImage;
}

// Applying Gaussian Blur 
void ImageTransformation::applyGaussianBlur(Mat& image)
{
	GaussianBlur(image, image, Size(7, 7), 0, 0);
}
	
// Converting an RGB image to greyscale
void ImageTransformation::applyCvtColor(Mat& image)
{
	cvtColor(image, image, CV_BGR2GRAY);
}

// Applying Canny edge detection
void ImageTransformation::applyCanny(Mat& image)
{
	Canny(image, image, minThreshold, maxThreshold);
	//imshow("canny image",image);
	//waitKey(0);
}

// Applying Hough Transform  应用Hough变换
vector<Vec4i> ImageTransformation::applyHoughLinesP(Mat& image,Mat temp)
{
	vector<Vec4i> lines;

	//Hough Lines for 1280*720 video
	HoughLinesP( image, lines, 1, CV_PI/180, 15, 10, 4);
#if 1
	for(size_t i=0;i<lines.size();i++)
	{
	    Vec4i l = lines[i];//v3c4i为vec<int,4>里面存放4个int
	    line(temp, Point(l[0],l[1]), Point(l[2],l[3]), Scalar(0, 0, 255), 1, CV_AA); 
	}
#endif
	imshow("xxx",temp);
	//Hough Lines for 640*420 video
	//HoughLinesP( image, lines, 1, CV_PI/180, 14, 9, 3);
	
	return lines;
}
