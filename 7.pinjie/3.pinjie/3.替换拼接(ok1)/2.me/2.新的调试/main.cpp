#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
 
using namespace cv;
using namespace std;
 
void rotate_arbitrarily_angle(Mat &src,Mat &dst,float angle)
{
    float radian = (float)(angle/180.0 * CV_PI);
//旋转填充图像
    int maxBorder = (int) (max(src.cols, src.rows)* 1.414 );
    int dx = (maxBorder - src.cols)/2;
    int dy = (maxBorder - src.rows)/2;
    copyMakeBorder(src, dst, dy, dy, dx, dx, BORDER_CONSTANT);
    //旋转
     Point2f center( (float)(dst.cols/2) , (float) (dst.rows/2));
    Mat affine_matrix = getRotationMatrix2D( center, angle, 1.0 );//求得旋转矩阵
    warpAffine(dst, dst, affine_matrix, dst.size());
    //计算图像旋转之后包含图像的最大的矩形
    float sinVal = abs(sin(radian));
    float cosVal = abs(cos(radian));
    Size targetSize( (int)(src.cols * cosVal +src.rows * sinVal),
                     (int)(src.cols * sinVal + src.rows * cosVal) );
 
    //剪掉多余边框
    int x = (dst.cols - targetSize.width) / 2;
    int y = (dst.rows - targetSize.height) / 2;
    Rect rect(x, y, targetSize.width, targetSize.height);
    dst = Mat(dst,rect);
}
  
 
int main(int argc, char** argv)
{
 
	// Read in the image.
	//Mat im_src = imread("first-image.jpg");
	Mat im_src = imread("result.jpg");
	Size size = im_src.size();
 
	// Create a vector of points.
	
 	Mat im_dst = imread("top_resize.jpg");
	//Mat im_black = imread("black.jpg");
	//Mat im_right = imread("right.jpg");
	//Mat img_left = imread("left.jpg");
	Mat black_rotate,right_rotate,left_rotate;
	//rotate_arbitrarily_angle(im_black,black_rotate,180);
	//rotate_arbitrarily_angle(im_right,right_rotate,-90);
	//rotate_arbitrarily_angle(img_left,left_rotate,90);

 	//top
	//userdata data_top;

	vector<Point2f> pts_src;
	pts_src.push_back(Point2f(103.9, 330.1));
	pts_src.push_back(Point2f(242.45, 321.59));
	pts_src.push_back(Point2f(242.45, 330.1));
	pts_src.push_back(Point2f(103.9, 321.59));
	circle(im_src, Point2f(103.9, 330.1), 1, Scalar(0, 255, 255), 5, CV_AA);
	circle(im_src, Point2f(242.45, 321.59), 1, Scalar(0, 255, 255), 5, CV_AA);
	circle(im_src, Point2f(242.45, 330.1), 1, Scalar(0, 255, 255), 5, CV_AA);
	circle(im_src, Point2f(103.9, 321.59), 1, Scalar(0, 255, 255), 5, CV_AA);

	vector<Point2f> points_top(4);
	points_top[0] = Point(586.039/4,346.715/4);
	points_top[1] = Point(714.968/4, 351.354/4);
	points_top[2] = Point(749.327/4, 409.65/4);
	points_top[3] = Point(544.286/4, 406.309/4);

	// Destination image
	//Mat im_dst = imread("times-square.jpg");
	
 
 
	// Set data for mouse handler
	Mat im_temp = im_dst.clone();
	
	//userdata data;
	//data.im = im_temp;
 	
 
	
 
	Mat H = findHomography(pts_src, points_top,0);
	
	

	Mat logo;
	warpPerspective(im_src, logo,H,im_dst.size());
	imshow("Image1", im_src);
	//阈值化
	Mat logo1;
	threshold(logo,logo1,0,255,1);
 
	im_dst.copyTo(logo,logo1);//把im_dst和logo1做 与 运算后再和logo相加
	

 	
	//imwrite("final.jpg",logo);
	//imwrite("final_black.jpg",logo_black);
 
	// Display image.
 
 
	imshow("Image", logo);
	
#if 0
//black
	// 定义两个平面上四个角坐标
	vector<Point> src_black_corners(4);
	vector<Point> dst_black_corners(4);
	src_black_corners[0] = Point(logo.cols/3, logo.rows/3*2);

	src_black_corners[1] = Point(logo.cols/3*2, logo.rows/3*2);

	src_black_corners[2] = Point(logo.cols, logo.rows);

	src_black_corners[3] = Point(0, logo.rows);
	// 目标平面四个角坐标

	dst_black_corners[0] = Point(black_rotate.cols/3,black_rotate.rows/3*2);

	dst_black_corners[1] = Point(black_rotate.cols/3*2, black_rotate.rows/3*2);

	dst_black_corners[2] = Point(black_rotate.cols, black_rotate.rows);

	dst_black_corners[3] = Point(0, black_rotate.rows);

	Mat im_temp_black = black_rotate.clone();

	Mat H_black = findHomography(src_black_corners, dst_black_corners,0);

	Mat logo_black;
	warpPerspective(logo, logo_black,H_black,black_rotate.size());

	Mat logo1_black;
	threshold(logo_black,logo1_black,0,255,1);

	black_rotate.copyTo(logo_black,logo1_black);
	//imshow("Image_black", logo_black);


//right
	vector<Point> src_right_corners(4);
	vector<Point> dst_right_corners(4);
	src_right_corners[0] = Point(logo_black.cols/3*2, logo_black.rows/3);

	src_right_corners[1] = Point(logo_black.cols, 0);

	src_right_corners[2] = Point(logo_black.cols, logo_black.rows);

	src_right_corners[3] = Point(logo_black.cols/3*2, logo_black.rows/3*2);
	// 目标平面四个角坐标

	dst_right_corners[0] = Point(right_rotate.cols/3*2, right_rotate.rows/3);

	dst_right_corners[1] = Point(right_rotate.cols, 0);

	dst_right_corners[2] = Point(right_rotate.cols, right_rotate.rows);

	dst_right_corners[3] = Point(right_rotate.cols/3*2,right_rotate.rows/3*2);//1280-720


	Mat im_temp_right = right_rotate.clone();

	Mat H_right = findHomography(src_right_corners, dst_right_corners,0);

	Mat logo_right;
	warpPerspective(logo_black, logo_right,H_right,right_rotate.size());

	Mat logo1_right;
	threshold(logo_right,logo1_right,0,255,1);

	right_rotate.copyTo(logo_right,logo1_right);
	//imshow("logo_right",logo_right);


// left
	vector<Point> src_left_corners(4);
	vector<Point> dst_left_corners(4);
	src_left_corners[0] = Point(0, 0);

	src_left_corners[1] = Point(logo_right.cols/3, logo_right.rows/3);

	src_left_corners[2] = Point(logo_right.cols/3, logo_right.rows/3*2);

	src_left_corners[3] = Point(0, logo_right.rows);
	// 目标平面四个角坐标

	dst_left_corners[0] = Point(0, 0);

	dst_left_corners[1] = Point(left_rotate.cols/3, left_rotate.rows/3);

	dst_left_corners[2] = Point(left_rotate.cols/3, left_rotate.rows/3*2);

	dst_left_corners[3] = Point(0,left_rotate.rows);//1280-720

	cout<<"***"<<endl;
	Mat im_temp_left = left_rotate.clone();

	Mat H_left = findHomography(src_left_corners, dst_left_corners,0);
cout<<"***"<<endl;
	Mat logo_left;
	warpPerspective(logo_right, logo_left,H_left,left_rotate.size());
	cout<<"***"<<endl;
	Mat logo1_left;
	threshold(logo_left,logo1_left,0,255,1);

	left_rotate.copyTo(logo_left,logo1_left);

	imshow("logo_left",logo_left);
	imwrite("pinjie.jpg",logo_left);
#endif
	waitKey(0);
 
	return 0;
}
