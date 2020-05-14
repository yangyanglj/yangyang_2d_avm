#include <opencv2/opencv.hpp>
/*
线性混合运算　带权值
	F(x,y) = alpha*f(x,y) (1-alpha)*g(x,y)
alpha属于0-1,代表两张图混合时候哪张图占的比例更大。f(x,y),g(x,y)代表两张图，也可以是两个区域。混合
运算的两个对象。f(x,y)代表混合后生成的图片。
*/
using namespace std;
using namespace cv;

int main()
{
	//读入四幅图片
	Mat image_1 = imread("result.jpg");
	float width = 1280;
         float height = 720;
    	 float Kx,Ky;
  	 float view_h,view_w,car_w,car_h;
  	 float dis_w,dis_h,block;
 	 view_h = 165;view_w = 150;
  	 car_w = 28;car_h = 48;
  	 dis_w = 4.5;dis_h=9.5;block=11.5;
    
  	 Kx = width/(2*view_w+car_w);
   	 Ky = height/(2*view_h+car_h);
cout<<"image_1.depth is ::"<< image_1.depth()<<endl;
	cvtColor(image_1,image_1,CV_BGR2GRAY);
	//imshow("xx",image_4);
	waitKey(0);
	//创建连接后存入的图像
	//Mat result(image_1.size(),CV_8UC3,Scalar(255,255,255));
	Mat result(720,1280,CV_8UC1);
	for(int i = 0;i <result.rows;i++ )
	{
	   //&& (j < (float)((i-p4)/k4) ) 
	   for(int j = 0;j <result.cols;j++ )
	   {
		
		result.at<uchar>(i,j)=0;
	   }

         }
	
//top
	Point a1,a3,a8,a6;
	//a1.x=468;a1.y=327;
	//a3.x=509;a3.y=349;
	a1.x=Kx*(view_w - dis_w - block);
	a1.y=Ky*(view_h - dis_h - block);
	a3.x=Kx*(view_w - dis_w);
	a3.y=Ky*(view_h - dis_h);
	a8.x=Kx*(view_w +car_w + dis_w );
	a8.y=Ky*(view_h - dis_h);
	a6.x=Kx*(view_w +car_w + dis_w +block);
	a6.y=Ky*(view_h - dis_h - block);

//circle(image_1,a1, 3, Scalar(255, 0, 0),-1);
//circle(image_1,a3, 3, Scalar(255, 0, 255),-1);
//circle(image_1,a6, 3, Scalar(255, 0, 255),-1);
//circle(image_1,a8, 3, Scalar(255, 0, 255),-1);




//left
	Point b2,b4;
	b2.x=Kx*(view_w - dis_w);
	b2.y=Ky*(view_h + car_h + dis_h);
	b4.x=Kx*(view_w - dis_w - block);
	b4.y=Ky*(view_h + car_h + dis_h + block);


 //black
	Point b5,b7;
	b5.x = Kx*(view_w + car_w + dis_w);
	b5.y = Ky*(view_h + car_h + dis_h);
	b7.x = Kx*(view_w + car_w + dis_w + block);
	b7.y = Ky*(view_h + car_h + dis_h + block);


//right
	Point r6,r8,r5,r7;
	r6.x = Kx*(view_w +car_w + dis_w +block);
	r6.y=Ky*(view_h - dis_h - block);

	r8.x = Kx*(view_w +car_w + dis_w );
	r8.y=Ky*(view_h - dis_h);
	r5.x=Kx*(view_w +car_w + dis_w );
	r5.y=Ky*(view_h + car_h + dis_h);
	r7.x=Kx*(view_w +car_w + dis_w +block);
	r7.y=Ky*(view_h + car_h + dis_h + block);
	

	float k3 = (float) ( a3.y - a1.y) / (a3.x - a1.x) ;//图像坐标系反的，向下为正
	float k4 = -(float) (a8.y - a6.y ) / (a6.x - a8.x) ;

	float k4_2 = (float)(b4.y - b2.y) / (b4.x - b2.x);//不要自己加正负
 	float k5_7 = (float)(b7.y - b5.y) / (b7.x - b5.x);

	float kr6_8 = -(float) (r8.y - r6.y ) / (r6.x - r8.x) ;
	float kr5_7 = (float)(r7.y - r5.y) / (r7.x - r5.x);

	float p3 = (a1.y - k3*a1.x);//截距 这里之前写错了导致负号
        float p4 = (a6.y - k4*a6.x);
	float p4_2 = (b4.y - k4_2*b4.x);
	float p5_7 = (b7.y - k5_7*b7.x);
	float pr6_8 = (r8.y - kr6_8*r8.x);
	float pr5_7 = (r7.y - kr5_7*r7.x);

	
	cout<<"p4:"<<k5_7<<" k4_2::"<<p5_7<<endl;

	Point t1,t2;//上半区域截止
	t1.x=527;t1.y=366;
	t2.x=627;t2.y=366;

	int dis_L = 5;
	float t_top = Ky*view_h;
	
	float t_black = Ky*(view_h + car_h - 5.5);//截止区

	cout<<"t_top: "<<t_top<<endl;

	double top_alpha = 0.5;
	double right_alpha = 0.7;
	double black_alpha = 0.5;
	double left_alpha = 0.7;
	for(int i = 1;i <result.rows;i++ )
	{
	   //&& (j < (float)((i-p4)/k4) ) 
	   for(int j = 1;j <result.cols;j++ )
	   {

//top
		if(j > (float)((i-p3-dis_L)/k3 ) && j < (float)((i-p3+dis_L)/k3 ) && i < t_top)
		{
		//image_1.at<uchar>(i,j)= 255;
		image_1.at<uchar>(i,j)=(1-top_alpha)*image_1.at<uchar>(i+1,j+1) + top_alpha*image_1.at<uchar>(i-1,j-1);
		}

//right
		if(j > (float)((i-p4+5+dis_L)/k4 ) && j < (float)((i-p4-dis_L)/k4 ) && i < t_top )
		{
		//image_1.at<uchar>(i,j)= 255;
		image_1.at<uchar>(i,j)=(1-right_alpha)*image_1.at<uchar>(i+1,j+1) + right_alpha*image_1.at<uchar>(i-1,j-1);
		}
#if 1
//black
		if( j > (float)((i-p4_2+dis_L)/k4_2 ) && j < (float)((i-p4_2-dis_L)/k4_2 ) && i > t_black)
		{
		//image_1.at<uchar>(i,j)= 255;
		image_1.at<uchar>(i,j)=(1-black_alpha)*image_1.at<uchar>(i+1,j+1) + black_alpha*image_1.at<uchar>(i-1,j-1);
		}

//left
		if(j > (float)((i-p5_7 - dis_L )/k5_7 ) && j < (float)((i-p5_7 + dis_L )/k5_7 ) && i > t_black)
		{
		//image_1.at<uchar>(i,j)= 255;
		image_1.at<uchar>(i,j)=(1-left_alpha)*image_1.at<uchar>(i+1,j+1) + left_alpha*image_1.at<uchar>(i-1,j-1);
		}
#endif
		//else 
		//result.at<uchar>(i,j)=image_1.at<uchar>(i,j);
		
	   }
	}
	

	//显示四幅图像连接后的图像
	//Mat dst;
	//blur(result, dst, Size(3,3));
	//medianBlur(result,dst,3);
	//GaussianBlur(result,dst,Size(3,3),0);
	//  bilateralFilter(result,dst,9,50,50);
	imshow("result", image_1);
	//imshow("dst", dst);
	imwrite("ronghe.bmp",image_1);
	
	

	waitKey(0);

	return 0;
}

