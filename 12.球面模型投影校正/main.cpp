#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

const float PI = 3.1415926;

void rectifyMap(Mat &mapImg, const int inWidth, const int inHeight,const float* rot, const int outWidth, const int outHeight, const float FOV, const float radius)
{
	float cx = inWidth/2.0;
	float cy = inHeight/2.0;

	float* pMapData = (float*)mapImg.data;
	for (int j = 0; j < outHeight; j++)
	{
		float theta1 = j*PI / outHeight;
		float sinTheta1 = sin(theta1);
		float z1 = cos(theta1);

		for (int i = 0; i < outWidth; i++)
		{
			float fi1 = 2 * PI - i* 2*PI / outWidth;
			float x1 = sinTheta1*cos(fi1);
			float y1 = sinTheta1*sin(fi1);

			//归一化三维坐标
			float x2 = rot[0] * x1 + rot[1] * y1 + rot[2] * z1;
			float y2 = rot[3] * x1 + rot[4] * y1 + rot[5] * z1;
			float z2 = rot[6] * x1 + rot[7] * y1 + rot[8] * z1;
			float norm = sqrt(x2*x2 + y2*y2 + z2*z2);
			x2 /= norm;
			y2 /= norm;
			z2 /= norm;

			//球面坐标系转换
			float theta2 = acos(z2)*180/PI;
			float fi2 = atan2(y2, x2);

			if (theta2 <= (FOV / 2) && theta2 >= 0)
			{
				//球面到鱼眼
				float radius2 =radius* theta2 / (FOV / 2);
				float u = (radius2*cos(fi2) + cx);
				float v = (radius2*sin(fi2) + cy);
				if (u >= 0 && u < inWidth - 1 && v >= 0 && v < inHeight - 1)
				{
					pMapData[j*outWidth * 2 + 2 * i + 0] = u;
					pMapData[j*outWidth * 2 + 2 * i + 1] = v;
				}
				else
				{
					pMapData[j*outWidth * 2 + 2 * i + 0] = 0;
					pMapData[j*outWidth * 2 + 2 * i + 1] = 0;
				}
			}
			else
			{
				pMapData[j*outWidth * 2 + 2 * i + 0] = 0;
				pMapData[j*outWidth * 2 + 2 * i + 1] = 0;
			}
		}
	}
}

void remap(const cv::Mat& srcImg, cv::Mat& dstImg, const cv::Mat& mapImg, int inHeight, int inWidth, int outHeight, int outWidth)
{
	uchar* pSrcData = (uchar*)srcImg.data;
	uchar* pDstData = (uchar*)dstImg.data;
	float* pMapData = (float*)mapImg.data;

	for (int j = 0; j < outHeight; j++)
	{
		for (int i = 0; i < outWidth; i++)
		{
			int idx = j*outWidth * 2 + i * 2;
			float u = pMapData[idx + 0];
			float v = pMapData[idx + 1];

			int u0 = floor(u);
			int v0 = floor(v);
			float dx = u - u0;
			float dy = v - v0;
			float weight1 = (1 - dx)*(1 - dy);
			float weight2 = dx*(1 - dy);
			float weight3 = (1 - dx)*dy;
			float weight4 = dx*dy;

			if (u0 >= 0 && v0 >= 0 && (u0 + 1) < inWidth && (v0 + 1) < inHeight)
			{
				float B = weight1*pSrcData[v0*inWidth * 3 + u0 * 3 + 0] + weight2*pSrcData[v0*inWidth * 3 + (u0 + 1) * 3 + 0] +
					weight3*pSrcData[(v0 + 1)*inWidth * 3 + u0 * 3 + 0] + weight4*pSrcData[(v0 + 1)*inWidth * 3 + (u0 + 1) * 3 + 0];

				float G = weight1*pSrcData[v0*inWidth * 3 + u0 * 3 + 1] + weight2*pSrcData[v0*inWidth * 3 + (u0 + 1) * 3 + 1] +
					weight3*pSrcData[(v0 + 1)*inWidth * 3 + u0 * 3 + 1] + weight4*pSrcData[(v0 + 1)*inWidth * 3 + (u0 + 1) * 3 + 1];

				float R = weight1*pSrcData[v0*inWidth * 3 + u0 * 3 + 2] + weight2*pSrcData[v0*inWidth * 3 + (u0 + 1) * 3 + 2] +
					weight3*pSrcData[(v0 + 1)*inWidth * 3 + u0 * 3 + 2] + weight4*pSrcData[(v0 + 1)*inWidth * 3 + (u0 + 1) * 3 + 2];

				int idxResult = j*outWidth * 3 + i * 3;
				pDstData[idxResult + 0] = uchar(B);
				pDstData[idxResult + 1] = uchar(G);
				pDstData[idxResult + 2] = uchar(R);
			}
		}
	}
}

int main()
{
	string imgPath = "./";
	Mat srcImg = imread(imgPath + "right.jpg");
    //输入鱼眼图像尺寸
	int inHeight = srcImg.rows;
	int inWidth = srcImg.cols;
    //输出经纬度图像尺寸
	int outHeight = inHeight;
	int outWidth = inWidth;
    //视场角
	float FOV = 100;
    //鱼眼半径
	float radius = inWidth / 2.0;

    //以图像中心为赤道
    float rot[9] = { 1,0,0,0,1,0,0,0,1 };
	float angle = PI/2;
	rot[0] = cos(angle);
	rot[2] = sin(angle);
	rot[6] = -sin(angle);
	rot[8] = cos(angle);

    //求映射Map
	cv::Mat mapImg = cv::Mat::zeros(outHeight, outWidth, CV_32FC2);
	rectifyMap(mapImg, inWidth, inHeight,rot, outWidth, outHeight, FOV, radius);
    //remap得到经纬度图像
	Mat dstImg = Mat::zeros(outHeight, outWidth, CV_8UC3);
	remap(srcImg, dstImg, mapImg, inHeight, inWidth, outHeight, outWidth);

	imwrite("dstImg.jpg", dstImg);
}
