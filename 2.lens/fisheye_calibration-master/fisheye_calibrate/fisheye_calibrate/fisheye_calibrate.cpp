/***********Edit by WangZheng***********/
/************Calibration Test***********/
/*************2016--11--10**************/
#include <opencv2\opencv.hpp>
#include <fstream>
#include "videoInput.h"
using namespace std;
using namespace cv;

int main()
{
	int deviceNum = 0;
	videoInput VI;
	int deviceNums = VI.listDevices();
	cout << "You have " << deviceNums << " avaliable!" << endl;
	VI.setUseCallback(true);
	VI.setIdealFramerate(deviceNum, 60);
	//VI.setupDevice(deviceNum,640,480,VI_COMPOSITE);
	VI.setupDevice(deviceNum, 640, 480);
	int width = VI.getWidth(deviceNum);
	int height = VI.getHeight(deviceNum);
	cout << "Present paras as " << "width=" << width << "\t" << "height=" << height << endl;
	VI.showSettingsWindow(0);//����������ʾ��Ƶ���ô��ڣ�����ȥ��
	IplImage* image_ipl = cvCreateImage(cvSize(width, height), 8, 3);//��ʾ��ͼ��
	IplImage* img_frame = cvCreateImage(cvSize(width, height), 8, 3);//�����ͼ��
	int framesize = VI.getSize(deviceNum);
	cout << "framesize=" << framesize << endl;
	unsigned char* frameBuffer = (unsigned char*)malloc(framesize);
	char *str1;
	str1 = ".jpg";
	char filename[2][20] = { "", "" };

	ofstream fout("caliberation_result.txt");  /**    ���涨�������ļ�     **/

	/************************************************************************
	��ȡÿһ��ͼ�񣬴�����ȡ���ǵ㣬Ȼ��Խǵ���������ؾ�ȷ��
	*************************************************************************/
	cout << "��ʼ��ȡ�ǵ㡭����������" << endl;
	//��������Ϊ��Ҫ�ֶ��޸ĵĲ���
	int n_boards = 10;						//ͼ������
	int board_dt = 90;						//Wait 90 frames per chessboard view
	int board_w = 7;
	int board_h = 5;
	int board_n = board_w * board_h;
	Size board_sz = Size(board_w, board_h); //�������ÿ�С��еĽǵ���
	int x_expand = 0, y_expand = 0;		//x,y�������չ(x����y����)���ʵ�������Բ���ʧԭͼ����Ϣ
	int frame=0;

	vector<Point2f> corners;                //����ÿ��ͼ���ϼ�⵽�Ľǵ�
	vector<vector<Point2f>>  corners_Seq;   //�����⵽�����нǵ�/   
	vector<Mat>  image_Seq;
	int successes = 0;						//�ɹ���ȡ�ǵ������ͼ����	
	bool conner_flag = true;				//����ͼ��ǵ���ȡ�ɹ�Ϊtrue������Ϊfalse
	while (successes < n_boards) {
//		printf("successes is %d",successes);
		VI.getPixels(deviceNum, frameBuffer, false, false);//����ʹ��videoInput���õķ�ת���ڴ��ͻ����δ���
		img_frame->imageData = (char*)frameBuffer;
		cvFlip(img_frame, image_ipl, 0);//0��ֱ��ת1ˮƽ��ת-1��ֱˮƽ��ת
		cvShowImage("Raw Video", image_ipl);

		cv::Mat imageSrc = cvarrToMat(image_ipl,1);
		Mat image;//�߽���չ���ͼƬ
		copyMakeBorder(imageSrc, image, (int)(y_expand / 2), (int)(y_expand / 2), (int)(x_expand / 2), (int)(x_expand / 2), BORDER_CONSTANT);
		/* ��ȡ�ǵ� */
		Mat imageGray;
		if ((frame++ % board_dt) == 0){
			cvtColor(image, imageGray, CV_RGB2GRAY);
			bool patternfound = findChessboardCorners(image, board_sz, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE +
				CALIB_CB_FAST_CHECK);
			printf("patternfound is %d", patternfound);
			if (patternfound)
			{
				/* �����ؾ�ȷ�� */
				cornerSubPix(imageGray, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
				/* ���Ƽ�⵽�Ľǵ㲢���� */
				drawChessboardCorners(image, board_sz, corners, patternfound);
				imshow("Calibration", image);
				imwrite("Calibration.jpg", image);
				string imageFileName;
				std::stringstream StrStm;
				StrStm << successes + 1;
				StrStm >> imageFileName;
				imageFileName += "_r.jpg";
				imwrite(imageFileName, image);

				successes = successes + 1;
				corners_Seq.push_back(corners);
				image_Seq.push_back(image);
			}

			
		}
		if (waitKey(15)==27)
		{
			exit(0);
		}
	}
	cvDestroyWindow("Calibration");
	cout << "�ǵ���ȡ��ɣ�" << endl;
	/************************************************************************
	���������
	*************************************************************************/
	cout << "��ʼ���ꡭ����������" << endl;
	Size square_size = Size(20, 20);
	vector<vector<Point3f>>  object_Points;        /****  ���涨����Ͻǵ����ά����   ****/

	vector<int>  point_counts;
	/* ��ʼ��������Ͻǵ����ά���� */
	for (int t = 0; t<successes; t++)
	{
//		printf("t is %d", t);
		vector<Point3f> tempPointSet;
		for (int i = 0; i<board_sz.height; i++)
		{
//			printf("i is %d", i);
			for (int j = 0; j<board_sz.width; j++)
			{
//				printf("j is %d", j);
				/* ���趨��������������ϵ��z=0��ƽ���� */
				Point3f tempPoint;
				tempPoint.x = i*square_size.width;
				tempPoint.y = j*square_size.height;
				tempPoint.z = 0;
				tempPointSet.push_back(tempPoint);
			}
		}
		object_Points.push_back(tempPointSet);
	}
	for (int i = 0; i< successes; i++)
	{
		point_counts.push_back(board_sz.width*board_sz.height);
	}
	/* ��ʼ���� */
	Size image_size = image_Seq[0].size();
	cv::Matx33d intrinsic_matrix;    /*****    ������ڲ�������    ****/
	cv::Vec4d distortion_coeffs;     /* �������4������ϵ����k1,k2,k3,k4*/
	std::vector<cv::Vec3d> rotation_vectors;                           /* ÿ��ͼ�����ת���� */
	std::vector<cv::Vec3d> translation_vectors;                        /* ÿ��ͼ���ƽ������ */
	int flags = 0;
	flags |= cv::fisheye::CALIB_RECOMPUTE_EXTRINSIC;
	flags |= cv::fisheye::CALIB_CHECK_COND;
	flags |= cv::fisheye::CALIB_FIX_SKEW;
	fisheye::calibrate(object_Points, corners_Seq, image_size, intrinsic_matrix, distortion_coeffs, rotation_vectors, translation_vectors, flags, cv::TermCriteria(3, 20, 1e-6));
	cout << "������ɣ�\n";

	/************************************************************************
	�Զ�������������
	*************************************************************************/
	cout << "��ʼ���۶�����������������" << endl;
	double total_err = 0.0;                   /* ����ͼ���ƽ�������ܺ� */
	double err = 0.0;                        /* ÿ��ͼ���ƽ����� */
	vector<Point2f>  image_points2;             /****   �������¼���õ���ͶӰ��    ****/

	cout << "ÿ��ͼ��Ķ�����" << endl;
	cout << "ÿ��ͼ��Ķ�����" << endl << endl;
	for (int i = 0; i<n_boards; i++)
	{
		vector<Point3f> tempPointSet = object_Points[i];
		/****    ͨ���õ������������������Կռ����ά���������ͶӰ���㣬�õ��µ�ͶӰ��     ****/
		fisheye::projectPoints(tempPointSet, image_points2, rotation_vectors[i], translation_vectors[i], intrinsic_matrix, distortion_coeffs);
		/* �����µ�ͶӰ��;ɵ�ͶӰ��֮������*/
		vector<Point2f> tempImagePoint = corners_Seq[i];
		Mat tempImagePointMat = Mat(1, tempImagePoint.size(), CV_32FC2);
		Mat image_points2Mat = Mat(1, image_points2.size(), CV_32FC2);
		for (size_t i = 0; i != tempImagePoint.size(); i++)
		{
			image_points2Mat.at<Vec2f>(0, i) = Vec2f(image_points2[i].x, image_points2[i].y);
			tempImagePointMat.at<Vec2f>(0, i) = Vec2f(tempImagePoint[i].x, tempImagePoint[i].y);
		}
		err = norm(image_points2Mat, tempImagePointMat, NORM_L2);
		total_err += err /= point_counts[i];
		cout << "��" << i + 1 << "��ͼ���ƽ����" << err << "����" << endl;
		fout << "��" << i + 1 << "��ͼ���ƽ����" << err << "����" << endl;
	}
	cout << "����ƽ����" << total_err / n_boards << "����" << endl;
	fout << "����ƽ����" << total_err / n_boards << "����" << endl << endl;
	cout << "������ɣ�" << endl;

	/************************************************************************
	���涨����
	*************************************************************************/
	cout << "��ʼ���涨����������������" << endl;
	Mat rotation_matrix = Mat(3, 3, CV_32FC1, Scalar::all(0)); /* ����ÿ��ͼ�����ת���� */

	fout << "����ڲ�������" << endl;
	fout << intrinsic_matrix << endl;
	fout << "����ϵ����\n";
	fout << distortion_coeffs << endl;
	for (int i = 0; i<n_boards; i++)
	{
		fout << "��" << i + 1 << "��ͼ�����ת������" << endl;
		fout << rotation_vectors[i] << endl;

		/* ����ת����ת��Ϊ���Ӧ����ת���� */
		Rodrigues(rotation_vectors[i], rotation_matrix);
		fout << "��" << i + 1 << "��ͼ�����ת����" << endl;
		fout << rotation_matrix << endl;
		fout << "��" << i + 1 << "��ͼ���ƽ��������" << endl;
		fout << translation_vectors[i] << endl;
	}
	cout << "��ɱ���" << endl;
	fout << endl;


	/************************************************************************
	��ʾ������
	*************************************************************************/
	Mat mapx = Mat(image_size, CV_32FC1);
	Mat mapy = Mat(image_size, CV_32FC1);
	Mat R = Mat::eye(3, 3, CV_32F);
	cout << "�������ͼ��" << endl;
	for (int i = 0; i != n_boards; i++)
	{
		fisheye::initUndistortRectifyMap(intrinsic_matrix, distortion_coeffs, R, intrinsic_matrix, image_size, CV_32FC1, mapx, mapy);
		Mat t = image_Seq[i].clone();
		cv::remap(image_Seq[i], t, mapx, mapy, INTER_LINEAR);
		string imageFileName;
		std::stringstream StrStm;
		StrStm << i + 1;
		StrStm >> imageFileName;
		imageFileName += "_d.jpg";
		imwrite(imageFileName, t);
		cout << "img" << i + 1 << "����" << endl;
	}
	cout << "�������" << endl;


	/************************************************************************
	����һ��ͼƬ
	*************************************************************************/
	while (1)
	{
		cout << "TestImage ..." << endl;
		VI.getPixels(deviceNum, frameBuffer, false, false);//����ʹ��videoInput���õķ�ת���ڴ��ͻ����δ���
		img_frame->imageData = (char*)frameBuffer;
		cvFlip(img_frame, image_ipl, 0);//0��ֱ��ת1ˮƽ��ת-1��ֱˮƽ��ת
		cvShowImage("Raw Video", image_ipl);
		Mat image = cvarrToMat(image_ipl);;
		Mat testImage;
		copyMakeBorder(image, testImage, (int)(y_expand / 2), (int)(y_expand / 2), (int)(x_expand / 2), (int)(x_expand / 2), BORDER_CONSTANT);
		fisheye::initUndistortRectifyMap(intrinsic_matrix, distortion_coeffs, R, intrinsic_matrix, image_size, CV_32FC1, mapx, mapy);
		Mat t = testImage.clone();
		cv::remap(testImage, t, mapx, mapy, INTER_LINEAR);
		imshow("Undistort", t);
		if (cvWaitKey(15)==27)
		{
			break;
		}
	}

	VI.stopDevice(deviceNum);
	cvReleaseImage(&image_ipl);
	cvReleaseImage(&img_frame);
	return 0;
}