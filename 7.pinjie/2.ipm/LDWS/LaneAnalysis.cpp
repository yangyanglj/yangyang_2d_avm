#include "LaneAnalysis.h"

// Default constructor 默认构造函数
LaneAnalysis::LaneAnalysis()
{
	rightLine = {};
	leftLine = {};
	linePointsIPMImage = {};
	linePointsSourceImage = {};
	sourceLines = {};
	rightLines = {};
	leftLines = {};
	//Booleans storing if right or left lane detected 如果检测到右侧或左侧车道，则存储布尔值
	rightLaneDetected = false;
	leftLaneDetected = false;
}

// Constructor 构造函数
LaneAnalysis::LaneAnalysis(vector<Vec4i> lines)
{
	rightLine = {};
	leftLine = {};
	linePointsIPMImage = {};
	linePointsSourceImage = {};
	rightLines = {};
	leftLines = {};
	sourceLines = lines;
	//Booleans storing if right or left lane detected 如果检测到右侧或左侧车道，则存储布尔值
	rightLaneDetected = false;
	leftLaneDetected = false;
}
 
// Setter method for the initial IPM lines detected. 检测到初始IPM线的Setter方法。
void LaneAnalysis::setSourceLines(vector<Vec4i> lines)
{
	sourceLines = lines;
}

// Getter method for the rightLaneDetected boolean attribute. rightLaneDetected布尔属性的getter方法。
bool LaneAnalysis::getRightLaneDetected()
{
	return rightLaneDetected;
}

// Getter method for the lefttLaneDetected boolean attribute. lefttLaneDetected布尔属性的getter方法。
bool LaneAnalysis::getLeftLaneDetected()
{
	return leftLaneDetected;
}

// Getter method for the linePointsIPMImage vector attribute. linePointsIPMImage矢量属性的getter方法。
vector<Vec2f>& LaneAnalysis::getLinePointsIPMImage()
{
	return linePointsIPMImage;
}

// Setter method for the linePointsSourceImage attribute linePointsSourceImage属性的setter方法
void LaneAnalysis::setlinePointsSourceImage(vector<Vec2f> linePoints)
{
	linePointsSourceImage = linePoints;
}

// Method sorting the initialy detected IPM lines by angle 方法按角度对初始检测到的IPM线进行排序
void LaneAnalysis::sortLinesByAngle(int& imageWidth)
{
	//Storing slope of the lanes 存储车道的坡度
	double slope = 0.0;
	
	for( size_t i = 0; i < sourceLines.size(); i++ )
	{
		Vec4i l = sourceLines[i];
		if(l[2] - l[0] != 0)
		{
			slope = (float(l[3]) - l[1])/(l[2] - l[0]);
			
			//Storing lanes by slope 通过斜坡存放车道
			if(slope > 1)
			{
				//Storing lanes by space coordinates 按空间坐标存储车道
				//if(l[2] > (imageWidth - (2 * imageWidth / 3)))
				if(l[2] > (imageWidth - 0.95*imageWidth))
					rightLines.push_back(Vec4i(l[0],l[1],l[2],l[3]));
				else
					leftLines.push_back(Vec4i(l[0],l[1],l[2],l[3]));
			}
			//Storing left lanes 	存放左侧车道
			else if(slope < - 1.0 )
			{
				//Storing lanes by space coordinates 
				//if(l[0] < (imageWidth - (imageWidth / 3)))
				if(l[0] < (imageWidth - 0.05*imageWidth))
					leftLines.push_back(Vec4i(l[0],l[1],l[2],l[3]));
				else
					rightLines.push_back(Vec4i(l[0],l[1],l[2],l[3]));
			}
		}
	}
}

