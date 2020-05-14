#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <string>
#include "videoInput.h"

#include <opencv2\calib3d\calib3d_c.h>

using namespace std;
#define DIS

int main()//���Ի�ȡUSB�ɼ���
{
	int deviceNum = 0;
	videoInput VI;
	int deviceNums = VI.listDevices();
	cout << "You have " <<deviceNums<<" avaliable!"<< endl;
	VI.setUseCallback(true);
	VI.setIdealFramerate(deviceNum,60);
	//VI.setupDevice(deviceNum,640,480,VI_COMPOSITE);
	VI.setupDevice(deviceNum, 640, 480);
	int width = VI.getWidth(deviceNum);
	int height = VI.getHeight(deviceNum);
	cout << "Present paras as "<<"width=" << width << "\t" << "height=" << height << endl;
	VI.showSettingsWindow(0);//����������ʾ��Ƶ���ô��ڣ�����ȥ��
	IplImage* image = cvCreateImage(cvSize(width, height), 8, 3);//��ʾ��ͼ��
	IplImage* frame = cvCreateImage(cvSize(width, height), 8, 3);//�����ͼ��
	int framesize = VI.getSize(deviceNum);
	cout << "framesize=" << framesize << endl;
	unsigned char* frameBuffer = (unsigned char*)malloc(framesize);
	char *str1;
	str1 = ".jpg";
	char filename[2][20] = { "", "" };
	/*
	�����Ǳ궨���ֳ�ʼ��
	*/
#ifdef DIS
	int presentNum = 1;
	int number_image = 10;
	int number_image_copy = number_image;
	CvSize board_size = cvSize(7, 5);//���̴�С
	int cube_length = board_size.width;//�����С
	int board_width = board_size.width;
	int board_height = board_size.height;
	int total_per_image = board_width*board_height;
	CvPoint2D32f * image_points_buf = new CvPoint2D32f[total_per_image];
	CvMat * image_points = cvCreateMat(number_image*total_per_image, 2, CV_32FC1);//ͼ������ϵ
	CvMat * object_points = cvCreateMat(number_image*total_per_image, 3, CV_32FC1);//��������ϵ
	CvMat * point_counts = cvCreateMat(number_image, 1, CV_32SC1);//
	CvMat * intrinsic_matrix = cvCreateMat(3, 3, CV_32FC1);//
	CvMat * distortion_coeffs = cvCreateMat(5, 1, CV_32FC1);
	IplImage * show=cvCreateImage(cvSize(width, height), 8, 3);
	int count;
	int found;
	int step;
	int successes = 0;

	cvNamedWindow("ԭʼͼ��");
	cvNamedWindow("�궨");
	while (presentNum <= number_image_copy)
	{
		VI.getPixels(deviceNum, frameBuffer, false, false);//����ʹ��videoInput���õķ�ת���ڴ��ͻ����δ���
		frame->imageData = (char*)frameBuffer;
		cvFlip(frame, image, 0);//0��ֱ��ת1ˮƽ��ת-1��ֱˮƽ��ת
		cvShowImage("ԭʼͼ��", image);
		if (cvWaitKey(1) == 27) break;
		cvCopy(image,show);
		found = cvFindChessboardCorners(show, board_size, image_points_buf, &count,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if (found == 0){
			cout << "Can't find corners in this frame,continue next frame,wating....\n";
			continue;
		}
		else{
			cout << "Get " << count << "corners in this frame,continue...\n";
			IplImage * gray_image = cvCreateImage(cvGetSize(show), 8, 1); //����ͷ����������IplImage* cvCreateImage( CvSize size, int depth, int channels ); depth ͼ��Ԫ�ص�λ���
			cvCvtColor(show, gray_image, CV_BGR2GRAY); // cvCvtColor(...),��Opencv�����ɫ�ռ�ת������������ʵ��rgb��ɫ��HSV,HSI����ɫ�ռ��ת����Ҳ����ת��Ϊ�Ҷ�ͼ��
			cout << "��ȡԴͼ��Ҷ�ͼ�������...\n";
			cvFindCornerSubPix(gray_image, image_points_buf, count, cvSize(11, 11), cvSize(-1, -1),// ���ڷǳ��ӽ�P�����ز����˺�С������ֵ�������������ؾ��󲢲����ǿ���ġ�
				//Ϊ�˽��������⣬һ����Լ򵥵��޳���P��ǳ��������ء��������:ero_zone������һ������(��win���ƣ���ͨ����winС)����������ڷ������Լ�����ؾ����в������ǡ��������Ҫ����һ����������zero_zoneӦ����ΪcvSize(-1�� - 1)0
				cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			cout << "�Ҷ�ͼ�����ػ��������...\n";
			cvDrawChessboardCorners(show, board_size, image_points_buf, count, found);
			cout << "��Դͼ���ϻ��ƽǵ�������...\n";
			cvShowImage("�궨", show);
			sprintf_s(filename[0], "Image%d.jpg", presentNum);
			cvSaveImage(filename[0], image);//����
			sprintf_s(filename[1], "Show%d.jpg", presentNum);
			cvSaveImage(filename[1], show);//����
			cout << "����ͼƬ���...\n";
			if (cvWaitKey(1) == 27) break;
		}
		if (total_per_image == count){
			step = successes*total_per_image;
			for (int i = step, j = 0; j<total_per_image; ++i, ++j){
				CV_MAT_ELEM(*image_points, float, i, 0) = image_points_buf[j].x; // opencv���������ʾ���ÿ��Ԫ�صĺ꣬�����ֻ�Ե�ͨ��������Ч����ͨ��CV_MAT_ELEM( matrix, elemtype, row, col )��������matrix��Ҫ���ʵľ��󡡡�elemtype������Ԫ�ص����͡���row����Ҫ����Ԫ�ص���������col����Ҫ����Ԫ�ص�����

				CV_MAT_ELEM(*image_points, float, i, 1) = image_points_buf[j].y;// ����ÿ���ǵ��������ֵ������image_point_buf��
				CV_MAT_ELEM(*object_points, float, i, 0) = (float)(j / cube_length);
				CV_MAT_ELEM(*object_points, float, i, 1) = (float)(j%cube_length);
				CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
			}
			CV_MAT_ELEM(*point_counts, int, successes, 0) = total_per_image;
			successes++;
			presentNum++;
		}
	}
	cout << "*********************************************\n";
	cout << "�궨�ɹ���ͼƬΪ" << successes << "֡...\n";
	cout << "*********************************************\n\n";	
	//VI.stopDevice(deviceNum);
	cvReleaseImage(&show);
	//cvReleaseImage(&frame);
	//cvReleaseImage(&image);
	cvDestroyWindow("ԭʼͼ��");
	cvDestroyWindow("�궨");

	cout << "���������ʼ����������ڲ���...\n\n";


	IplImage * show_colie = cvCreateImage(cvSize(width, height), 8, 3);;
	cvCopy(image,show_colie);


	CvMat * object_points2 = cvCreateMat(successes*total_per_image, 3, CV_32FC1); // OpenCV ����Ҫ�ľ���任������ʹ�÷���ΪcvMat* cvCreateMat ( int rows, int cols, int type ); ����type�������κ�Ԥ�������ͣ�Ԥ�������͵Ľṹ���£�CV_<bit_depth> (S|U|F)C<number_of_channels>��

	CvMat * image_points2 = cvCreateMat(successes*total_per_image, 2, CV_32FC1);
	CvMat * point_counts2 = cvCreateMat(successes, 1, CV_32SC1);
	for (int i = 0; i<successes*total_per_image; ++i){
		CV_MAT_ELEM(*image_points2, float, i, 0) = CV_MAT_ELEM(*image_points, float, i, 0);//�����洢�ǵ���ȡ�ɹ���ͼ��Ľǵ�
		CV_MAT_ELEM(*image_points2, float, i, 1) = CV_MAT_ELEM(*image_points, float, i, 1);
		CV_MAT_ELEM(*object_points2, float, i, 0) = CV_MAT_ELEM(*object_points, float, i, 0);
		CV_MAT_ELEM(*object_points2, float, i, 1) = CV_MAT_ELEM(*object_points, float, i, 1);
		CV_MAT_ELEM(*object_points2, float, i, 2) = CV_MAT_ELEM(*object_points, float, i, 2);
	}

	for (int i = 0; i<successes; ++i){
		CV_MAT_ELEM(*point_counts2, int, i, 0) = CV_MAT_ELEM(*point_counts, int, i, 0);
	}


	cvReleaseMat(&object_points);
	cvReleaseMat(&image_points);
	cvReleaseMat(&point_counts);


	CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0) = 1.0f;
	CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1) = 1.0f;


	cvCalibrateCamera2(object_points2, image_points2, point_counts2, cvGetSize(show_colie),
		intrinsic_matrix, distortion_coeffs, NULL, NULL, 0);

	cout << "������ڲ�������Ϊ��\n";
	cout << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0) << "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 1)
		<< "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 2)
		<< "\n\n";
	cout << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 0) << "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1)
		<< "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 2)
		<< "\n\n";
	cout << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 0) << "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 1)
		<< "          " << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 2)
		<< "\n\n";

	cout << "����ϵ������Ϊ��\n";
	cout << CV_MAT_ELEM(*distortion_coeffs, float, 0, 0) << "    " << CV_MAT_ELEM(*distortion_coeffs, float, 1, 0)
		<< "    " << CV_MAT_ELEM(*distortion_coeffs, float, 2, 0)
		<< "    " << CV_MAT_ELEM(*distortion_coeffs, float, 3, 0)
		<< "    " << CV_MAT_ELEM(*distortion_coeffs, float, 4, 0)
		<< "\n\n";

	cvSave("Intrinsics.xml", intrinsic_matrix);
	cvSave("Distortion.xml", distortion_coeffs);

	cout << "��������󡢻���ϵ�������Ѿ��ֱ�洢����ΪIntrinsics.xml��Distortion.xml�ĵ���\n\n";
