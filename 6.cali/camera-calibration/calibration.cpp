#include "calibration.h"

bool CCalibration::writeParams()
{
    camK.convertTo(camK, CV_32FC1);
    camDiscoeff.convertTo(camDiscoeff, CV_32FC1);
    char temp[100] = "/home/yy/yangyang/9.棋盘格/My_Snapshots/result/calibResult.txt";
    ofstream out;
    //out.open(calibResultPath+"calibResult.txt", ios::out);
    out.open(temp, ios::out);
    out<<camK.at<float>(0, 0)<<endl;
    out<<camK.at<float>(1, 1)<<endl;
    out<<camK.at<float>(0, 2)<<endl;
    out<<camK.at<float>(1, 2)<<endl;
#ifdef CV
	out << camDiscoeff.at<float>(0, 0) << endl;
	out << camDiscoeff.at<float>(0, 1) << endl;
	out << camDiscoeff.at<float>(0, 2) << endl;
	out << camDiscoeff.at<float>(0, 3) << endl;
	out << camDiscoeff.at<float>(0, 4) << endl;
#elif defined FISHEYE
	out << camDiscoeff.at<float>(0, 0) << endl;
	out << camDiscoeff.at<float>(0, 1) << endl;
	out << camDiscoeff.at<float>(0, 2) << endl;
	out << camDiscoeff.at<float>(0, 3) << endl;
#endif
    out.close();
    return true;
}

bool CCalibration::readPatternImg()
{
    int imgNum=0;
    Mat img;
    do
    {
        stringstream ss;
        ss<<imgNum;
        string path=patternImgPath+ss.str()+".jpg";
	cout<<"path :"<<path<<endl;
        img=imread(path, 0);
        if (!img.data)
        {
            break;
        }
        patternImgList.push_back(img);
        imgNum++;
    } while(true);
    if (imgNum==0)
    {
        cout<<" error! No pattern imgs!"<<endl;
        return false;
    }
    this->imgNum=imgNum;
    imgHeight=patternImgList[0].rows;
    imgWidth=patternImgList[0].cols;

    return true;
}
//通过计算三个相邻角点构成的两个向量之间的夹角判断角点连接性
bool CCalibration::testCorners(vector<cv::Point2f>& corners, int patternWidth, int patternHeight)
{
	if (corners.size() != patternWidth * patternHeight)
	{
		return false;
	}
	double dx1, dx2, dy1, dy2;
	double cosVal;
	for (int i = 0; i < patternHeight; ++i)
	{
		for (int j = 0; j < patternWidth - 2; ++j)
		{
			dx1 = corners[i*patternWidth + j + 1].x - corners[i*patternWidth + j].x;
			dy1 = corners[i*patternWidth + j + 1].y - corners[i*patternWidth + j].y;
			dx2 = corners[i*patternWidth + j + 2].x - corners[i*patternWidth + j + 1].x;
			dy2 = corners[i*patternWidth + j + 2].y - corners[i*patternWidth + j + 1].y;
			cosVal = (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2));
			if (fabs(cosVal) < 0.993)
			{
				return false;
			}
		}
	}
	for (int i = 0; i < patternHeight - 2; ++i)
	{
		for (int j = 0; j < patternWidth; ++j)
		{
			dx1 = corners[(i + 1)*patternWidth + j].x - corners[i*patternWidth + j].x;
			dy1 = corners[(i + 1)*patternWidth + j].y - corners[i*patternWidth + j].y;
			dx2 = corners[(i + 2)*patternWidth + j].x - corners[(i + 1)*patternWidth + j].x;
			dy2 = corners[(i + 2)*patternWidth + j].y - corners[(i + 1)*patternWidth + j].y;
			cosVal = (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2));
			if (fabs(cosVal) < 0.993)
			{
				return false;
			}
		}
	}
	return true;
}

//初始化角点的三维坐标
void CCalibration::init3DPoints(cv::Size boardSize, cv::Size squareSize, vector<cv::Point3f> &singlePatternPoint)
{
	for (int i = 0; i<boardSize.height; i++)
	{
		for (int j = 0; j<boardSize.width; j++)
		{
			cv::Point3f tempPoint;//单个角点的三维坐标
			tempPoint.x = float(i * squareSize.width);
			tempPoint.y = float(j * squareSize.height);
			tempPoint.z = 0;
			singlePatternPoint.push_back(tempPoint);
		}
	}
}

