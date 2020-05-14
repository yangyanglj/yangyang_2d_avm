#include "ROI.h"

// Default constructor
ROI::ROI()
{
	origPointsROI = {};
	destPointsROI = {};
	ipmHomography = Mat();
	inverseHomography = Mat();
}

// Getter method for the ipmHomography matrix
Mat& ROI::getIpmHomography()
{
	return ipmHomography;
}

// Getter method for the inverseHomography matrix
Mat& ROI::getInverseHomography()
{
	return inverseHomography;
}

// Method implementing the ROI
void ROI::setROI(int& originalWidth, int& originalHeight)
{
#if 0
	//ROI points in the source image
	origPointsROI.push_back( Point2f(originalWidth - (5 * originalWidth / 8), originalHeight - (3 * originalHeight / 8)) );
	origPointsROI.push_back( Point2f(originalWidth - (3 * originalWidth / 8), originalHeight - (3 * originalHeight / 8)) );
	origPointsROI.push_back( Point2f(originalWidth - (originalWidth / 6), originalHeight - (originalHeight / 6)) );
	origPointsROI.push_back( Point2f(originalWidth - (5 * originalWidth / 6), originalHeight - (originalHeight / 6)) );
#endif	
#if 0//ok
	origPointsROI.push_back( Point2f(originalWidth*0.31,originalHeight*0.40) );
	origPointsROI.push_back( Point2f(originalWidth*0.75,originalHeight*0.40) );
	origPointsROI.push_back( Point2f(originalWidth*0.97,originalHeight*0.53) );
	origPointsROI.push_back( Point2f(originalWidth*0.13,originalHeight*0.53) );
#endif
#if 1//ok
	origPointsROI.push_back( Point2f(originalWidth*0.01,originalHeight*0.01) );
	origPointsROI.push_back( Point2f(originalWidth*1,originalHeight*0.01) );
	origPointsROI.push_back( Point2f(originalWidth*1,originalHeight*1) );
	origPointsROI.push_back( Point2f(originalWidth*0.01,originalHeight*1) );
#endif
#if 1
	//ROI points in the IPM image
	destPointsROI.push_back(Point2d(0,0));
	destPointsROI.push_back(Point2d(originalWidth / 5, 0));
	destPointsROI.push_back(Point2d(originalWidth / 5, originalHeight / 5));
	destPointsROI.push_back(Point2d(0, originalHeight / 5 ));
#endif
#if 0
	//ROI points in the IPM image
	destPointsROI.push_back(Point2d(0,0));
	destPointsROI.push_back(Point2d(originalWidth*2 , 0));
	destPointsROI.push_back(Point2d(originalWidth*2 , originalHeight*2 ));
	destPointsROI.push_back(Point2d(0, originalHeight*2  ));
#endif
}

// Method displaying the ROI
void ROI::showROI(Mat& img)
{
	assert(origPointsROI.size() == 4);
	line(img, Point(static_cast<int>(origPointsROI[0].x), static_cast<int>(origPointsROI[0].y)), Point(static_cast<int>(origPointsROI[3].x), static_cast<int>(origPointsROI[3].y)), CV_RGB( 205,205,0), 2);
	line(img, Point(static_cast<int>(origPointsROI[2].x), static_cast<int>(origPointsROI[2].y)), Point(static_cast<int>(origPointsROI[3].x), static_cast<int>(origPointsROI[3].y)), CV_RGB( 205,205,0), 2);
	line(img, Point(static_cast<int>(origPointsROI[0].x), static_cast<int>(origPointsROI[0].y)), Point(static_cast<int>(origPointsROI[1].x), static_cast<int>(origPointsROI[1].y)), CV_RGB( 205,205,0), 2);
	line(img, Point(static_cast<int>(origPointsROI[2].x), static_cast<int>(origPointsROI[2].y)), Point(static_cast<int>(origPointsROI[1].x), static_cast<int>(origPointsROI[1].y)), CV_RGB( 205,205,0), 2);
	
	for(size_t i=0; i<origPointsROI.size(); i++)
	{
		circle(img, Point(static_cast<int>(origPointsROI[i].x), static_cast<int>(origPointsROI[i].y)), 2, CV_RGB(238,238,0), -1);
		circle(img, Point(static_cast<int>(origPointsROI[i].x), static_cast<int>(origPointsROI[i].y)), 5, CV_RGB(255,255,255), 2);
	}
}

// Method computing the homography matrix 计算单应矩阵的方法
void ROI::computeHomography()
{
	// Compute the homography 计算单应性
	ipmHomography = findHomography(origPointsROI, destPointsROI);
}

// Method computing the inverse homography matrix 计算逆单应矩阵的方法
void ROI::computeInverseHomography()
{
	// Compute the inverse homography matrix计算逆单应矩阵
	inverseHomography = ipmHomography.inv();
}

