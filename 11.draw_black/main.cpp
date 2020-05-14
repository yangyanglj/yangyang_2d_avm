#include<opencv2/opencv.hpp>
using namespace cv; 
int main(){
   Mat img = Mat::zeros(720, 720, CV_8UC1);
   imwrite("img.jpg",img);
   waitKey(0);
 }