// Method selecting a single line from the initialy detected IPM lines for both right and left lanes.从最初检测到的右侧和左侧通道的IPM线中选择单条线的方法。
void LaneAnalysis::getSingleLaneSegment()
{
	//Storing lane length
	double lineLength = 0.0;
	double previousLineLength = 0.0;
	
	//Right Lane
	for( size_t i = 0; i < rightLines.size(); i++ )
	{
		Vec4i l = rightLines[i];
		//Computing line lenght
		lineLength = sqrt(pow(float(l[3] - l[1]),2) + pow(l[2] - l[0],2));
		if(lineLength > previousLineLength)
		{
			rightLine = Vec4i(l[0],l[1],l[2],l[3]);
			previousLineLength = lineLength;
		}
	}

	//Left Lane
	previousLineLength = 0.0;
	for( size_t i = 0; i < leftLines.size(); i++ )
	{
		Vec4i l = leftLines[i];
		//Computing line lenght
		lineLength = sqrt(pow(float(l[3] - l[1]),2) + pow(l[2] - l[0],2));
		if(lineLength > previousLineLength)
		{
			leftLine = Vec4i(l[0],l[1],l[2],l[3]);
			previousLineLength = lineLength;
		}
	}
}


// Method creating a line fitting in hole image for both right and left lanes.在右侧和左侧车道的孔图像中创建线拟合的方法。
void LaneAnalysis::getLineFromSegment(int& imageHeight)
{
	double slope = 0.0;
	double b = 0.0;
	Vec2i tempA, tempB;

	//Right Lane
	if(rightLine[0] - rightLine[2] != 0)
	{
		slope = (float(rightLine[1]) - rightLine[3]) / (rightLine[0] - rightLine[2]);
		b = rightLine[1] - (slope * rightLine[0]);
		tempA = Vec2i(0,0);
		tempA[0] = - b / slope; 
		tempB = Vec2i(0,imageHeight);
		tempB[0] = (imageHeight - b) / slope;
		rightLine = Vec4i(tempA[0],tempA[1],tempB[0],tempB[1]);
		if(rightLine != Vec4i(0, 0, 0, 0))
			rightLaneDetected = true;
		else
			rightLaneDetected = false;
	}

	//Left Lane
	if(leftLine[0] - leftLine[2] != 0)
	{
		slope = (float(leftLine[1]) - leftLine[3]) / (leftLine[0] - leftLine[2]);
		b = leftLine[1] - (slope * leftLine[0]);
		tempA = Vec2i(0,0);
		tempA[0] = - b / slope; 
		tempB = Vec2i(0,imageHeight);
		tempB[0] = (imageHeight - b) / slope;
		leftLine = Vec4i(tempA[0],tempA[1],tempB[0],tempB[1]);
		if(leftLine != Vec4i(0, 0, 0, 0))
			leftLaneDetected = true;
		else
			leftLaneDetected = false;
	}
}

// Method adding up final right and left detected lanes in a single vector.在单个向量中将最终的右侧和左侧检测到的通道相加的方法。
void LaneAnalysis::addIPMPoints()
{
	//Right Lane
	if(rightLines.size() != 0)
	{
		linePointsIPMImage.push_back(Vec2f(float(rightLine[0]), float(rightLine[1])));
		linePointsIPMImage.push_back(Vec2f(float(rightLine[2]), float(rightLine[3])));
	}
	
	//Left Lane
	if(leftLines.size() != 0)
	{
		linePointsIPMImage.push_back(Vec2f(float(leftLine[0]), float(leftLine[1])));
		linePointsIPMImage.push_back(Vec2f(float(leftLine[2]), float(leftLine[3])));
	}
}


// Method Drawing the final right and left detected lanes in a given source image.方法在给定的源图像中绘制最终的右侧和左侧检测到的通道。
void LaneAnalysis::drawFinalLines(Mat& img)
{
	//Condition if only one lane has been detected
	if(linePointsSourceImage.size() == 2)
	{
		line( img, Point(linePointsSourceImage[0][0], linePointsSourceImage[0][1]), Point(linePointsSourceImage[1][0], linePointsSourceImage[1][1]), Scalar(255,0,0), 3);
	}

	//Condition if two lanes have been detected
	else if(linePointsSourceImage.size() == 4)
	{
		line( img, Point(linePointsSourceImage[0][0], linePointsSourceImage[0][1]), Point(linePointsSourceImage[1][0], linePointsSourceImage[1][1]), Scalar(255,0,0), 3);	
		line( img, Point(linePointsSourceImage[2][0], linePointsSourceImage[2][1]), Point(linePointsSourceImage[3][0], linePointsSourceImage[3][1]), Scalar(255,0,0), 3);
	}
}