#endif
#ifndef DIS
	sprintf_s(filename[0], "Image%d.jpg", 1);
	image=cvLoadImage(filename[0]);
	IplImage * show_colie = cvCreateImage(cvSize(width, height), 8, 3);;
	cvCopy(image, show_colie);
#endif
	CvMat * intrinsic = (CvMat *)cvLoad("Intrinsics.xml");
	CvMat * distortion = (CvMat *)cvLoad("Distortion.xml");

	IplImage * mapx = cvCreateImage(cvGetSize(show_colie), IPL_DEPTH_32F, 1);
	IplImage * mapy = cvCreateImage(cvGetSize(show_colie), IPL_DEPTH_32F, 1);

	cvInitUndistortMap(intrinsic, distortion, mapx, mapy);

	cvNamedWindow("ԭʼͼ��", 1);
	cvNamedWindow("�ǻ���ͼ��", 1);

	cout << "����E�����˳���ʾ...\n";
	cout << "����W������ȡԭͼ...\n";
	cout << "����S������ȡУ��ͼ...\n";
	int capnums1 = 0;
	int capnums2 = 0;
	char capfilename[20] = " ";
	while (show_colie){
		IplImage * clone = cvCloneImage(show_colie);
		cvShowImage("ԭʼͼ��", show_colie);
		cvRemap(clone, show_colie, mapx, mapy);
		cvReleaseImage(&clone);
		cvShowImage("�ǻ���ͼ��", show_colie);

		if (cvWaitKey(10) == 'e'){
			break;
		}
	    if (cvWaitKey(10) == 's'){
			memset(capfilename, '\0', 20);
			sprintf_s(capfilename, "cap_dis_%d.jpg", capnums1);
			cvSaveImage(capfilename, show_colie);
			cout << "�ɹ���ȡ��ǰ֡�������ļ���" << capfilename << "����...\n";
			capnums1++;
		}
		VI.getPixels(deviceNum, frameBuffer, false, false);//����ʹ��videoInput���õķ�ת���ڴ��ͻ����δ���
		frame->imageData = (char*)frameBuffer;
		cvFlip(frame, show_colie, 0);//0��ֱ��ת1ˮƽ��ת-1��ֱˮƽ��ת
		if (cvWaitKey(10) == 'w'){
			memset(capfilename, '\0', 20);
			sprintf_s(capfilename, "cap_%d.jpg", capnums2);
			cvSaveImage(capfilename, show_colie);
			cout << "�ɹ���ȡ��ǰ֡�������ļ���" << capfilename << "����...\n";
			capnums2++;
		}
	}

	return 0;
}
int main2()//USB�ɼ�������
{
	int device1 = 0;
	videoInput VI;
	int numDevices = VI.listDevices();
	//VI.setUseCallback(true);
	VI.setIdealFramerate(device1, 60);
	VI.setupDevice(device1, 640, 480, VI_COMPOSITE);
	int width = VI.getWidth(device1);
	int height = VI.getHeight(device1);
	cout << "width=" << width << "\t" << "height=" << height << endl;
	VI.showSettingsWindow(0);//����������ʾ��Ƶ���ô��ڣ�����ȥ��
	IplImage* image = cvCreateImage(cvSize(width, height), 8, 3);
	IplImage* frame = cvCreateImage(cvSize(width, height), 8, 3);
	int framesize = VI.getSize(device1);
	cout << "framesize=" << framesize << endl;
	unsigned char* yourBuffer = (unsigned char*)malloc(framesize);
	cvNamedWindow("test");
	while (1)
	{
		VI.getPixels(device1, yourBuffer, false, false);//����ʹ��videoInput���õķ�ת���ڴ��ͻ����δ���
		image->imageData = (char*)yourBuffer;
		//cvConvertImage(image, image, CV_CVTIMG_FLIP);
		cvFlip(image,frame,0);//0��ֱ��ת1ˮƽ��ת-1��ֱˮƽ��ת
		cvShowImage("test", frame);
		if (cvWaitKey(15) == 27) break;
	}
	VI.stopDevice(device1);
	cvNamedWindow("test");
	cvReleaseImage(&image);
	return 0;
}

