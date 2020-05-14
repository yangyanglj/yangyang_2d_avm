#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;//声明使用的命名空间

struct edge_list
{
	CvPoint *EdgePiont;	
	int 	 ListSize;		

};

void train_shape_model(Mat Image, int Contrast, int MinContrast, int PointReduction, edge_list *EdgeList)
{
    /* source image size */
    int width  = Image.cols;
    int height = Image.rows;
    /* Compute buffer sizes */
    uint32_t  bufferSize  = width * height;
    /* Allocate buffers for each vector */
    uint8_t  *pInput  = (uint8_t *) malloc(bufferSize * sizeof(uint8_t));
    uint8_t  *pBufOut = (uint8_t *) malloc(bufferSize * sizeof(uint8_t));
    int16_t  *pBufGradX  = (int16_t *) malloc(bufferSize * sizeof(int16_t));
    int16_t  *pBufGradY  = (int16_t *) malloc(bufferSize * sizeof(int16_t));
    int32_t *pBufOrien =  (int32_t *) malloc(bufferSize * sizeof(int32_t));
    float *pBufMag = (float *) malloc(bufferSize * sizeof(float));
    CvPoint  *pEdgePiont  = (CvPoint *) malloc(bufferSize * sizeof(CvPoint));
    if( pInput && pBufGradX && pBufGradY && pBufMag && pBufOrien && pBufOut)
    {
      //GaussianBlur(Image,Image,Size(3,3),0,0);
	//gaussian_filter((uint8_t*)Image.data, pInput, width, height);
	memset(pBufGradX,  0, bufferSize * sizeof(int16_t));
	memset(pBufGradY,  0, bufferSize * sizeof(int16_t));
	memset(pBufOrien,  0, bufferSize * sizeof(int32_t));
	memset(pBufOut,    0, bufferSize * sizeof(uint8_t));
	memset(pBufMag,   0, bufferSize * sizeof(float));
	float MaxGradient=-9999.99f;
	int count = 0,i,j; // count variable;
/***********************step 1**************************************/
	for( i = 1; i < width-1; i++ )
	{
	    for( j = 1; j < height-1; j++ )
	    {		
	int16_t sdx =*(pInput + j*width + i + 1) - *(pInput + j*width + i - 1);
	int16_t sdy =*(pInput + (j+1)*width + i ) - *(pInput + (j-1)*width + i);
	*(pBufGradX + j*width + i) = sdx;
	*(pBufGradY + j*width + i) = sdy;
	float MagG = sqrt((float)(sdx*sdx) + (float)(sdy*sdy));
	*(pBufMag + j*width + i) = MagG;

	if(MagG>MaxGradient)
	MaxGradient=MagG;
	    }
	}
/************************step 2**********************************/
	for( i = 1; i < width - 1; i++ )
	{
	    for( j = 1; j < height - 1; j++ )
	    {
		int16_t fdx = *(pBufGradX + j*width + i);
		int16_t fdy = *(pBufGradY + j*width + i);
		float direction =cvFastArctan((float)fdy,(float)fdx);
		// get closest angle from 0, 45, 90, 135 set
		if ( (direction>0 && direction < 22.5) || (direction >157.5 && direction < 202.5) || (direction>337.5 && direction<360)  )
		direction = 0;
		else if ( (direction>22.5 && direction < 67.5) || (direction >202.5 && direction <247.5)  )
		direction = 45;
		else if ( (direction >67.5 && direction < 112.5) || (direction>247.5 && direction<292.5) )
		direction = 90;
		else if ( (direction >112.5 && direction < 157.5) || (direction>292.5 && direction<337.5) )
		direction = 135;
		else
		direction = 0;
		pBufOrien[count]  = (int32_t)direction;
		count++;
	    }
	}
/*********************step 3***********************/
    float leftPixel,rightPixel;
    count=0; // init count
    for( i = 1; i < width-1; i++ )
    {
	for( j = 1; j < height-1; j++ )
	{
	    switch ( pBufOrien[count] )
	    {
		case 0:
		leftPixel   =  *(pBufMag + j*width + i - 1);
		rightPixel = *(pBufMag + j*width + i + 1);
		break;
		case 45:
		leftPixel   =  *(pBufMag + (j-1)*width + i - 1);
		rightPixel = *(pBufMag + (j+1)*width + i + 1);
		break;
		case 90:
		leftPixel   =  *(pBufMag + (j-1)*width + i);
		rightPixel = *(pBufMag + (j+1)*width + i);
		break;
		case 135:
		leftPixel   =  *(pBufMag + (j+1)*width + i - 1);
		rightPixel =  *(pBufMag + (j-1)*width + i + 1);
		break;
	    }
	// compare current pixels value with adjacent pixels
	if (( *(pBufMag + j*width + i) < leftPixel ) || (*(pBufMag + j*width + i) < rightPixel ))
		*(pBufOut + j*width + i) = 0;
	else
		*(pBufOut + j*width + i) = (uint8_t)(*(pBufMag + j*width + i) / MaxGradient * 255);

	count++;
	}
    }
/***********************step 4************************************/
//Hysteresis threshold
    int RSum=0,CSum=0;
    int curX,curY;
    int flag=1;
    int n = 0;
    int iPr = PointReduction;
    for( i = 1; i < width-1; i+=iPr )
    {
	for( j = 1; j < height-1; j+=iPr )
	{
	    int16_t fdx = *(pBufGradX + j*width + i);
	    int16_t fdy = *(pBufGradY + j*width + i);
	    float MagG =*(pBufMag   + j*width + i);
	    flag=1;
	    if((float)*(pBufOut + j*width + i) < Contrast)
	    {
		if((float)*(pBufOut + j*width + i) < MinContrast)
		{
			*(pBufOut + j*width + i)=0;
			flag=0; // remove from edge
		}
		else
		{
	if( ((float)*(pBufOut + (j-1)   * width + i - 1) < Contrast) &&
	((float)*(pBufOut +     j     * width + i - 1) < Contrast)  &&
	((float)*(pBufOut + (j-1)   * width + i - 1) < Contrast) &&
	((float)*(pBufOut + (j-1)   * width + i     ) < Contrast) &&
	((float)*(pBufOut + (j+1)  * width + i     ) < Contrast) &&
	((float)*(pBufOut + (j-1)   * width + i +1) < Contrast) &&
	((float)*(pBufOut + j        * width + i + 1) < Contrast) &&
	((float)*(pBufOut + (j+1)  * width + i + 1) < Contrast))
	{
		*(pBufOut + j*width + i)=0;
		flag=0;
	}
		}
	    }
/**********************step 5********************************/
// save selected edge information
    curX=i;curY=j;
    if(flag!=0)
    {
	if(fdx!=0 || fdy!=0)
	{
	    pEdgePiont[n].x = curX;
	    pEdgePiont[n].y = curY;
	    n++;
	}
    }

	}
    }
/***********************step 6****************************************/
    EdgeList->ListSize = n;
   // cout<<" EdgeList->ListSize is ="<< EdgeList->ListSize<<endl;
    memset(EdgeList->EdgePiont, 0, n * sizeof(CvPoint));
    memcpy(EdgeList->EdgePiont, pEdgePiont, n * sizeof(CvPoint));
    }//end if
    /* Free buffers */
    free(pEdgePiont);
    free(pBufMag);
    free(pBufOrien);
    free(pBufGradY);
    free(pBufGradX);
    free(pBufOut);
    free(pInput);
}

