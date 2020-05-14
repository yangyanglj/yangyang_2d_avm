//  OpenCV 文件头
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

using namespace cv;
using namespace std;

int
main(int argc, char *argv[])
{
    // 初始化图像矩阵

    Mat black_img = Mat::zeros(Size(480, 480), CV_8UC3);//黑色图像

   
    namedWindow("black image", CV_WINDOW_AUTOSIZE | CV_WINDOW_FREERATIO);// 创建一个窗口
 

    imwrite("black.jpg", black_img);// 在窗口中显示图片

    waitKey(0);// 等待一次按键，程序结束
}
