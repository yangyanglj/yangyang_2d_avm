#pragma once
#include "opencv2/opencv.hpp"
#include <string>
#include <fstream>
#include <iostream>

using namespace std;
using namespace cv;

#define CV
//#define FISHEYE

class CCalibration
{
public:
    CCalibration(string patternImgPath, string CalibResultPath, Size boardSize)
    {
        this->patternImgPath=patternImgPath;
        this->calibResultPath=CalibResultPath;
        this->boardSize=boardSize;
    }
    ~CCalibration(){}

private:
	vector<cv::Point3f> singlePatternPoints;
    vector<Mat> patternImgList; 
    int imgHeight;
    int imgWidth;
    int imgNum;
	float scale = 0.25;
	float errThresh = 3000;
    string patternImgPath;
    string calibResultPath;
    Size boardSize;
    Mat camK;
    Mat camDiscoeff;

private:
	int evaluateCalibrationResult(vector<vector<cv::Point3f> > objectPoints, vector<vector<cv::Point2f> > cornerSquare, vector<int> pointCnts, vector<cv::Vec3d> _rvec,
		vector<cv::Vec3d> _tvec, cv::Mat _K, cv::Mat _D, int count, vector<int> &outLierIndex, int errThresh);
	bool testCorners(vector<cv::Point2f>& corners, int patternWidth, int patternHeight);
	void init3DPoints(cv::Size boardSize, cv::Size squareSize, vector<cv::Point3f> &singlePatternPoint);
public:
    bool writeParams();
    bool readPatternImg();
    void calibProcess();
    void run();
};
