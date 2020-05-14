//https://blog.csdn.net/YunlinWang/article/details/78147026
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

using namespace std;
using namespace cv;
//生成[0,1]之间符合均匀分布的数
double uniformRandom(void)
{
	//cout<<"(double)rand() / (double)RAND_MAX "<<(double)rand() / (double)RAND_MAX<<endl;
	return (double)rand() / (double)RAND_MAX;
}
/*https://www.cnblogs.com/tsingke/p/6194737.html
要得到服从正太分布的随机数：
基本思路：先得到服从均匀分布的随机数；然后再将服从均匀分布的随机数变为服从正太分布
如果在(0,1]值域内有两个一致的随机数字u1,u2
可以使用下面等式中的一个算出一个正态分布的随机数字z
Z = R * cos( θ ) 或 Z = R * sin( θ )
其中， R = sqrt(-2 * ln(U2)), θ = 2 * π * U1,
正太值z有一个等于0的平均值和一个等于１的标准偏差，可以使用下面的等式将z映射到一个平均值为m，标准偏差为sd的统计量x
x =m+(z*sd)
*/
//生成[0,1]之间符合高斯分布的数
double gaussianRandom(void)
{
    static int next_gaussian = 0;
	static double saved_gaussian_value;
	double fac,rsq,v1,v2;
	
	if(next_gaussian==0){
		do {
            v1 = 2 * uniformRandom() - 1;
            v2 = 2 * uniformRandom() - 1;
            rsq = v1*v1 + v2*v2;
        } while (rsq >= 1.0 || rsq == 0.0);
		fac = sqrt(-2*log(rsq)/rsq);
		saved_gaussian_value = v1*fac;
		next_gaussian = 1;
		return v2*fac;
	}else{
		next_gaussian = 0;
		return saved_gaussian_value;
	}
}
//得到直线拟合样本，即在直线采样点集上随机选2个点
bool getSample(vector<int> set,vector<int> &sset)
{
	//cout<<"set size is:"<<set.size()<<endl;
    int i[2];
    if(set.size() > 2)
	{
		do{
			for(int n=0;n<2;n++){
				//在30个的样本中选取２个，uniformRandom返回值是0-1之间，保证计算的结果在0-30的样本中
				i[n] = int(uniformRandom() * (set.size() - 1));
			}
		}while(!(i[1] != i[0]));//两个样本不相同就结束
		for(int n=0;n<2;n++)
		{
			sset.push_back(i[n]);//样本序号保存
		}
	}else{
		return false;
	}
	
	return true;
}

//直线样本中两个随机点的位置不能太近
bool verifyComposition(const vector<Point2d> pts)
{
    cout<<"pts :"<<pts[0]<<" "<<pts[1]<<endl;
	if(abs(pts[0].x - pts[1].x) < 5 && abs(pts[0].y - pts[1].y) < 5)
		return false;
	return true;
}
/*
firLine 参数　输入点集　可以是二维点Mat数组，也可以是二维点vector
第二个参数是输出直线，对于二维直线的类型是vec4f,输出参数的前半部分是给出的是直线方向，后半部分是直线上的点(点斜式)
第三个参数是距离类型，拟合直线时候，要使得输入点到拟合直线的距离的最小化，下面是输入点到直线的距离
cv_dist_l2 = 2
 0 1e-2 1e-2
 float *line = new float[4];
 二维空间时候:line[0-3] 分别为(vx,vy,x0,y0) vx,vy是正规划后的斜率向量
 x0,y0是直线经过的点
 http://blog.sina.com.cn/s/blog_a5b3ed560101bvi8.html
*/
//根据点集拟合直线ax+by+c=0,res为残差
void calcLinePara(vector<Point2d> pts,double &a,double &b,double &c,double &res)
{
    res = 0;
	vector<Point2f> ptsF;
	Vec4f line;
	for(unsigned int i=0;i< pts.size();i++)
		ptsF.push_back(pts[i]);
	//cout<<"ptsF:"<<ptsF[0]<<" "<<ptsF[1]<<endl;
	fitLine(ptsF,line,CV_DIST_L2,0,1e-2,1e-2);
	a=line[1];//vy
	b=-line[0];//vx line[3] x0 
	c = line[0] * line[3] - line[1] * line[2];
	cout<<"a,b,c"<<a<<","<<b<<","<<c<<endl;
	for(unsigned int i=0;i<pts.size();i++)
	{
		double resid_ = fabs(pts[i].x * a + pts[i].y*b + c);
		res += resid_;
	}
	
	res /= pts.size();
}