int main1()//���Ի�ȡUSB��������ͷ
{

	int cube_length = 0;

	CvCapture* capture;

	capture = cvCreateCameraCapture(0);

	if (capture == 0){
		printf("�޷���������ͷ�豸��\n\n");
		return 0;
	}
	else{
		printf("��������ͷ�豸�ɹ�����\n\n");
	}
	IplImage* frame;

	cvNamedWindow("�����֡��ȡ����", 1); //cvNamedWindow()������������Ļ�ϴ���һ�����ڣ�������ʾ��ͼ������ڸô����С�
			//�����ĵ�һ������ָ���˸ô��ڵĴ��ڱ���,���Ҫʹ��HighGUI�����ṩ������������ô��ڽ��н���ʱ�����ǽ�ͨ���ò���ֵ����������ڡ�


	printf("����C������ȡ��ǰ֡������Ϊ�궨ͼƬ...\n����Q�����˳���ȡ֡����...\n\n");

	int number_image = 1;
	char *str1;
	str1 = ".jpg";
	char filename[20] = "";
	while (true)
	{
		frame = cvQueryFrame(capture);// ������ͷ�����ļ���ץȡ������һ֡
		if (!frame)
		{
			cout << "frame err" <<endl;
			break;
		}
		cvShowImage("�����֡��ȡ����", frame); //ͼ����ʾ


		if (cvWaitKey(10) == 'c'){
			sprintf_s(filename, "%d.jpg", number_image); // int sprintf_s( char *buffer, size_t sizeOfBuffer, const char *format [, argument] ... );
					//�����������Ҫ�����ǽ����ɸ�argument����format��ʽ�浽buffer��



				cvSaveImage(filename, frame);//����
			cout << "�ɹ���ȡ��ǰ֡�������ļ���" << filename << "����...\n\n";
			printf("����C������ȡ��ǰ֡������Ϊ�궨ͼƬ...\n����Q�����˳���ȡ֡����...\n\n");
			number_image++;
		}
		else if (cvWaitKey(10) == 'q'){
			printf("��ȡͼ��֡�������...\n\n");
			cout << "���ɹ���ȡ" << --number_image << "֡ͼ�񣡣�\n\n";
			break;
		}
	}
	cvReleaseImage(&frame); //�ͷ�ͼ��

	cvDestroyWindow("�����֡��ȡ����");

	IplImage * show;
	cvNamedWindow("RePlay", 1);

	int a = 1;
	int number_image_copy = number_image;

	CvSize board_size = cvSize(10, 8); // Cvsizes:OpenCV�Ļ�����������֮һ����ʾ������С��������Ϊ���ȡ���CvPoint�ṹ���ƣ������ݳ�Ա��integer���͵�width��height��
	//cvSize��
	cube_length = board_size.width;
	int board_width = board_size.width;
	int board_height = board_size.height;
	int total_per_image = board_width*board_height;
	CvPoint2D32f * image_points_buf = new CvPoint2D32f[total_per_image];
	CvMat * image_points = cvCreateMat(number_image*total_per_image, 2, CV_32FC1);//ͼ������ϵ
	CvMat * object_points = cvCreateMat(number_image*total_per_image, 3, CV_32FC1);//��������ϵ
	CvMat * point_counts = cvCreateMat(number_image, 1, CV_32SC1);//
	CvMat * intrinsic_matrix = cvCreateMat(3, 3, CV_32FC1);//
	CvMat * distortion_coeffs = cvCreateMat(5, 1, CV_32FC1);

	int count;
	int found;
	int step;
	int successes = 0;

	while (a <= number_image_copy){
		sprintf_s(filename, "pans%d.jpg", a);
		show = cvLoadImage(filename, -1);

		found = cvFindChessboardCorners(show, board_size, image_points_buf, &count,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if (found == 0){
			cout << "��" << a << "֡ͼƬ�޷��ҵ����̸����нǵ�!\n\n";
			cvNamedWindow("RePlay", 1);
			cvShowImage("RePlay", show);
			cvWaitKey(0);

		}
		else{
			cout << "��" << a << "֡ͼ��ɹ����" << count << "���ǵ�...\n";

			cvNamedWindow("RePlay", 1);

			IplImage * gray_image = cvCreateImage(cvGetSize(show), 8, 1); //����ͷ����������IplImage* cvCreateImage( CvSize size, int depth, int channels ); depth ͼ��Ԫ�ص�λ���

			cvCvtColor(show, gray_image, CV_BGR2GRAY); // cvCvtColor(...),��Opencv�����ɫ�ռ�ת������������ʵ��rgb��ɫ��HSV,HSI����ɫ�ռ��ת����Ҳ����ת��Ϊ�Ҷ�ͼ��

			cout << "��ȡԴͼ��Ҷ�ͼ�������...\n";
			cvFindCornerSubPix(gray_image, image_points_buf, count, cvSize(11, 11), cvSize(-1, -1),// ���ڷǳ��ӽ�P�����ز����˺�С������ֵ�������������ؾ��󲢲����ǿ���ġ�
			//Ϊ�˽��������⣬һ����Լ򵥵��޳���P��ǳ��������ء��������:ero_zone������һ������(��win���ƣ���ͨ����winС)����������ڷ������Լ�����ؾ����в������ǡ��������Ҫ����һ����������zero_zoneӦ����ΪcvSize(-1�� - 1)0
				cvTermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
			cout << "�Ҷ�ͼ�����ػ��������...\n";
			cvDrawChessboardCorners(show, board_size, image_points_buf, count, found);
			cout << "��Դͼ���ϻ��ƽǵ�������...\n\n";
			cvShowImage("RePlay", show);
			cvWaitKey(0);
		}

		if (total_per_image == count){
			step = successes*total_per_image;
			for (int i = step, j = 0; j<total_per_image; ++i, ++j){
				CV_MAT_ELEM(*image_points, float, i, 0) = image_points_buf[j].x; // opencv���������ʾ���ÿ��Ԫ�صĺ꣬�����ֻ�Ե�ͨ��������Ч����ͨ��CV_MAT_ELEM( matrix, elemtype, row, col )��������matrix��Ҫ���ʵľ��󡡡�elemtype������Ԫ�ص����͡���row����Ҫ����Ԫ�ص���������col����Ҫ����Ԫ�ص�����

				CV_MAT_ELEM(*image_points, float, i, 1) = image_points_buf[j].y;// ����ÿ���ǵ��������ֵ������image_point_buf��
				CV_MAT_ELEM(*object_points, float, i, 0) = (float)(j / cube_length);
				CV_MAT_ELEM(*object_points, float, i, 1) = (float)(j%cube_length);
				CV_MAT_ELEM(*object_points, float, i, 2) = 0.0f;
			}
			CV_MAT_ELEM(*point_counts, int, successes, 0) = total_per_image;
			successes++;
		}
		a++;
	}
	cvReleaseImage(&show);
	cvDestroyWindow("RePlay");
	cout << "*********************************************\n";
	cout << number_image << "֡ͼƬ�У��궨�ɹ���ͼƬΪ" << successes << "֡...\n";
	cout << number_image << "֡ͼƬ�У��궨ʧ�ܵ�ͼƬΪ" << number_image - successes << "֡...\n\n";
	cout << "*********************************************\n\n";

	cout << "���������ʼ����������ڲ���...\n\n";


	CvCapture* capture1;
	capture1 = cvCreateCameraCapture(0);
	IplImage * show_colie;
	show_colie = cvQueryFrame(capture1);


	CvMat * object_points2 = cvCreateMat(successes*total_per_image, 3, CV_32FC1); // OpenCV ����Ҫ�ľ���任������ʹ�÷���ΪcvMat* cvCreateMat ( int rows, int cols, int type ); ����type�������κ�Ԥ�������ͣ�Ԥ�������͵Ľṹ���£�CV_<bit_depth> (S|U|F)C<number_of_channels>��

	CvMat * image_points2 = cvCreateMat(successes*total_per_image, 2, CV_32FC1);
	CvMat * point_counts2 = cvCreateMat(successes, 1, CV_32SC1);
	for (int i = 0; i<successes*total_per_image; ++i){
		CV_MAT_ELEM(*image_points2, float, i, 0) = CV_MAT_ELEM(*image_points, float, i, 0);//�����洢�ǵ���ȡ�ɹ���ͼ��Ľǵ�
		CV_MAT_ELEM(*image_points2, float, i, 1) = CV_MAT_ELEM(*image_points, float, i, 1);
		CV_MAT_ELEM(*object_points2, float, i, 0) = CV_MAT_ELEM(*object_points, float, i, 0);
		CV_MAT_ELEM(*object_points2, float, i, 1) = CV_MAT_ELEM(*object_points, float, i, 1);
		CV_MAT_ELEM(*object_points2, float, i, 2) = CV_MAT_ELEM(*object_points, float, i, 2);
	}

	for (int i = 0; i<successes; ++i){
		CV_MAT_ELEM(*point_counts2, int, i, 0) = CV_MAT_ELEM(*point_counts, int, i, 0);
	}


	cvReleaseMat(&object_points);
	cvReleaseMat(&image_points);
	cvReleaseMat(&point_counts);


	CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0) = 1.0f;
	CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1) = 1.0f;


	cvCalibrateCamera2(object_points2, image_points2, point_counts2, cvGetSize(show_colie),
		intrinsic_matrix, distortion_coeffs, NULL, NULL, 0);

	cout << "������ڲ�������Ϊ��\n";
	cout << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 0) << "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 1)
		<< "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 0, 2)
		<< "\n\n";
	cout << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 0) << "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 1)
		<< "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 1, 2)
		<< "\n\n";
	cout << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 0) << "    " << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 1)
		<< "          " << CV_MAT_ELEM(*intrinsic_matrix, float, 2, 2)
		<< "\n\n";

	cout << "����ϵ������Ϊ��\n";
	cout << CV_MAT_ELEM(*distortion_coeffs, float, 0, 0) << "    " << CV_MAT_ELEM(*distortion_coeffs, float, 1, 0)
		<< "    " << CV_MAT_ELEM(*distortion_coeffs, float, 2, 0)
		<< "    " << CV_MAT_ELEM(*distortion_coeffs, float, 3, 0)
		<< "    " << CV_MAT_ELEM(*distortion_coeffs, float, 4, 0)
		<< "\n\n";

	cvSave("Intrinsics.xml", intrinsic_matrix);
	cvSave("Distortion.xml", distortion_coeffs);

	cout << "��������󡢻���ϵ�������Ѿ��ֱ�洢����ΪIntrinsics.xml��Distortion.xml�ĵ���\n\n";

	CvMat * intrinsic = (CvMat *)cvLoad("Intrinsics.xml");
	CvMat * distortion = (CvMat *)cvLoad("Distortion.xml");

	IplImage * mapx = cvCreateImage(cvGetSize(show_colie), IPL_DEPTH_32F, 1);
	IplImage * mapy = cvCreateImage(cvGetSize(show_colie), IPL_DEPTH_32F, 1);

	cvInitUndistortMap(intrinsic, distortion, mapx, mapy);

	cvNamedWindow("ԭʼͼ��", 1);
	cvNamedWindow("�ǻ���ͼ��", 1);

	cout << "����E�����˳���ʾ...\n\n";

	while (show_colie){
		IplImage * clone = cvCloneImage(show_colie);
		cvShowImage("ԭʼͼ��", show_colie);
		cvRemap(clone, show_colie, mapx, mapy);
		cvReleaseImage(&clone);
		cvShowImage("�ǻ���ͼ��", show_colie);

		if (cvWaitKey(10) == 'e'){
			break;
		}

		show_colie = cvQueryFrame(capture1);
	}
	return 0;
}