#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{

         float width = 1280;
         float height = 720;
    	 float Kx,Ky;
  	 float view_h,view_w,car_w,car_h;
  	 float dis_w,dis_h,block;
 	 view_h = 165;view_w = 150;
  	 car_w = 28;car_h = 48;
  	 dis_w = 4.5;dis_h=9.5;block=11.5;
//3d
	 float    top_offset = 40;//车前一定范围


  	 Kx = width/(2*view_w+car_w);
   	 Ky = height/(2*view_h+car_h);
	//读入四幅图片
	Mat image_1 = imread("left_2.jpg");
	Mat image_2 = imread("right_2.jpg");
	Mat image_3 = imread("black_2.jpg");
	Mat image_4 = imread("top_2.jpg");
	//归一化为相同的大小：320*240
	Size sz = Size(320, 240);
	cvtColor(image_4,image_4,CV_BGR2GRAY);
	cvtColor(image_3,image_3,CV_BGR2GRAY);
	cvtColor(image_2,image_2,CV_BGR2GRAY);
	cvtColor(image_1,image_1,CV_BGR2GRAY);
	//imshow("xx",image_4);
	//waitKey(0);
	//创建连接后存入的图像
	//Mat result(image_1.size(),CV_8UC3,Scalar(255,255,255));
	Mat result(720,1280,CV_8UC1);
	Mat top_roi(720,1280,CV_8UC1);
	Mat black_roi(720,1280,CV_8UC1);
	Mat left_roi(720,1280,CV_8UC1);
	Mat right_roi(720,1280,CV_8UC1);
	//black_roi,left_roi,right_roi;
	
	for(int i = 0;i <result.rows;i++ )
	{
	   //&& (j < (float)((i-p4)/k4) ) 
	   for(int j = 0;j <result.cols;j++ )
	   {
		
		result.at<uchar>(i,j)=0;
	   }

         }
/*
	k1	k2




	k3	k4
中间小车区域
	

*/
#if 1
	Point2f car_k1,car_k2,car_k3,car_k4;
	car_k1.x = Kx*(view_w);
	car_k1.y = Ky*view_h;

	car_k2.x = Kx*(view_w + car_w);
	car_k2.y = Ky*view_h;

	car_k3.x = car_k1.x;
	car_k3.y = Ky*(view_h + car_h);

	car_k4.x = car_k2.x;
	car_k4.y = car_k3.y;

#endif
	
		
	

/*********************************************************/
//top
/*
a1		     a6
	a3    a8

*/
	Point2f top_a1,top_a3,top_a8,top_a6;
	top_a1.x = Kx*(view_w - dis_w - block);
	top_a1.y = Ky*(view_h - dis_h - block);

	top_a3.x = Kx*(view_w - dis_w);
	top_a3.y = Ky*(view_h - dis_h);

	top_a6.x = Kx*(view_w +car_w + dis_w +block);
	top_a6.y = Ky*(view_h - dis_h - block);
	
	top_a8.x = Kx*(view_w +car_w + dis_w );
	top_a8.y = Ky*(view_h - dis_h);
	
	float k_top_1_3 = (float)( top_a3.y - top_a1.y) / (top_a3.x - top_a1.x);
	float k_top_6_8 = -(float) (top_a8.y - top_a6.y ) / (top_a6.x - top_a8.x) ;

	float p_top_1_3 = (float)(top_a1.y - k_top_1_3*top_a1.x);//截距
	float p_top_6_8 = (top_a8.y - k_top_6_8*top_a8.x);

	float t_top = Kx*view_h;
	float t_top_stop = Kx*(view_h - top_offset);
#if 1//left
/*
a1	
	a3

	b2
b4
*/
	

	Point2f left_a1,left_a3,left_b2,left_b4;
	left_a1.x = Kx*(view_w - dis_w - block);
	left_a1.y = Ky*(view_h - dis_h - block);

	left_a3.x = Kx*(view_w - dis_w);
	left_a3.y = Ky*(view_h - dis_h);
		
	left_b2.x = Kx*(view_w - dis_w);
	left_b2.y = Ky*(view_h + car_h + dis_h);

	left_b4.x = Kx*(view_w - dis_w - block);
	left_b4.y = Ky*(view_h + car_h + dis_h + block);

	float k_left_1_3 = (float)( left_a3.y - left_a1.y) / (left_a3.x - left_a1.x);
	float k_left_4_2 = (float)(float)(left_b4.y - left_b2.y) / (left_b4.x - left_b2.x);

	float p_left_1_3 = (float)(left_a1.y - k_left_1_3*left_a1.x);//截距
	float p_left_4_2 = (float)(left_b4.y - k_left_4_2*left_b4.x);

	float t_left = Kx*(view_h + 3);
#endif
   
