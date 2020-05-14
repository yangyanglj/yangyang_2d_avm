#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
/*
top ok
src 

   vector<Point2f> pts_src;
    pts_src.push_back(Point2f(425, 84));
    pts_src.push_back(Point2f(854, 89));
    pts_src.push_back(Point2f(977, 145));
    pts_src.push_back(Point2f(293, 136));


 

left //ok
  vector<Point2f> pts_src;
    pts_src.push_back(Point2f(1081, 151));
    pts_src.push_back(Point2f(940, 104));
    pts_src.push_back(Point2f(311, 131));
    pts_src.push_back(Point2f(147, 204));

   vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(320, 513));
    pts_dst.push_back(Point2f(285, 513));
    pts_dst.push_back(Point2f(285, 764));
    pts_dst.push_back(Point2f(320, 764));
 
black ok
     vector<Point2f> pts_src;
    pts_src.push_back(Point2f(446, 129));
    pts_src.push_back(Point2f(820, 134));
    pts_src.push_back(Point2f(875, 182));
    pts_src.push_back(Point2f(380, 180));
 
  vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(752, 430));
    pts_dst.push_back(Point2f(568, 430));
    pts_dst.push_back(Point2f(568, 412));
    pts_dst.push_back(Point2f(752, 412));
right ok
     vector<Point2f> pts_src;
    pts_src.push_back(Point2f(114, 172));
    pts_src.push_back(Point2f(308, 110));
    pts_src.push_back(Point2f(917, 123));
    pts_src.push_back(Point2f(1034, 174));

  vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(399, 513));
    pts_dst.push_back(Point2f(423, 513));
    pts_dst.push_back(Point2f(423, 764));
    pts_dst.push_back(Point2f(399, 764));
 
*/
using namespace cv;
using namespace std;
 
char *filename = "black.jpg";

