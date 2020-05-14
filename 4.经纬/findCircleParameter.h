#pragma once
#include "globalInclude.h"

class findCircleParameter
{
private:
	findCircleParameter();
	~findCircleParameter();

	//Ϊȷ������Բ����Ч������������õĻص���������OpenCVʹ��
	static void On_N_trackbar(int N_slider_value, void* params);
	static void On_threshold_trackbar(int thresholdValue_slider_value, void* params);
	static void revisedScanLineMethod(Mat imgOrg, Point2i& center, int& radius, int threshold, int N);
	static bool CircleFitByKasa(vector<Point> validPoints, Point& center, int&	 radius);
	static void onMouse(int event, int x, int y, int, void* args);
	static void findCircleParameter::findPoints(Point2i center, int radius, std::vector<cv::Point> &points, camMode projMode = ORTHOGONAL);

public:
	static bool init(Mat img);
	static void findCircle();
	static void checkVarify();
	static bool getCircleParatemer(Point2i& center, int& radius);
private:
	/*��ȡԲ����*/
	static string win_name;
	static string N_trackbar_name;
	static string thresholdValue_trackbar_name;
	static int N_slider_value;
	static int N_max_value;
	static int thresholdValue_slider_value;
	static int thresholdValue_max_value;
	static Point2i center;
	static int radius;
	static Mat image;

	/*��֤Բ*/
	static std::vector<std::vector<cv::Point> > lines;
	static std::vector<cv::Point> points;
	static std::string check_win_name;

	static int width_disp_img;
	static int height_disp_img;
public:
	static const double FOV;
};