// Method checking and drawing the lane departure warnings.方法检查和绘制车道偏离警告。
void LaneAnalysis::checkAndDrawDeparture(Mat& img, int& originalWidth)
{
	//Warning Rectangles points
	Point pt1RightWarning(img.size().width - 60, 10); 
	Point pt2RightWarning(img.size().width - 10, 60); 
	Point pt1LeftWarning(10, 10); 
	Point pt2LeftWarning(60, 60); 
	int ROIlength = (4 * originalWidth) / 6;
	int ROIStart = originalWidth - ((5 * originalWidth) / 6);
	
	//Check if only one lane was detected
	if((rightLaneDetected == true && leftLaneDetected == false) || (rightLaneDetected == false && leftLaneDetected == true))
	{
		if(rightLaneDetected == true)
		{
			if(linePointsSourceImage[1][0]  >= (ROIStart + (3 * ROIlength / 4)))
			{
				rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(0, 255, 0), CV_FILLED, 8, 0);
				rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
			}
			else if((ROIStart + ROIlength) > linePointsSourceImage[1][0] && linePointsSourceImage[1][0] > (ROIStart + (ROIlength / 2)))
			{
				rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(0, 150, 255), CV_FILLED, 8, 0);
				rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
			}
			else if(linePointsSourceImage[1][0] < ROIStart + (ROIlength / 2))
			{
				rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(0, 0, 255), CV_FILLED, 8, 0);
				rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
			}
			else
			{
				rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
				rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
			}
		}
		if(leftLaneDetected == true)
		{
			if(linePointsSourceImage[1][0] <= (ROIStart + (ROIlength/ 4)))
			{
				rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(0, 255, 0), CV_FILLED, 8, 0);
				rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
			}
			else if((ROIStart + (ROIlength / 4)) < linePointsSourceImage[1][0] && linePointsSourceImage[1][0] < (ROIStart + (ROIlength / 2)))
			{
				rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(0, 150, 255), CV_FILLED, 8, 0);
				rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
			}
			else if (linePointsSourceImage[1][0] > (ROIStart + (ROIlength/ 2)))
			{
				rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(0, 0, 255), CV_FILLED, 8, 0);
				rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
			}
			else
			{
				rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
				rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
			}
		}
	}
	
	//Check if two lanes were detected 检查是否检测到两条车道
	else if(rightLaneDetected == true && leftLaneDetected == true)
	{
		//Right lane signaling
		if(linePointsSourceImage[1][0]  >= (ROIStart + (3 * ROIlength / 4)))
			rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(0, 255, 0), CV_FILLED, 8, 0);
			
		else if((ROIStart + ROIlength) > linePointsSourceImage[1][0] && linePointsSourceImage[1][0] > (ROIStart + (ROIlength / 2)))
			rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(0, 150, 255), CV_FILLED, 8, 0);
			
		else if(linePointsSourceImage[1][0] < ROIStart + (ROIlength / 2))
			rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(0, 0, 255), CV_FILLED, 8, 0);
		
		else 
			rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
		//Left lane signaling
		if(linePointsSourceImage[3][0] <= (ROIStart + (ROIlength/ 4)))
			rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(0, 255, 0), CV_FILLED, 8, 0);
			
		else if((ROIStart + (ROIlength / 4)) < linePointsSourceImage[3][0] && linePointsSourceImage[3][0] < (ROIStart + (ROIlength / 2)))
			rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(0, 150, 255), CV_FILLED, 8, 0);
			
		else if (linePointsSourceImage[3][0] > (ROIStart + (ROIlength/ 2)))
			rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(0, 0, 255), CV_FILLED, 8, 0);

		else 
			rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
	}
	
	//Check if no lane were detected
	else
	{
		rectangle(img, pt1RightWarning, pt2RightWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);
		rectangle(img, pt1LeftWarning, pt2LeftWarning, Scalar(169, 169, 169), CV_FILLED, 8, 0);

	}
	//circle(img, Point(ROIStart + (4 * ROIlength / 5), linePointsSourceImage[1][1]), 10, CV_RGB(255,0,0), -1);
}
