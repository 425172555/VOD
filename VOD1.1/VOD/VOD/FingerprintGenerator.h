#ifndef FINGERPRINTGENERATOR_INCLUDE_H
#define FINGERPRINTGENERATOR_INCLUDE_H

#include <vector>
#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "imgproc/imgproc.hpp"
#include "opencv2\opencv.hpp"  
using namespace std;
using namespace cv;

//FingerprintGenerator类用于对图像提取视频指纹
class FingerprintGenerator
{
public:
	//构造函数
	FingerprintGenerator();
	//GenerateFingerprint用于提取传入的图像src的视频指纹，生成的视频指纹为11Byte长度，传入的图像src若设置了ROI，
	//则只计算ROI区域的视频指纹
	std::vector<char> GenerateFingerprint(IplImage *src, char *FingerPosition_confirm);

private:
	struct
	{
		/*****用来生成指纹的图像块的划分****/
		int ColNum;
		int RowNum;
		int ColStep;
		int RowStep;
		/*****用来生成指纹的图像的大小****/
		int ImageResizeWidth;
		int ImageResizeHeight;

		int Imagepooling;
	}ImgParameter;   //VideoFingerParameter

	int FINGER_MASK_BYTE;
	//int FINGER_MASK_BYTE2;
	//int FINGER_MASK_BYTE3;

	void GenerateVideoFinger(IplImage *src, char *FingerOutput, char *FingerPosition_confirm);
	void GenerateVideoFinger2(Mat &src, char *FingerOutput);
	void EightFingerCount(CvMat *ImgIntegral, char *FingerOutput, int rCenter, int cCenter);
	void FourFingerCount(CvMat *ImgIntegral, char *FingerOutput, int rCenter, int cCenter);
	void AddFingerPosition(char *FingerOutput, char *FingerPosition_confirm);
	int TranslateFingerprint(std::vector<char>::iterator start, std::vector<char>::iterator end, const char *fingerOutput);
public:
	Mat pooling(Mat img, int overlap);
};

#endif