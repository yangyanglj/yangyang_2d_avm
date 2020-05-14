#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include "calibration.h"
#include "undistort.h"

using namespace std;
using namespace cv;

int main()
{
    string patternImgPath="/home/yy/yangyang/1.ransac_opencv/6.cali/camera-calibration/1/";
    string calibResultPath="/home/yy/yangyang/9.棋盘格/My_Snapshots/result";
    string srcImgPath="/home/yy/yangyang/1.ransac_opencv/6.cali/camera-calibration/1/0.jpg";
    Size boardSize=Size(9, 6);
    CCalibration calibration(patternImgPath, calibResultPath, boardSize);
    calibration.run();
    CUndistort undistort(srcImgPath, calibResultPath);
    undistort.run();
}
