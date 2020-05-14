#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <string.h>

/*******************俯视投影变换********************/

void main( )
{
	float size_ratio = 1;
	char imagename[1024];
	char input_image[100] = "E:\\undistort\\test.txt";
	char output_image[100] = "E:\\undistort\\output\\1.jpg";
	int m = strlen( output_image );
	FILE* f = 0;
	double _camera[9], _dist_coeffs[4];
	CvMat camera_matrix = cvMat( 3, 3, CV_64F, _camera );
	CvMat dist_coeffs = cvMat( 1, 4, CV_64F, _dist_coeffs );

	if( input_image )
	{
		f = fopen( input_image, "rt" );
		if( !f )
			printf( "The input file could not be opened.\n" );
	}
	
	for( int i = 1;; i++ )
    {
        IplImage* view = 0;
		IplImage* view_undist;
		if( f && fgets( imagename, sizeof(imagename)-2, f ))
        {
            int l = strlen(imagename);
            if( l > 0 && imagename[l-1] == '\n' )
                imagename[--l] = '\0';
            view = cvLoadImage( imagename, 1 );
			view_undist = cvCreateImage(cvSize(view->width, view->height), IPL_DEPTH_8U, 3);
			for( int v1 = 0; v1 < view->height; v1++ )
			{
				for( int u1 = 0; u1 < view->width; u1++ )
				{
					
					//远叶参数
					double fx = 316.4304626368295300, u0 = 359.7842894509942700;
					double fy = 286.6144154341764100, v0 = 241.1773302487586600;
					double k1 = -0.2886408794746905, k2 = 0.0797363257543829;
					double p1 = 4.3286484966358654e-004, p2 = 1.7829547874750706e-003;
					/*
					//松下参数
					double fx = 238.5075200728045700, u0 = 360;//354.7139318014819800;
					double fy = 215.8244926518681600, v0 = 240;//216.8775148989670100;
					double k1 = -0.2062630671240197, k2 = 0.0230184949575857;
					double p1 = -1.8516749021758924e-003, p2 = -7.0089354821733943e-004;
					*/
					//待确定的参数：
					float kx = 0.300, ky = 0.300, x0 = -1250, y0 = -800;//-1250, -1600
					float theta = -11*3.14159/180, h = 700;//700
					
					double xw = u1/kx + x0, yw = v1/ky + y0;
					double x = xw /(yw*sin(theta)+h*cos(theta));
					double y = (yw*cos(theta)-h*sin(theta))/(yw*sin(theta)+h*cos(theta));
					double x2 = x*x, y2 = y*y;
					double r2 = x2 + y2, _2xy = 2*x*y;
					double kr = 1 + (k2*r2 + k1)*r2;
					double _u = fx*(x*kr + p1*_2xy + p2*(r2 + 2*x2)) + u0;
					double _v = fy*(y*kr + p1*(r2 + 2*y2) + p2*_2xy) + v0; 
					int u = floor(_u), v = floor(_v);
					CvScalar s, s1, s2, s3, s4;

					if( u < 0 )
						u = 0;
					if( v < 0 )
						v = 0;
					if( u > (view->width - 2) )
						u = view->width - 2;
					if( v > (view->height - 2) )
						v = view->height - 2;

					if( u == _u && v == _v )			
					{
						s = cvGet2D( view, v, u );
					}
					else if( u < _u && v == _v )
					{
						s1 = cvGet2D( view, v, u );
						s2 = cvGet2D( view, v+1, u );
						s.val[0] = (s1.val[0] + s2.val[0])/2;
						s.val[1] = (s1.val[1] + s2.val[1])/2;
						s.val[2] = (s1.val[2] + s2.val[2])/2;
					}
					else if( u ==_u && v < _v )
					{
						s1 = cvGet2D( view, v, u );
						s2 = cvGet2D( view, v, u+1 );
						s.val[0] = (s1.val[0] + s2.val[0])/2;
						s.val[1] = (s1.val[1] + s2.val[1])/2;
						s.val[2] = (s1.val[2] + s2.val[2])/2;
					}
					else
					{
						s1 = cvGet2D( view, v, u );
						s2 = cvGet2D( view, v+1, u );
						s3 = cvGet2D( view, v, u+1 );
						s4 = cvGet2D( view, v+1, u+1 );
						s.val[0] = (s1.val[0] + s2.val[0] + s3.val[0] + s4.val[0])/4;
						s.val[1] = (s1.val[1] + s2.val[1] + s3.val[1] + s4.val[1])/4;
						s.val[2] = (s1.val[2] + s2.val[2] + s3.val[2] + s4.val[2])/4;
					}
					cvSet2D( view_undist, v1, u1, s );
				} 
			}
			cvShowImage( "undistort", view_undist );
			cvWaitKey(500);
			cvSaveImage( output_image, view_undist );
			output_image[m-5]++;
        }
		if( !view )
			break;
		cvReleaseImage( &view );
	}
}