void DrawContours(Mat source, CvPoint* Contours, int ContoursSize, CvScalar color, int lineWidth)
{
	CvPoint point;
	int x = source.cols / 2;
	int y = source.rows / 2;
	//line(source, CvPoint(x , y-5), CvPoint(x, y+5),CV_RGB( 0, 0, 255 ),lineWidth);
	//line(source, CvPoint(x-5 , y), CvPoint(x+5, y),CV_RGB( 0, 0, 255 ),lineWidth);
	for (int i = 0; i < ContoursSize; i++)
	{
		point.x=Contours[i].x ;
		point.y=Contours[i].y ;
		cout<<" point.x"<<point.x<<" "<<point.y<<endl;
		circle(source, point, 0.1, color);
		//line(source,point,point,color,lineWidth);
	}
	
}

int main() {
    Mat source = imread("0612m1.bmp", 1); //加载一个图片到Mat中
    Mat temp(source.rows, source.cols, CV_8UC1); //根据原有的一个Mat创建一个新的单通道的Mat
    cvtColor(source, temp, COLOR_BGR2GRAY);//将原来的彩色图片转化成灰色图片
   
    edge_list EdgeList;
    Mat dilate_img,canny_img;
     EdgeList.EdgePiont = (CvPoint *) malloc(source.cols * temp.rows * sizeof(CvPoint));
   // Canny(temp, canny_img, 50, 200);
    train_shape_model(temp, 120,30, 1, &EdgeList);
    DrawContours(source, EdgeList.EdgePiont, EdgeList.ListSize , CV_RGB( 255, 0, 0 ),1);
 cout<<" EdgeList->ListSize is ="<< EdgeList.ListSize<<endl;
    //imshow("kk", dilate_img);
 //   imshow("yy", canny_img);
     imshow("imageROI", source);
    free(EdgeList.EdgePiont);
    waitKey(0);//等待5秒，5秒后自动销毁显示窗口，结束主程序

    return 0;
}
