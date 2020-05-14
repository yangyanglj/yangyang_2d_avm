#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <iostream>
#include <math.h>
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
	Mat src = imread("result.jpg");
	if (!src.data) {
	printf("could not load image...\n");
	return -1;
	}
	Mat replaceImg_top = imread("top.jpg");
	Mat replaceImg_black = imread("black.jpg");
	Mat replaceImg_right = imread("right.jpg");
	Mat replaceImg_left = imread("left.jpg");
	Mat black_rotate,right_rotate,left_rotate;
	rotate_arbitrarily_angle(replaceImg_black,black_rotate,180);
	rotate_arbitrarily_angle(replaceImg_right,right_rotate,-90);
	rotate_arbitrarily_angle(replaceImg_right,left_rotate,90);
	// resize(right_rotate, right_rotate, Size(),0.5,0.5);//我长宽都变为原来的0.5倍
	//imshow("black_rotate",black_rotate);
	//imshow("right_rotate",right_rotate);
	// resize(right_rotate, right_rotate, Size(),0.5,0.5);//我长宽都变为原来的0.5倍
	//imwrite("left_rotate.jpg",left_rotate);
//top
#if 1
	// 定义两个平面上四个角坐标
	vector<Point> src_corners(4);
	vector<Point> dst_corners(4);
	src_corners[0] = Point(0, 0);

	src_corners[1] = Point(src.cols-1, 0);

	src_corners[2] = Point(src.cols/3*2, src.rows/3);

	src_corners[3] = Point(src.cols/3, src.rows/3);
	// 目标平面四个角坐标

	dst_corners[0] = Point(0,0);

	dst_corners[1] = Point(replaceImg_top.cols-1, 0);

	dst_corners[2] = Point(replaceImg_top.cols/3*2, replaceImg_top.rows/3);

	dst_corners[3] = Point(replaceImg_top.cols/3, replaceImg_top.rows/3);


//right
	vector<Point> src_right_corners(4);
	vector<Point> dst_right_corners(4);
	src_right_corners[0] = Point(src.cols/3*2, src.rows/3);

	src_right_corners[1] = Point(src.cols, 0);

	src_right_corners[2] = Point(src.cols, src.rows);

	src_right_corners[3] = Point(src.cols/3*2, src.rows/3*2);
	// 目标平面四个角坐标

	dst_right_corners[0] = Point(right_rotate.cols, 0);

	dst_right_corners[1] = Point(right_rotate.cols, right_rotate.rows);

	dst_right_corners[2] = Point(right_rotate.cols/3*2+560, right_rotate.rows/3*2);

	dst_right_corners[3] = Point(right_rotate.cols/3*2+560,right_rotate.rows/3);//1280-720


#endif
//left
	vector<Point> src_left_corners(4);
	vector<Point> dst_left_corners(4);
	src_left_corners[0] = Point(0, 0);

	src_left_corners[1] = Point(src.cols/3, src.rows/3);

	src_left_corners[2] = Point(src.cols/3, src.rows/3*2);

	src_left_corners[3] = Point(0, src.rows);
	// 目标平面四个角坐标

	dst_left_corners[0] = Point(0, 0);

	dst_left_corners[1] = Point(left_rotate.cols, 0);

	dst_left_corners[2] = Point(left_rotate.cols/3+560, left_rotate.rows/3*2);

	dst_left_corners[3] = Point(left_rotate.cols/3+560,left_rotate.rows/3);//1280-720
//top

	// 计算单应性矩阵与透视变换
	Mat h = findHomography(src_corners, dst_corners);
	Mat output_img;
	warpPerspective(replaceImg_top, output_img, h, src.size());
	
	// create mask
	Mat m1 = Mat::zeros(replaceImg_top.size(), CV_8UC1);
	m1 = Scalar(255);
	Mat mask_output;
	warpPerspective(m1, mask_output, h, src.size());
	//imshow("Mask1 Result", mask_output);
	// use mask
	Mat result1;
	add(output_img, output_img, result1, mask_output);
	
	Mat result2;
	bitwise_not(mask_output, mask_output);
	
	add(src, result1, result2, mask_output);
	
	Mat result;
	add(result1, result2, result);
#if 0
	//black
	// 定义两个平面上四个角坐标
	vector<Point> src_black_corners(4);
	vector<Point> dst_black_corners(4);
	src_black_corners[0] = Point(result.cols/3, result.rows/3*2);

	src_black_corners[1] = Point(result.cols/3*2, result.rows/3*2);

	src_black_corners[2] = Point(result.cols, result.rows);

	src_black_corners[3] = Point(0, result.rows);
	// 目标平面四个角坐标

	dst_black_corners[0] = Point(black_rotate.cols/3,black_rotate.rows/3*2);

	dst_black_corners[1] = Point(black_rotate.cols/3*2, black_rotate.rows/3*2);

	dst_black_corners[2] = Point(black_rotate.cols, black_rotate.rows);

	dst_black_corners[3] = Point(0, black_rotate.rows);	
//black
	// 计算单应性矩阵与透视变换
	Mat h_black = findHomography(src_black_corners, dst_black_corners);

	Mat output_img_black;

	warpPerspective(black_rotate, output_img_black, h_black, result.size());
	//imshow("Mask1 Result", output_img_black);
	// create mask
	Mat m1_black = Mat::zeros(black_rotate.size(), CV_8UC1);
	m1_black = Scalar(255);
	Mat mask_output_black;
	warpPerspective(m1_black, mask_output_black, h_black, result.size());
	// use mask
	Mat result1_black;
	add(output_img_black, output_img_black, result1_black, mask_output_black);
	Mat result2_black;
	bitwise_not(mask_output_black, mask_output_black);
	add(result, result1_black, result2_black, mask_output_black);
	//Mat result_black;
	add(result1_black, result2_black, result);
	

//right
	Mat h_right = findHomography(src_right_corners, dst_right_corners);
	Mat output_img_right;
	warpPerspective(right_rotate, output_img_right, h_right, right_rotate.size());
	// create mask
	Mat m1_right = Mat::zeros(right_rotate.size(), CV_8UC1);
	m1_right = Scalar(255);
	Mat mask_output_right;
	warpPerspective(m1_right, mask_output_right, h_right, right_rotate.size());
	// use mask
	Mat result1_right;
	add(output_img_right, output_img_right, result1_right, mask_output_right);
	Mat result2_right;
	bitwise_not(mask_output_right, mask_output_right);
	add(right_rotate, result1_right, result2_right, mask_output_right);
	Mat result_right;
	add(result1_right, result2_right, result_right);

	//imshow("Final Result", result);
	//imshow("black Result", result_black);
	imshow("right Result", result_right);

	Mat h_left = findHomography(src_left_corners, dst_left_corners);
	Mat output_img_left;
	warpPerspective(left_rotate, output_img_left, h_left, left_rotate.size());
	// create mask
	Mat m1_left = Mat::zeros(left_rotate.size(), CV_8UC1);
	m1_left = Scalar(255);
	Mat mask_output_left;
	warpPerspective(m1_left, mask_output_left, h_left, left_rotate.size());
	// use mask
	Mat result1_left;
	add(output_img_left, output_img_left, result1_left, mask_output_left);
	Mat result2_left;
	bitwise_not(mask_output_left, mask_output_left);
	add(left_rotate, result1_left, result2_left, mask_output_left);
	Mat result_left;
	add(result1_left, result2_left, result_left);
#endif
	imshow("right Result", result);
	waitKey(0);
}





