void CCalibration::calibProcess()
{
    //***************摄像机标定****************//
    double time0=(double)getTickCount();
    vector<Point2f> corners;//存储一幅棋盘图中的所有角点二维坐标
    vector<vector<Point2f> > cornersSeq;//存储所有棋盘图角点的二维坐标
    vector<Mat> image_Seq;//存储所有棋盘图
    int successImgNum=0;
    int count=0;
    cout<<"********now to corner********"<<endl;//开始提取角点
    Mat image,scaleImg;
    for (int i=0; i<imgNum; i++)
    {
	    cout<<"Image#"<<i<<"......."<<endl;
        image=patternImgList[i].clone();
		//降采样原图,加快角点提取速度
		cv::resize(image, scaleImg, cv::Size(), scale, scale, CV_INTER_LINEAR);
	    /**********************提取角点*************************/
	    bool patternfound= findChessboardCorners(scaleImg, boardSize,
            corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE+CALIB_CB_FAST_CHECK);
	    if (!patternfound)
	    {
		    cout<<"Can not find chess board corners!\n"<<endl;
		    continue;
	    }
	    else
	    {
			//上采样corner
			for (int num = 0; num < corners.size(); num++)
			{
				cv::Point2f tempPoint = corners[num];
				corners[num] = cv::Point2f(tempPoint.x / scale, tempPoint.y / scale);
			}

		    /************************亚像素精确化******************************/
		    cornerSubPix(image, corners, Size(11, 11), Size(-1,-1) , TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			bool good = testCorners(corners, boardSize.width, boardSize.height);
			if (false == good)	continue;
			/************************绘制检测到的角点并显示******************************/
		    Mat cornerImg = image.clone();
            cvtColor(cornerImg, cornerImg, CV_GRAY2BGR);
		    for (int j=0; j< corners.size(); j++)
		    {
			    circle(cornerImg, corners[j], 20, Scalar(0,0,255), 2, 8, 0);
		    }
            namedWindow("CirclePattern", 0);
            imshow("CirclePattern", cornerImg);
			cout << "press any key to see next pattern image" << endl;
            waitKey(1);

		    count +=(int)corners.size();//所有棋盘图中的角点个数
		    successImgNum++;//成功提取角点的棋盘图个数
		    cornersSeq.push_back(corners);
			image_Seq.push_back(image);
	    }
    }
    cout<<"*******end corner******"<<endl;//角点提取完成
	
    /**************************摄像机标定******************************/
    Size squre_size=Size(20,20);//棋盘格尺寸
    vector<vector<Point3f> > object_points;//所有棋盘图像的角点三维坐标
    vector<int> pointCounts;
	//初始化单幅靶标图片的三维点
	init3DPoints(boardSize, squre_size, singlePatternPoints);
    //初始化标定板上的三维坐标
	for (int n = 0; n<successImgNum; n++)
	{
		object_points.push_back(singlePatternPoints);
		pointCounts.push_back(boardSize.width * boardSize.height);
	}

    /***开始标定***/
    cout<<"*****start calibration******"<<endl;//开始标定
    Size imgSize=Size(imgWidth, imgHeight);
    vector<Vec3d> rotation;//旋转向量
    vector<Vec3d> translation;//平移向量
#ifdef CV
    int flags=0;
	cv::calibrateCamera(object_points, cornersSeq, imgSize, camK, camDiscoeff,
		rotation, translation, flags);
#elif defined FISHEYE
	int flags = 0;
	cv::fisheye::calibrate(object_points, cornersSeq, imgSize, camK,
		camDiscoeff, rotation, translation, flags, cv::TermCriteria(3, 20, 1e-6));
#endif
    cout<<"*****标定完成！*****"<<endl;
    double time1=getTickCount();
    cout<<"Calibration time :"<<(time1-time0)/getTickFrequency()<<"s"<<endl;
	//评价
	vector<int> outLierIndex;
	evaluateCalibrationResult(object_points, cornersSeq, pointCounts, rotation, translation,
		camK, camDiscoeff, successImgNum, outLierIndex, errThresh);
	//删除误差大的角点图
	vector<vector<cv::Point2f> > newCornersSeq;
	successImgNum = 0;
	for (int i = 0; i < cornersSeq.size(); i++)
	{
		if (outLierIndex[i] == 0)
		{
			newCornersSeq.push_back(cornersSeq[i]);
			successImgNum++;
		}
	}
	vector<vector<cv::Point3f> > newObjectPoints;
	for (int n = 0; n<successImgNum; n++)
	{
		newObjectPoints.push_back(singlePatternPoints);
	}
	//重新标定
#ifdef CV
	cv::calibrateCamera(object_points, cornersSeq, imgSize, camK, camDiscoeff,
		rotation, translation, flags);
#elif defined FISHEYE
	cv::fisheye::calibrate(newObjectPoints, newCornersSeq, imgSize, camK, camDiscoeff,
		rotation, translation, flags, cv::TermCriteria(3, 20, 1e-6));
#endif
	//重新计算重投影误差
	outLierIndex.clear();
	evaluateCalibrationResult(object_points, cornersSeq, pointCounts, rotation, translation,
		camK, camDiscoeff, successImgNum, outLierIndex, errThresh);
#ifdef DEBUG
	//通过畸变校正效果查看摄像机标定效果
	cv::Mat R = cv::Mat::eye(3, 3, CV_32FC1);
	cv::Mat mapx, mapy, newCamK, undistortImg, showImg;
	cv::initUndistortRectifyMap(camK, camDiscoeff, R, camK, imgSize, CV_32FC1, mapx, mapy);
	cv::remap(image_Seq[0], undistortImg, mapx, mapy, CV_INTER_LINEAR);
	cv::resize(undistortImg, showImg, cv::Size(), 0.25, 0.25, CV_INTER_LINEAR);
	string winName = "undistortImg";
	cv::namedWindow(winName, 1);
	cv::imshow(winName, showImg);
	cv::waitKey(0);
#endif
}

//估计重投影误差，并排除误差大于设定阈值的靶标图片
int CCalibration::evaluateCalibrationResult(vector<vector<cv::Point3f> > objectPoints, vector<vector<cv::Point2f> > cornerSquare, vector<int> pointCnts, vector<cv::Vec3d> _rvec,
	vector<cv::Vec3d> _tvec, cv::Mat _K, cv::Mat _D, int count, vector<int> &outLierIndex, int errThresh)
{
	//string evaluatPath = calibResultPath + "evaluateCalibrationResult.txt";
	char evaluatPath[100] = "/home/yy/yangyang/9.棋盘格/My_Snapshots/result/evaluateCalibrationResult.txt";
	ofstream fout(evaluatPath);

	double total_err = 0.0;//所有图像的平均误差和
	double err = 0.0;//单幅图像的平均误差
	vector<cv::Point2f> proImgPoints;
	for (int i = 0; i< count; i++)
	{
		float maxValue = -1;
		vector<cv::Point3f> tempPointSet = objectPoints[i];
#ifdef CV
		cv::projectPoints(tempPointSet, _rvec[i], _tvec[i], _K, _D, proImgPoints);
#elif defined FISHEYE
		cv::fisheye::projectPoints(tempPointSet, proImgPoints, _rvec[i], _tvec[i], _K, _D);
#endif
		
		vector<cv::Point2f> tempImgPoint = cornerSquare[i];
		cv::Mat tempImgPointMat = cv::Mat(1, tempImgPoint.size(), CV_32FC2);
		cv::Mat proImgPointsMat = cv::Mat(1, proImgPoints.size(), CV_32FC2);
		for (int j = 0; j != tempImgPoint.size(); j++)
		{
			proImgPointsMat.at<cv::Vec2f>(0, j) = cv::Vec2f(proImgPoints[j].x, proImgPoints[j].y);
			tempImgPointMat.at<cv::Vec2f>(0, j) = cv::Vec2f(tempImgPoint[j].x, tempImgPoint[j].y);
			float dx = proImgPoints[j].x - tempImgPoint[j].x;
			float dy = proImgPoints[j].y - tempImgPoint[j].y;
			float diff = sqrt(dx*dx + dy*dy);
			if (diff > maxValue)
			{
				maxValue = diff;
			}
		}
		fout << "第" << i << "幅图像的最大重投影误差：" << maxValue << "像素" << endl;

		//找出重投影误差大于errThresh的图
		if (maxValue > errThresh)
		{
			outLierIndex.push_back(-1);
		}
		else
		{
			outLierIndex.push_back(0);
		}
	}
	fout.close();
	return 0;
}

void CCalibration::run()
{
    bool readSuccess=readPatternImg();
	if (!readSuccess)
	{
		cout << "Fail!  No Pattern Imgs !" << endl;
		getchar();
	}
	calibProcess();
	writeParams();


}