#if 1 //black
/*
	b2		b5

b4				b7

*/

	Point black_b2,black_b4,black_b5,black_b7;

	black_b2.x = Kx*(view_w - dis_w);
	black_b2.y = Ky*(view_h + car_h + dis_h);

	black_b4.x = Kx*(view_w - dis_w - block);
	black_b4.y = Ky*(view_h + car_h + dis_h + block);

	black_b5.x = Kx*(view_w + car_w + dis_w);
	black_b5.y = Ky*(view_h + car_h + dis_h);

	black_b7.x = Kx*(view_w + car_w + dis_w + block);
	black_b7.y = Ky*(view_h + car_h + dis_h + block);

	float k_black_4_2 = (float)(black_b4.y - black_b2.y) / (black_b4.x - black_b2.x);
	float k_black_5_7 = (float)(black_b7.y - black_b5.y) / (black_b7.x - black_b5.x);

	float p_black_4_2 = (float)(black_b4.y - k_black_4_2*black_b4.x);
	float p_black_5_7 = (float)(black_b7.y - k_black_5_7*black_b7.x);

	float t_black = Ky*(view_h + car_h );//截止区
#endif

//right
/*
	a6
	
a8


b5

	b7

*/
	Point2f right_a6,right_a8,right_b5,right_b7;
	right_a6.x = Kx*(view_w +car_w + dis_w +block);
	right_a6.y= Ky*(view_h - dis_h - block);
	

	right_a8.x = Kx*(view_w +car_w + dis_w );
	right_a8.y=  Ky*(view_h - dis_h);
	

	right_b5.x= Kx*(view_w +car_w + dis_w );
	right_b5.y= Ky*(view_h + car_h + dis_h);
	

	right_b7.x= Kx*(view_w +car_w + dis_w +block);
	right_b7.y= Ky*(view_h + car_h + dis_h + block);
	
	
	float k_right_6_8 = -(float) (right_a8.y - right_a6.y ) / (right_a6.x - right_a8.x) ;
	float k_right_5_7 = (float)(right_b7.y - right_b5.y) / (right_b7.x - right_b5.x);

	float p_right_6_8 = (right_a8.y - k_right_6_8*right_a8.x);
	float p_right_5_7 = (right_b7.y - k_right_5_7*right_b7.x);

	float t_right = Kx*(view_w + car_w);

/******************3维容器*****************/
	//top 
	vector<Point3i> top_3d_sum;
	Point3i top_3d;
	int num_top_3d;
/******************3维容器*****************/
	cout << " t_top is "<<t_top<<"  t_top_stop "<<t_top_stop<<endl;
	for(int i = 0;i <result.rows;i++ )
	{
	   
	   for(int j = 0;j <result.cols;j++ )
	   {
//top
	
		if(i < t_top && (j > (float)((i-p_top_1_3)/k_top_1_3 ))   && (j < (float)((i-p_top_6_8)/k_top_6_8) ))
		{
		result.at<uchar>(i,j)=image_4.at<uchar>(i,j);
		top_roi.at<uchar>(i,j)=image_4.at<uchar>(i,j);
#if 0
		top_3d.x = j;
		top_3d.y = i;
		if(i > 200)
			top_3d.z = 1;
		else
			top_3d.z = 0;
		top_3d_sum.push_back(top_3d);
#endif	
		}


//black
		
		if( j > (float)((i-p_black_4_2)/k_black_4_2 ) && i > t_black && j < (float)((i-p_black_5_7)/k_black_5_7 ) )
		{
		
		result.at<uchar>(i,j)=image_3.at<uchar>(i,j);
		black_roi.at<uchar>(i,j)=image_3.at<uchar>(i,j);
		}

//负斜率　大于小于　　上下方颠倒 left
		if(j < (float)((i-p_left_4_2)/k_left_4_2 ) && j < (float)((i-p_left_1_3)/k_left_1_3 ) && j <t_left )
		{
		
		result.at<uchar>(i,j)=image_1.at<uchar>(i,j);
		left_roi.at<uchar>(i,j)=image_1.at<uchar>(i,j);
		}


//right
		if( j > t_right && (j > (float)((i-p_right_6_8)/k_right_6_8) ) && j > (float)((i-p_black_5_7)/k_black_5_7 ) )
		{
		   result.at<uchar>(i,j)=image_2.at<uchar>(i,j);
		   right_roi.at<uchar>(i,j)=image_2.at<uchar>(i,j);
		}


		if(i > car_k1.y - 7 && i < car_k3.y + 7 && j > car_k1.x -7 && j < car_k2.x + 7)
			result.at<uchar>(i,j)=0;
	   }

         }

	 //打印top 3d容器
	//for(size_t i = 0;i < top_3d_sum.size(); i++)
	//{
	//  cout << "top 3d is" << top_3d_sum[i].x << endl;  
	//}	

#if 0
	Mat mask;
	Rect r1(0,0,result.cols,result.rows/3);
	mask = Mat::zeros(result.size(),CV_8UC1);
	mask(r1).setTo(255);
#endif
	//显示四幅图像连接后的图像
	imwrite("top_roi.jpg",top_roi);
	imwrite("black_roi.jpg",black_roi);
	imwrite("left_roi.jpg",left_roi);
	imwrite("right_roi.jpg",right_roi);
	imshow("result", result);
	imwrite("result.jpg",result);
	waitKey(0);

	return 0;
}

