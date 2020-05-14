#include<opencv2/opencv.hpp>
using namespace cv;
using namespace std;

struct userdata {
	Mat im;
	//vector<Point2f> points;
	Point2f points[4];
};


void mouseHandler(int event, int x, int y, int flags, void* data_ptr)
{
	if (event == EVENT_LBUTTONDOWN)
	{
		userdata *data = ((userdata *)data_ptr);
		circle(data->im, Point(x, y), 3, Scalar(0, 255, 255), 5, CV_AA);
		imshow("Image", data->im);
		if (data->points.size() < 4)
		{
			//data->points.push_back(Point2f(x, y));
			data->points = Point2f(x, y);
			//cout<<data->points[0] <<endl;
		}
	}
 
}

int main()
{
    float mul = 1.0;
    Mat birdImage;
    int board_w = 13;
    int board_h = 9;
    int board_n = board_w*board_h;
    Size board_sz = Size(13, 9);
    Mat src = imread("img1.jpg");

    Size image_size = src.size();
    Size size = Size(int(1280*mul),int(720*mul));
    Mat mapx = Mat(size, CV_32FC1);
    Mat mapy = Mat(size, CV_32FC1);
    Mat R = Mat::eye(3, 3, CV_32F);
/*
https://blog.csdn.net/fireflychh/article/details/82462197
*/
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

    Mat intrinsic_mat(intrinsic_matrix), new_intrinsic_mat;
    intrinsic_mat.copyTo(new_intrinsic_mat);
    //调节视场大小,乘的系数越小视场越大
    new_intrinsic_mat.at<float>(0, 0) *= 0.6*mul;
    new_intrinsic_mat.at<float>(1, 1) *= 0.6*mul;
    //调节校正图中心，建议置于校正图中心
    new_intrinsic_mat.at<float>(0, 2) = 0.5 *mul * src.cols;
    new_intrinsic_mat.at<float>(1, 2) = 0.5 *mul * src.rows;

    fisheye::initUndistortRectifyMap(intrinsic_matrix,distortion_coeffs,R,new_intrinsic_mat, size,CV_32FC1,mapx,mapy);
    Mat src1;
    remap(src, src1, mapx, mapy, INTER_LINEAR);

    Mat grayImage;
    cvtColor(src1, grayImage, CV_BGR2GRAY);
    //Rect rect(0,1*mul,1280*mul, 600*mul);
    Mat temp(src1);
    Size temp_size = temp.size();
    //imshow("remap", src1);
    //imshow("roi", temp);

    Mat im_temp = src.clone();
    userdata data;
    data.im = im_temp;
    imshow("Image", im_temp);
    setMouseCallback("Image", mouseHandler, &data);
   
#if 0
    vector<Point2f> corners;
    //寻找4组对应点坐标
    bool found = findChessboardCorners(temp, board_sz, corners, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK);
    cout << "found == 1:" << found << endl;
    if (!found)
    {
         cout << "找不到角点，需删除图片文件,重新排列文件名，再次标定" << endl;
    }
    else
    {
          // 亚像素精确化
         cornerSubPix(grayImage, corners, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));
          
    }
     cout << corners << endl;
#endif
  // drawChessboardCorners(temp, board_sz, corners, found);
   Point2f imagePoints[4], objectPoints[4];
  
  // imshow("chess", temp);
#if 1

    Mat quad = Mat::zeros(1280, 1280, CV_8UC3);
#if 0
    imagePoints[3] = corners.at(0) ;
    imagePoints[2] = corners.at(board_w - 1) ;
    imagePoints[1] = corners.at(board_w*(board_h - 1)) ;
    imagePoints[0] = corners.at(board_n - 1) ;
#endif
     cout << data.points << endl;
    imagePoints[0].x = 100 ;imagePoints[0].y = 200 ;
    imagePoints[1].x = 200 ;imagePoints[0].y = 300 ;
    imagePoints[2].x = 300 ;imagePoints[0].y = 400 ;
    imagePoints[3].x = 400 ;imagePoints[0].y = 500 ;

    objectPoints[0].x = 0;  objectPoints[0].y = 0;
    objectPoints[1].x = quad.cols - 1;    objectPoints[1].y = 0;
    objectPoints[2].x = quad.cols-1;  objectPoints[2].y = quad.rows-1;
    objectPoints[3].x = 0;  objectPoints[3].y = quad.rows-1;

  //  circle(temp, imagePoints[0], 5, Scalar(0, 0, 255),-1);
  //  circle(temp, imagePoints[1], 5, Scalar(0, 255, 0), -1);
  //  circle(temp, imagePoints[2], 5, Scalar(255, 0, 0), -1);
  //   circle(temp, imagePoints[3], 5, Scalar(255, 255, 255), -1);


    Mat H(3, 3, cv::DataType<double>::type); // Intrisic matrix
     cout << objectPoints[0] << imagePoints[0] << endl;
    cout << objectPoints[1] << imagePoints[1] << endl;
    cout << objectPoints[2] << imagePoints[2] << endl;
    cout << objectPoints[3] << imagePoints[3] << endl;
    //imshow("chess", temp);

    H = getPerspectiveTransform(imagePoints, objectPoints);
   warpPerspective(temp, quad, H, quad.size() , WARP_INVERSE_MAP+INTER_LINEAR);
   imshow("birdImage", quad);
#endif
    waitKey(0);
}
