int main( int argc, char** argv)
{
    // Read source image.
    Mat im_src = imread(filename);


    char *str_black = "black.jpg";
    char *str_top = "top.jpg";
    char *str_left = "left.jpg";
    char *str_right = "right.jpg";

    // Four corners of the book in source image
    float width = 1280;
    float height = 720;
    float Kx,Ky;
    float view_h,view_w,car_w,car_h;
    float dis_w,dis_h,block;
    view_h = 170;view_w = 160;
    car_w = 28;car_h = 48;
    dis_w = 4.5;dis_h=9.5;block=11.5;
    
    Kx = width/(2*view_w+car_w);
    Ky = height/(2*view_h+car_h);
    
    /*
	top
     1(1)		6(2)

     4(4)			7(3)
   */
    Point2f top_a1,top_a6,top_a7,top_a4;
    top_a1.x = Kx*(view_w - dis_w - block);
    top_a1.y = Ky*(view_h - dis_h - block);

    top_a6.x = Kx*(view_w + car_w + dis_w + block);
    top_a6.y = top_a1.y;

    top_a7.x = top_a6.x;
    top_a7.y = Ky*(view_h - dis_h);

    top_a4.x = top_a1.x;
    top_a4.y = top_a7.y;
   // cout<<"  "<<top_a1<<endl;

    /*
	black
	b7(1)		b4(2)
	
	b6(4)		b1(3)
   
    */
    Point2f black_b7,black_b4,black_b1,black_b6;
    black_b7.x = Kx*(view_w + car_w + dis_w + block);
    black_b7.y = Ky*(view_h + car_h + dis_h + block);

    black_b4.x = Kx*(view_w - dis_w - block);
    black_b4.y = black_b7.y;

    black_b1.x = black_b4.x;
    black_b1.y = Ky*(view_h + car_h + dis_h);

    black_b6.x = black_b7.x;
    black_b6.y = black_b1.y;
   
    /*
	left
	2 a1		1 a2

	3 b4		4 b3
    */
    Point2f left_a2,left_a1,left_b4,left_b3;
    left_a2.x = Kx*(view_w - dis_w);
    left_a2.y = Ky*(view_h - dis_h - block);

    left_a1.x = Kx*(view_w - dis_w - block);
    left_a1.y = left_a2.y;

    left_b4.x = left_a1.x;
    left_b4.y = Ky*(view_h + car_h + dis_h + block);

    left_b3.x = left_a2.x;
    left_b3.y = left_b4.y;


    /*
	right
	1 a5	2 a6

	4 b8	3 b7
	
    */
     Point2f right_a5,right_a6,right_b7,right_b8;
     right_a5.x = Kx*(view_w + car_w + dis_w);
     right_a5.y = Ky*(view_h - dis_h - block);

     right_a6.x = Kx*(view_w + car_w + dis_w + block);
     right_a6.y = right_a5.y;

     right_b7.x = right_a6.x;
     right_b7.y = Ky*(view_h + car_h + dis_h + block);

     right_b8.x = right_a5.x;
     right_b8.y = right_b7.y;

     vector<Point2f> pts_src;

     if(strcmp(filename,str_top)==0)
     {
       
        pts_src.push_back(Point2f(501, 134));
        pts_src.push_back(Point2f(783, 140));
        pts_src.push_back(Point2f(822, 194));
        pts_src.push_back(Point2f(455, 183));
     }

     if(strcmp(filename,str_black)==0)
     {
       
        pts_src.push_back(Point2f(530, 142));
        pts_src.push_back(Point2f(755, 151));
        pts_src.push_back(Point2f(773, 185));
        pts_src.push_back(Point2f(507, 177));
     }

      if(strcmp(filename,str_left)==0)
     {
       
        pts_src.push_back(Point2f(891, 188));
        pts_src.push_back(Point2f(843, 143));
        pts_src.push_back(Point2f(460, 141));
        pts_src.push_back(Point2f(413, 186));
     }

      if(strcmp(filename,str_right)==0)
      {
       
        pts_src.push_back(Point2f(408, 176));
        pts_src.push_back(Point2f(452, 135));
        pts_src.push_back(Point2f(826, 147));
        pts_src.push_back(Point2f(867, 191));
     }
     
   // circle(im_src, Point2f(425, 84), 3, Scalar(0, 0, 255),-1);
   // circle(im_src, Point2f(854, 89), 3, Scalar(0, 0, 255),-1);
  //  circle(im_src, Point2f(977, 145), 3, Scalar(0, 0, 255),-1);
  //  circle(im_src, Point2f(293, 136), 3, Scalar(0, 0, 255),-1);

#if 0
    vector<Point2f> pts_src;
    pts_src.push_back(Point2f(605, 323));
    pts_src.push_back(Point2f(716, 320));
    pts_src.push_back(Point2f(739, 367));
    pts_src.push_back(Point2f(586, 370));
 
    circle(im_src, Point2f(605, 323), 3, Scalar(0, 0, 255),-1);
    circle(im_src, Point2f(716, 320), 3, Scalar(0, 0, 255),-1);
    circle(im_src, Point2f(739, 367), 3, Scalar(0, 0, 255),-1);
    circle(im_src, Point2f(586, 370), 3, Scalar(0, 0, 255),-1);
#endif
    // Read destination image.
    //Mat im_dst = imread("book1.jpg");
    // Four corners of the book in destination image.
 
   vector<Point2f> pts_dst;
    if(strcmp(filename,str_top)==0)
    {
       
        pts_dst.push_back(top_a1);
        pts_dst.push_back(top_a6);
        pts_dst.push_back(top_a7);
        pts_dst.push_back(top_a4);
    }
    if(strcmp(filename,str_black)==0)
    {
       
        pts_dst.push_back(black_b7);
        pts_dst.push_back(black_b4);
        pts_dst.push_back(black_b1);
        pts_dst.push_back(black_b6);
    }
 
    if(strcmp(filename,str_left)==0)
    {
       
        pts_dst.push_back(left_a2);
        pts_dst.push_back(left_a1);
        pts_dst.push_back(left_b4);
        pts_dst.push_back(left_b3);
    }

     if(strcmp(filename,str_right)==0)
    {
       
        pts_dst.push_back(right_a5);
        pts_dst.push_back(right_a6);
        pts_dst.push_back(right_b7);
        pts_dst.push_back(right_b8);
    }
    // Calculate Homography
 
    //Mat h = findHomography(pts_src, pts_dst);
    Mat h = getPerspectiveTransform(pts_src,pts_dst);
    cout<<"xx"<<h<<endl;
    // Output image
    //Mat im_out = Mat::zeros( im_src.rows, im_src.cols, im_src.type() );
    Mat im_out = Mat::zeros( 720, 1280, im_src.type() );
   // Mat im_out = imread("black_bird.jpg");
    // Warp source image to destination based on homography
   // warpPerspective(im_src, im_out, h, im_src.size());
  // circle(im_src, top_a1, 3, Scalar(0, 255, 255),-1);
   // circle(im_src, top_a6, 3, Scalar(0, 255, 255),-1);
  //  circle(im_src, top_a7, 3, Scalar(0, 255, 255),-1);
   //  circle(im_src, top_a4, 3, Scalar(0, 255, 255),-1);
   
    // imshow("xxxxx", im_out);
    warpPerspective(im_src,im_out,h, im_out.size(),cv::INTER_LINEAR);
    
    // Display images
    imshow("Source Image", im_src);
   // imshow("Destination Image", im_dst);
    imshow("Warped Source Image", im_out);
    imwrite("1.jpg",im_out);
    waitKey(0);
}
