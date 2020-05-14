#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main()
{
#if 0 //black
    cv::Mat intrinsic_matrix(3, 3, cv::DataType<float>::type); // Intrisic matrix
    intrinsic_matrix.at<float>(0, 0) = 318.1855538903441;//fx
    intrinsic_matrix.at<float>(1, 0) = 0;
    intrinsic_matrix.at<float>(2, 0) = 0;
	
    intrinsic_matrix.at<float>(0, 1) = 0;
    intrinsic_matrix.at<float>(1, 1) = 238.2719306588492;//fy
    intrinsic_matrix.at<float>(2, 1) = 0;
	
    intrinsic_matrix.at<float>(0, 2) = 640.7426046182887;//ux
    intrinsic_matrix.at<float>(1, 2) = 452.2179166682663;//uy
    intrinsic_matrix.at<float>(2, 2) = 1;


    cv::Mat distortion_coeffs(4, 1, cv::DataType<float>::type);   // Distortion vector
    distortion_coeffs.at<float>(0) = 0.0992428;//k1
    distortion_coeffs.at<float>(1) = -0.0159836;//k2
    distortion_coeffs.at<float>(2) = 0.0066086;//p1
    distortion_coeffs.at<float>(3) = -0.00178397;//p2

    double fx = intrinsic_matrix.at<float>(0, 0);
    double fy = intrinsic_matrix.at<float>(1, 1);
    double ux = intrinsic_matrix.at<float>(0, 2);
    double uy = intrinsic_matrix.at<float>(1, 2);

    double k1 = distortion_coeffs.at<float>(0);
    double k2 = distortion_coeffs.at<float>(1);
    double p1 = distortion_coeffs.at<float>(2);
    double p2 = distortion_coeffs.at<float>(3);
    double k3,k4,k5,k6;

    Point black[4];
    black[0].x = 600;black[0].y = 319;
    black[1].x = 727;black[1].y = 319;
    black[2].x = 758;black[2].y = 387;
    black[3].x = 578;black[3].y = 385;
    for(int i=0;i<4;i++)
    {
    //获取的点通常是图像的像素点，所以需要先通过小孔相机模型转换到归一化坐标系下；
    double xCorrected = (double)((black[i].x - ux) / fx);
    double yCorrected = (double)((black[i].y - uy) / fy);
    //cout<<"xCorrected:"<<xCorrected<<"   "<<"yCorrected:"<<yCorrected<<endl;
    double xDistortion, yDistortion;

     //我们已知的是经过畸变矫正或理想点的坐标；
    double r2 = xCorrected*xCorrected + yCorrected*yCorrected;

    double deltaRa = 1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2;
    double deltaRb = 1 / (1. + k4 * r2 + k5 * r2 * r2 + k6 * r2 * r2 * r2);
    double deltaTx = 2. * p1 * xCorrected * yCorrected + p2 * (r2 + 2. * xCorrected * xCorrected);
    double deltaTy = p1 * (r2 + 2. * yCorrected * yCorrected) + 2. * p2 * xCorrected * yCorrected;
    //下面为畸变模型；
    xDistortion = xCorrected * deltaRa * deltaRb + deltaTx;
    yDistortion = yCorrected * deltaRa * deltaRb + deltaTy;

    //最后再次通过相机模型将归一化的坐标转换到像素坐标系下；
     xDistortion = xDistortion * fx + ux;
     yDistortion = yDistortion * fy + uy;
     cout<<"xDistortion:"<<xDistortion<<"   "<<"yDistortion:"<<yDistortion<<endl;
    }
#endif
#if 0//top
    cv::Mat intrinsic_matrix(3, 3, cv::DataType<float>::type); // Intrisic matrix
    intrinsic_matrix.at<float>(0, 0) = 325.1640457188606;//fx
    intrinsic_matrix.at<float>(1, 0) = 0;
    intrinsic_matrix.at<float>(2, 0) = 0;
	
    intrinsic_matrix.at<float>(0, 1) = 0;
    intrinsic_matrix.at<float>(1, 1) = 242.9263076239038;//fy
    intrinsic_matrix.at<float>(2, 1) = 0;
	
    intrinsic_matrix.at<float>(0, 2) = 646.0638739464076;//ux
    intrinsic_matrix.at<float>(1, 2) = 459.4015922931151;//uy
    intrinsic_matrix.at<float>(2, 2) = 1;


    cv::Mat distortion_coeffs(4, 1, cv::DataType<float>::type);   // Distortion vector
    distortion_coeffs.at<float>(0) = 0.100413;//k1
    distortion_coeffs.at<float>(1) = -0.0628999;//k2
    distortion_coeffs.at<float>(2) = 0.06066;//p1
    distortion_coeffs.at<float>(3) = -0.0192776;//p2

    double fx = intrinsic_matrix.at<float>(0, 0);
    double fy = intrinsic_matrix.at<float>(1, 1);
    double ux = intrinsic_matrix.at<float>(0, 2);
    double uy = intrinsic_matrix.at<float>(1, 2);

    double k1 = distortion_coeffs.at<float>(0);
    double k2 = distortion_coeffs.at<float>(1);
    double p1 = distortion_coeffs.at<float>(2);
    double p2 = distortion_coeffs.at<float>(3);
    double k3,k4,k5,k6;

    Point top[4];
    top[0].x = 586;top[0].y = 339;
    top[1].x = 720;top[1].y = 342;
    top[2].x = 753;top[2].y = 406;
    top[3].x = 545;top[3].y = 404;
    for(int i=0;i<4;i++)
    {
    //获取的点通常是图像的像素点，所以需要先通过小孔相机模型转换到归一化坐标系下；
    double xCorrected = (double)((top[i].x - ux) / fx);
    double yCorrected = (double)((top[i].y - uy) / fy);
    //cout<<"xCorrected:"<<xCorrected<<"   "<<"yCorrected:"<<yCorrected<<endl;
    double xDistortion, yDistortion;

     //我们已知的是经过畸变矫正或理想点的坐标；
    double r2 = xCorrected*xCorrected + yCorrected*yCorrected;

    double deltaRa = 1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2;
    double deltaRb = 1 / (1. + k4 * r2 + k5 * r2 * r2 + k6 * r2 * r2 * r2);
    double deltaTx = 2. * p1 * xCorrected * yCorrected + p2 * (r2 + 2. * xCorrected * xCorrected);
    double deltaTy = p1 * (r2 + 2. * yCorrected * yCorrected) + 2. * p2 * xCorrected * yCorrected;
    //下面为畸变模型；
    xDistortion = xCorrected * deltaRa * deltaRb + deltaTx;
    yDistortion = yCorrected * deltaRa * deltaRb + deltaTy;

    //最后再次通过相机模型将归一化的坐标转换到像素坐标系下；
     xDistortion = xDistortion * fx + ux;
     yDistortion = yDistortion * fy + uy;
     cout<<"xDistortion:"<<xDistortion<<"   "<<"yDistortion:"<<yDistortion<<endl;
    }
#endif
#if 0//left
    cv::Mat intrinsic_matrix(3, 3, cv::DataType<float>::type); // Intrisic matrix
    intrinsic_matrix.at<float>(0, 0) = 313.6229712767807;//fx
    intrinsic_matrix.at<float>(1, 0) = 0;
    intrinsic_matrix.at<float>(2, 0) = 0;
	
    intrinsic_matrix.at<float>(0, 1) = 0;
    intrinsic_matrix.at<float>(1, 1) = 234.3509169632622;//fy
    intrinsic_matrix.at<float>(2, 1) = 0;
	
    intrinsic_matrix.at<float>(0, 2) = 639.4050489359947;//ux
    intrinsic_matrix.at<float>(1, 2) = 453.6614601277603;//uy
    intrinsic_matrix.at<float>(2, 2) = 1;


    cv::Mat distortion_coeffs(4, 1, cv::DataType<float>::type);   // Distortion vector
    distortion_coeffs.at<float>(0) = 0.1156;//k1
    distortion_coeffs.at<float>(1) = -0.0639899;//k2
    distortion_coeffs.at<float>(2) = 0.0724575;//p1
    distortion_coeffs.at<float>(3) = -0.0336209;//p2

    double fx = intrinsic_matrix.at<float>(0, 0);
    double fy = intrinsic_matrix.at<float>(1, 1);
    double ux = intrinsic_matrix.at<float>(0, 2);
    double uy = intrinsic_matrix.at<float>(1, 2);

    double k1 = distortion_coeffs.at<float>(0);
    double k2 = distortion_coeffs.at<float>(1);
    double p1 = distortion_coeffs.at<float>(2);
    double p2 = distortion_coeffs.at<float>(3);
    double k3,k4,k5,k6;

    Point left[4];
    left[0].x = 605;left[0].y = 323;
    left[1].x = 716;left[1].y = 320;
    left[2].x = 739;left[2].y = 367;
    left[3].x = 586;left[3].y = 370;
    for(int i=0;i<4;i++)
    {
    //获取的点通常是图像的像素点，所以需要先通过小孔相机模型转换到归一化坐标系下；
    double xCorrected = (double)((left[i].x - ux) / fx);
    double yCorrected = (double)((left[i].y - uy) / fy);
    //cout<<"xCorrected:"<<xCorrected<<"   "<<"yCorrected:"<<yCorrected<<endl;
    double xDistortion, yDistortion;

     //我们已知的是经过畸变矫正或理想点的坐标；
    double r2 = xCorrected*xCorrected + yCorrected*yCorrected;

    double deltaRa = 1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2;
    double deltaRb = 1 / (1. + k4 * r2 + k5 * r2 * r2 + k6 * r2 * r2 * r2);
    double deltaTx = 2. * p1 * xCorrected * yCorrected + p2 * (r2 + 2. * xCorrected * xCorrected);
    double deltaTy = p1 * (r2 + 2. * yCorrected * yCorrected) + 2. * p2 * xCorrected * yCorrected;
    //下面为畸变模型；
    xDistortion = xCorrected * deltaRa * deltaRb + deltaTx;
    yDistortion = yCorrected * deltaRa * deltaRb + deltaTy;

    //最后再次通过相机模型将归一化的坐标转换到像素坐标系下；
     xDistortion = xDistortion * fx + ux;
     yDistortion = yDistortion * fy + uy;
     cout<<"xDistortion:"<<xDistortion<<"   "<<"yDistortion:"<<yDistortion<<endl;
    }
#endif
#if 1//right
    cv::Mat intrinsic_matrix(3, 3, cv::DataType<float>::type); // Intrisic matrix
    intrinsic_matrix.at<float>(0, 0) = 304.2478261102241;//fx
    intrinsic_matrix.at<float>(1, 0) = 0;
    intrinsic_matrix.at<float>(2, 0) = 0;
	
    intrinsic_matrix.at<float>(0, 1) = 0;
    intrinsic_matrix.at<float>(1, 1) = 227.6965665478303;//fy
    intrinsic_matrix.at<float>(2, 1) = 0;
	
    intrinsic_matrix.at<float>(0, 2) = 637.6816254038803;//ux
    intrinsic_matrix.at<float>(1, 2) = 464.6615939851297;//uy
    intrinsic_matrix.at<float>(2, 2) = 1;


    cv::Mat distortion_coeffs(4, 1, cv::DataType<float>::type);   // Distortion vector
    distortion_coeffs.at<float>(0) = 0.0667328;//k1
    distortion_coeffs.at<float>(1) = 0.0842976;//k2
    distortion_coeffs.at<float>(2) = -0.0515683;//p1
    distortion_coeffs.at<float>(3) = 0.00889971;//p2

    double fx = intrinsic_matrix.at<float>(0, 0);
    double fy = intrinsic_matrix.at<float>(1, 1);
    double ux = intrinsic_matrix.at<float>(0, 2);
    double uy = intrinsic_matrix.at<float>(1, 2);

    double k1 = distortion_coeffs.at<float>(0);
    double k2 = distortion_coeffs.at<float>(1);
    double p1 = distortion_coeffs.at<float>(2);
    double p2 = distortion_coeffs.at<float>(3);
    double k3,k4,k5,k6;

    Point right[4];
    right[0].x = 595;right[0].y = 358;
    right[1].x = 700;right[1].y = 361;
    right[2].x = 721;right[2].y = 409;
    right[3].x = 572;right[3].y = 404;
    for(int i=0;i<4;i++)
    {
    //获取的点通常是图像的像素点，所以需要先通过小孔相机模型转换到归一化坐标系下；
    double xCorrected = (double)((right[i].x - ux) / fx);
    double yCorrected = (double)((right[i].y - uy) / fy);
    //cout<<"xCorrected:"<<xCorrected<<"   "<<"yCorrected:"<<yCorrected<<endl;
    double xDistortion, yDistortion;

     //我们已知的是经过畸变矫正或理想点的坐标；
    double r2 = xCorrected*xCorrected + yCorrected*yCorrected;

    double deltaRa = 1. + k1 * r2 + k2 * r2 * r2 + k3 * r2 * r2 * r2;
    double deltaRb = 1 / (1. + k4 * r2 + k5 * r2 * r2 + k6 * r2 * r2 * r2);
    double deltaTx = 2. * p1 * xCorrected * yCorrected + p2 * (r2 + 2. * xCorrected * xCorrected);
    double deltaTy = p1 * (r2 + 2. * yCorrected * yCorrected) + 2. * p2 * xCorrected * yCorrected;
    //下面为畸变模型；
    xDistortion = xCorrected * deltaRa * deltaRb + deltaTx;
    yDistortion = yCorrected * deltaRa * deltaRb + deltaTy;

    //最后再次通过相机模型将归一化的坐标转换到像素坐标系下；
     xDistortion = xDistortion * fx + ux;
     yDistortion = yDistortion * fy + uy;
     double xDistortion_last,yDistortion_last;
     cout<<"xDistortion:"<<xDistortion<<"   "<<"yDistortion:"<<yDistortion<<endl;
    }
    
    
#endif
}
