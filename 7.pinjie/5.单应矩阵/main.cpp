#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
/*
top
src
vector<Point2f> pts_src;
    pts_src.push_back(Point2f(591, 123));
    pts_src.push_back(Point2f(688, 125));
    pts_src.push_back(Point2f(704, 168));
    pts_src.push_back(Point2f(573, 166));
lixiang
     vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(602, 575));
    pts_dst.push_back(Point2f(665, 575));
    pts_dst.push_back(Point2f(665, 601));
    pts_dst.push_back(Point2f(602, 601));
 

left
  vector<Point2f> pts_src;
    pts_src.push_back(Point2f(596, 164));
    pts_src.push_back(Point2f(689, 162));
    pts_src.push_back(Point2f(705, 209));
    pts_src.push_back(Point2f(584, 211));

 vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(583, 589));
    pts_dst.push_back(Point2f(697, 589));
    pts_dst.push_back(Point2f(697, 626));
    pts_dst.push_back(Point2f(583, 626));

black
    vector<Point2f> pts_src;
    pts_src.push_back(Point2f(544, 91));
    pts_src.push_back(Point2f(627, 89));
    pts_src.push_back(Point2f(645, 137));
    pts_src.push_back(Point2f(531, 135));
 
    vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(772, 289));
    pts_dst.push_back(Point2f(865, 289));
    pts_dst.push_back(Point2f(865, 342));
    pts_dst.push_back(Point2f(772, 342));

right
   vector<Point2f> pts_src;
    pts_src.push_back(Point2f(578, 183));
    pts_src.push_back(Point2f(669, 185));
    pts_src.push_back(Point2f(679, 231));
    pts_src.push_back(Point2f(561, 228));

  pts_dst.push_back(Point2f(634, 669));
    pts_dst.push_back(Point2f(703, 669));
    pts_dst.push_back(Point2f(703, 702));
    pts_dst.push_back(Point2f(634, 702));
*/
using namespace cv;
using namespace std;
 
int main( int argc, char** argv)
{
    // Read source image.
    Mat im_src = imread("black.JPG");
    // Four corners of the book in source image
#if 1
   vector<Point2f> pts_src;
    pts_src.push_back(Point2f(437, 201));
    pts_src.push_back(Point2f(590, 199));
    pts_src.push_back(Point2f(608, 267));
    pts_src.push_back(Point2f(423, 271));
 
    //circle(im_src, Point2f(606, 98), 3, Scalar(0, 0, 255),-1);
   // circle(im_src, Point2f(679, 94), 3, Scalar(0, 0, 255),-1);
   // circle(im_src, Point2f(692, 130), 3, Scalar(0, 0, 255),-1);
   // circle(im_src, Point2f(596, 133), 3, Scalar(0, 0, 255),-1);
#endif
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
    pts_dst.push_back(Point2f(583, 589));
    pts_dst.push_back(Point2f(697, 589));
    pts_dst.push_back(Point2f(697, 626));
    pts_dst.push_back(Point2f(583, 626));
 
    // Calculate Homography
 
    //Mat h = findHomography(pts_src, pts_dst);
    Mat h = getPerspectiveTransform(pts_src,pts_dst);
    cout<<"xx"<<h<<endl;
    // Output image
    Mat im_out = Mat::zeros( im_src.rows, im_src.cols, im_src.type() );
   // Mat im_out = imread();
    // Warp source image to destination based on homography
   // warpPerspective(im_src, im_out, h, im_src.size());
   
    //circle(im_out, Point2f(561, 473), 3, Scalar(0, 0, 255),-1);
    //circle(im_out, Point2f(672, 473), 3, Scalar(0, 0, 255),-1);
    //circle(im_out, Point2f(672, 515), 3, Scalar(0, 0, 255),-1);
    //circle(im_out, Point2f(561, 515), 3, Scalar(0, 0, 255),-1);
    // imshow("xxxxx", im_out);
    warpPerspective(im_src,im_out,h, im_src.size(),cv::INTER_LINEAR);
    
    // Display images
    imshow("Source Image", im_src);
   // imshow("Destination Image", im_dst);
    imshow("Warped Source Image", im_out);
    imwrite("1.jpg",im_out);
    waitKey(0);
}
