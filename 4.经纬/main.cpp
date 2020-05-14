//������ͷ�ļ�
#include "tools.h"
#include "findCircleParameter.h"
#include "corrector.h"
#include "imagesStitcher.h"
#include "viewer.h"

bool isDispCorrectRet = false;
bool saveResult = false;

static void help()
{
	printf("This is a navigator based on fisheye images.\n"
		"Usage: navigator\n"
		"     [-d]          # display the corrected fisheye images.\n"
		"     [-s]          # save the result of image process into files.\n"
		"     [-h]          # show the help information.\n"
		"\n");

}
void paserParams(int argc, char**argv)
{
	if (argc > 1)
	{
		for (int i = 1;i < argc;i++)
		{
			if (strcmp(argv[i], "-d")==0)
				isDispCorrectRet = true;
			else if (strcmp(argv[i], "-s")==0)
				saveResult = true;
			else if (strcmp(argv[i], "-h") == 0)
			{
				help();
				exit(0);
			}
		}
	}
}

//���̶���ڵ�
int main(int argc, char** argv)
{
	paserParams(argc, argv);

	correctParameters params;
	corrector adjuster;
	vector<Mat> outputs;
	imagesStitcher stitcher;

	//��ͼƬ���ڴ���
	if (tools::readImage())
	{
		vector<Mat>& inputs = tools::GetImages();

		Mat source_image = inputs[0];

		if (findCircleParameter::init(source_image))
		{
			findCircleParameter::findCircle();

#pragma region У�������趨��
			params.imgOrg = source_image;
			findCircleParameter::getCircleParatemer(params.center, params.radius);
			params.w_longtitude = PI / 2;
			params.w_latitude = PI / 2;
			params.distMap = LATITUDE_LONGTITUDE;
			params.theta_left = 0;
			params.phi_up = 0;
			params.camerFieldAngle = findCircleParameter::FOV;
			params.camProjMode = EQUIDISTANCE;
			params.typeOfCorrect = Reverse;
#pragma endregion								

			cout << endl
				<< "Correct Image(s) with the same circular region params: " << endl
				<< "radius = " << params.radius << ", center = " << params.center << endl << endl;

#pragma region ͼ��У����
			corrector::correctMethod method = corrector::correctMethod::PERSPECTIVE_LONG_LAT_MAP_CAMERA_LEN_MODEL_REVERSE_W_HALF_PI;

			outputs.push_back(
				adjuster.correctImage(params, method,
					isDispCorrectRet)
				);

			for (int i = 1;i < inputs.size();i++)
			{
				source_image = inputs[i];
				params.imgOrg = source_image;

				outputs.push_back(
					adjuster.correctImage(params, method, isDispCorrectRet)
					);
			}
		}
	}
#pragma endregion

#pragma region ͼ��ӽӲ���
	vector<Mat> images1, images2;
	string filenamePath;
	char suffix[MAX_PATH];
	for (int i = 0;i < outputs.size();i++)
	{
		if (saveResult)
		{
			sprintf_s(suffix, "corrected%d.jpg", i);
			string tmp(suffix);
			filenamePath = tmp;
			imwrite(filenamePath, outputs[i]);
		}
		if (i < outputs.size() - 1)
			images1.push_back(outputs[i]);
		else
			images2.push_back(outputs[i]);
	}

	if (outputs.size() > 1)
	{
		cout << endl << "Start Stitch images" << endl;

		Mat tmpRet = stitcher.stitchImages(images1);
		images2.insert(images2.begin(), tmpRet.clone());
		stitcher.stitchImages(images2);
		stitcher.showPanorama();

		cout << "Stitch Images completes!" << endl;
#pragma endregion
#pragma region ���ȫ��ͼ��

		Mat panoramaImage = stitcher.getPanorama();

		if (!panoramaImage.empty())
		{
			if (saveResult)	imwrite("panorama.jpg", panoramaImage);

			viewer panoViewer(panoramaImage);
			panoViewer.showWindow();
		}
#pragma endregion
	}
	return 0;
}