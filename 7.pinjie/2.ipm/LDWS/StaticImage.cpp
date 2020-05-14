#include "StaticImage.h"

// Default constructor
StaticImage::StaticImage(string path):LDWS(path)
{}

// Method applying the LDWS algorithm 应用LDWS算法的方法
void StaticImage::applyAlgorithm()
{
	//Store and reading original image in img matrix 在img矩阵中存储和读取原始图像
	sourceImg = imread(inputPath);
	Mat temp;
	// Check for invalid image
	if(!sourceImg.data)                              
	{
		cout <<  "Could not open or find the image" << std::endl;
		return;
	}
	else
	{
		imageTransformation = ImageTransformation(sourceImg);
		roi.setROI(imageTransformation.getWidth(), imageTransformation.getHeight());
		roi.computeHomography();
		roi.computeInverseHomography();
		while(1)
		{
			imageTransformation.applyIPM(sourceImg, ipmImage, roi.getIpmHomography());
			temp = ipmImage.clone();
			//去噪声
			imageTransformation.applyGaussianBlur(ipmImage);
			//灰度化
			imageTransformation.applyCvtColor(ipmImage);
			//边缘检测
			imageTransformation.applyCanny(ipmImage);
			
			laneAnalysis.setSourceLines(imageTransformation.applyHoughLinesP(ipmImage,temp));

			laneAnalysis.sortLinesByAngle(ipmImage.cols);
			
			laneAnalysis.getSingleLaneSegment();
			
			laneAnalysis.getLineFromSegment(ipmImage.rows);
			
			laneAnalysis.addIPMPoints();
			
			//Applying inverse IPM to detected lanes 将反IPM应用于检测到的通道
			if(laneAnalysis.getRightLaneDetected() == true || laneAnalysis.getLeftLaneDetected() == true)
				laneAnalysis.setlinePointsSourceImage(imageTransformation.applyReverseIPM(roi.getInverseHomography(), laneAnalysis.getLinePointsIPMImage()));
			
			laneAnalysis.drawFinalLines(sourceImg);
			imshow( "drawFinalLines", sourceImg);
			laneAnalysis.checkAndDrawDeparture(sourceImg, sourceImg.cols);
			
			roi.showROI(sourceImg);
			
			//Showing original image with detected lanes
			imshow( "FinalImage", sourceImg);

			// Wait until user exit program by pressing a key
			waitKey(0);
		}
	}
}