//ransac直线拟合
void fitLineRANSAC(vector<Point2d> ptSet,double &a,double &b,double &c,vector<bool> &inlierFlag)
{
    double residual_error = 2.99;//内点阈值
	bool stop_loop = false;
	int maximum = 0;//最大内点数
	int N=500;
	int sample_count = 0;
	
	//最终的内点标志与其残差
	vector<double> resids_(ptSet.size(),3);
	
	double res = 0;
	
	vector<int> ptsID;
	for(unsigned int i=0;i<ptSet.size();i++)
		ptsID.push_back(i);
	while(N>sample_count && !stop_loop)
	{
		vector<int> ptss;
		vector<bool> inlierstemp;//每个点是否为内点
		vector<double> residualstemp;//存放每个点到拟合直线的距离
	    int inlier_count = 0;//记录符合的点个数
		if(!getSample(ptsID,ptss))//得到直线拟合样本，即在直线采样点集上随机选2个点
		{
			stop_loop = true;
			continue;
		}
		
		vector<Point2d> pt_sam;
		pt_sam.push_back(ptSet[ptss[0]]);
		pt_sam.push_back(ptSet[ptss[1]]);
		
		//cout<<"pt_sam"<< pt_sam[0]<<pt_sam[1]<<" "<<pt_sam.size()<<endl;
		//选取的两点不能太近
		if(!verifyComposition(pt_sam))
	    {
			cout<<"选取的点距离太近!\n"<<endl;
		    ++sample_count;
			continue;
		}
	
		//计算直线方程
	    calcLinePara(pt_sam,a,b,c,res);
		//内点检验
		for(unsigned int i=0;i<ptSet.size();i++)
		{
			//cout<<"ptSet  "<<ptSet[i]<<endl;
			//Point2d pt = ptSet[i];
			double resid_ = fabs(ptSet[i].x*a + ptSet[i].y*b + c);
			//cout<<"resid_::"<<resid_<<endl;
			residualstemp.push_back(resid_);
			inlierstemp.push_back(false);//初始都为false
			if(resid_ < residual_error)
			{
				++inlier_count;
				inlierstemp[i] = true;//符合改变
			}
		}
		//找到最佳拟合直线
		if(inlier_count >=  maximum)
		{
			maximum = inlier_count;//inlier_count记录每次内点的数量
			resids_ = residualstemp;
			inlierFlag = inlierstemp;//记录内点的序号
		}
		  cout<<"inlier_count:"<<inlier_count<<endl;
	    //更新ransac迭代次数，以及内点概率
		if(inlier_count == 0){
			N = 500;
		}else{
			//野值点比例
			double epsilon = 1.0 - (double)inlier_count / (double)ptSet.size();
			double p = 0.99;//所有样本中存在１个好样本的概率
			double s = 2.0;
			/*
			k=log(1-z) / log(1-w^n) ,k为需要采样的次数，z为获取一个好样本的概率
			w为内点比例,ｎ为模型参数，需要最小点个数，２
			Ｎ表示采样Ｎ次中可以包含好的样本
			*/
            N = int(log(1.0 - p) / log(1.0 - pow((1.0 - epsilon), s)));//迭代次数
			//cout<<"N:"<<N<<endl;
		}
		++sample_count;
		//cout<<"sample_count:"<<sample_count<<endl;
	}//end for while
	
	//利用所有的内点重新拟合直线
	vector<Point2d> pSet;//将内点集点取出
	for(unsigned int i = 0;i < ptSet.size();i++)
	{
		if(inlierFlag[i])
			pSet.push_back(ptSet[i]);	
	}
	//重新拟合直线
    calcLinePara(pSet,a,b,c,res);
	
}
int main()
{
        
        int width = 640;
	int height = 320;
	//直线参数
	double a=1,b=2,c=-640;
	//随机获取直线上的20个点
	int ninliers = 0;
	vector<Point2d> ptSet;
	srand((unsigned int)time(NULL));//设置随机数种子
	while(true){
		double x = uniformRandom()*(width-1);
		double y = -(a*x+c)/b;
		//加入0.5高斯噪声
		x += gaussianRandom()*0.5;
		y += gaussianRandom()*0.5;
		//cout<<"x,y:"<<x<<","<<y<<endl;
		if(x >= 640 && y >= 320)
			continue;
		Point2d pt(x,y);
		cout<<"pt:"<<pt<<endl;
		ptSet.push_back(pt);
		ninliers++;
		if(ninliers == 20)
			break;
	}
	
	int nOutliers = 0;
	//随机获取10个野值点
	while(true)
	{
		double x = uniformRandom()*(width - 1);
		double y = uniformRandom()*(height - 1);
		
		if(fabs(a*x+b*y+c) <10)//野值点到直线距离不小于１０个像素
			continue;
		
		Point2d pt(x,y);
		ptSet.push_back(pt);
		nOutliers++;
		if(nOutliers==10)
			break;
	}
	
	//绘制点集中所有的点
	Mat img(321,641,CV_8UC3,Scalar(255,255,255));
	for(unsigned int i=0;i<ptSet.size();i++)
		circle(img,ptSet[i],3,Scalar(255,0,0),3,8);
	
	
	double A,B,C;
	vector<bool> inliers;
	fitLineRANSAC(ptSet,A,B,C,inliers);
	
	B = B / A;
	C = C / A;
	A = A / A;
	//ax+by+c=0 y= -(ax+x)/b    x=-(by+c)/a
	Point2d ptStart,ptEnd;
	ptStart.x = 0;
	ptStart.y = -(A*ptStart.x + C) / B;
	ptEnd.x = -(B*ptEnd.y + C) / A;
	ptEnd.y = 0;
	line(img,ptStart,ptEnd,Scalar(0,255,255),2,8);
	cout << "A:" << A << " " << "B:" << B << " " << "C:" << C << " " << endl;
	imshow("img",img);
	waitKey(0);
	
	
	
	
	
	
	
	
	
}
