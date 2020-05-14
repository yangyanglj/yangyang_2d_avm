/*
相机内参数矩阵：
[448.5792994463924, 0, 636.5047511641519;
 0, 338.7300360911764, 362.1045264912111;
 0, 0, 1]

畸变系数：
[-0.3569055798505001, 0.1139294202113153, -0.0003076439066842473, 9.132627897464701e-05, -0.01424420569257568]
*/
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
using namespace cv;
using namespace std;

string image_file = "./1.JPG";

int main()
{
//畸变参数
    double k1 = -0.3569055798505001,k2 = 0.1139294202113153,p1 = -0.0003076439066842473,p2 = 
9.132627897464701e-05,k3 = -0.01424420569257568;
//内参
    double fx = 448.5792994463924,fy = 636.5047511641519,cx = 338.7300360911764,cy = 362.1045264912111;
    Mat image = imread(image_file);
    
    cvtColor(image, image, CV_BGR2GRAY);
   // resize(image, image, cv::Size(640, 360));
    int rows = image.rows, cols = image.cols;
    Mat image_undistort = Mat(rows,cols,image.type());
     // 计算去畸变后图像的内容
    for (int v = 0; v < rows; v++)
    {
	for (int u = 0; u < cols; u++)
	{
	    double u_distorted = 0, v_distorted = 0;
	      // TODO 按照公式，计算点(u,v)对应到畸变图像中的坐标(u_distorted, v_distorted)
		//image_undistort中含有非畸变的图像坐标
	      double x1, y1, x2, y2;
	    x1 = (u - cx) / fx;
	    y1 = (v - cy) / fy;
	    double r2;
	    r2 = pow(x1, 2) + pow(y1, 2);
	    x2 = x1*(1 + k1*r2 + k2*pow(r2, 2) + k3 * r2 * r2 * r2) + 2 * p1*x1*y1 + p2*(r2 + 2 * x1*x1);
	    y2 = y1*(1 + k1*r2 + k2*pow(r2, 2) + k3 * r2 * r2 * r2) + p1*(r2 + 2 * y1*y1) + 2 * p2*x1*y1;
	    u_distorted = fx*x2 + cx;
	    v_distorted = fy*y2 + cy;

	    if (u_distorted >= 0 && v_distorted >= 0 && u_distorted < cols && v_distorted < rows)
	    {
		image_undistort.at<uchar>(v, u) = image.at<uchar>((int)v_distorted, (int)u_distorted);
	    }else {
    		image_undistort.at<uchar>(v, u) = 0;
  	    }
	}
    }

    imshow("image_file", image_undistort);
    waitKey(0);
}






























