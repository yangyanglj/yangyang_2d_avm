/*
 *******************************************************************************
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
 *
 * The present file contains source code taken from the file 
 * opencv_2.4.11\sources\samples\cpp\tutorial_code\calib3d\camera_calibration\camera_calibration.cpp
 * and opencv_2.4.11\sources\samples\cpp\stereo_calib.cpp  .
 *
 *
 *                               License Agreement
 *         For Camera Automatic Calibration tool based on openCV library .
 *                              (3-clause BSD License)
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 * - Neither the names of the copyright holders nor the names of the contributors may be used to endorse or promote products derived from this software without specific prior written permission.
 * This software is provided by the copyright holders and contributors “as is” and any express or implied warranties, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose are disclaimed. In no event shall copyright holders or contributors be liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, procurement of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on any theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out of
 * the use of this software, even if advised of the possibility of such damage.
 *
 *******************************************************************************
 */
 
#include <iostream>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

using namespace cv;
using namespace std;


using namespace std;
using namespace cv;
 
void ReadIntrinsics(Mat &cameraMatrix, Mat &distCoeffs, Size &imageSize, char *IntrinsicsPath)
{
	bool FSflag = false;
	FileStorage readfs;
 
	FSflag = readfs.open(IntrinsicsPath, FileStorage::READ);
	if (FSflag == false) cout << "Cannot open the file" << endl;
	readfs["Camera_Matrix"] >> cameraMatrix;
	readfs["Distortion_Coefficients"] >> distCoeffs;
	readfs["image_Width"] >> imageSize.width;
	readfs["image_Height"] >> imageSize.height;
 
	cout << cameraMatrix << endl << distCoeffs << endl << imageSize << endl;
 
	readfs.release();
}
 
void Undistort_img(Mat map1, Mat map2, char *path)
{
	Mat img1, img2;
	img1 = imread(path);
	if (img1.empty()) cout << "Cannot open the image" << endl;
	remap(img1, img2, map1, map2, INTER_LINEAR);
// 	imwrite(path, img2);
	imshow("src img", img1);
	imshow("dst img", img2);
	waitKey();
}
 
int main()
{
	Mat	cameraMatrix, distCoeffs, map1, map2;
	Size imageSize;
	char * IntrinsicsPath = "Intrinsics.yml";
	ReadIntrinsics(cameraMatrix, distCoeffs, imageSize, IntrinsicsPath);
 
	// 去畸变并保留最大图
	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(),
		getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
		imageSize, CV_16SC2, map1, map2);
 
	Undistort_img(map1, map2, "./img1.bmp");
 
	// 去畸变至全图
	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), Mat(),
		imageSize, CV_16SC2, map1, map2);
 
    Undistort_img(map1, map2, "./img1.bmp");
}
