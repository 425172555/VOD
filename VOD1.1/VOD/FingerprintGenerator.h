#ifndef FINGERPRINTGENERATOR_INCLUDE_H
#define FINGERPRINTGENERATOR_INCLUDE_H

#include <vector>
#include <opencv\cv.h>
#include <opencv\highgui.h>
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
	std::vector<char> GenerateFingerprint_7(IplImage *src, char *FingerPosition_confirm);
	std::vector<char> GenerateFingerprint_5(IplImage *src, char *FingerPosition_confirm);


private:
	struct
	{
		/*****用来生成7*7指纹的图像块的划分****/
		int ColNum_7;
		int RowNum_7;
		int ColStep_7;
		int RowStep_7;
		/*****用来生成7*7指纹的图像的大小****/
		int ImageResizeWidth_7;
		int ImageResizeHeight_7;

		/*****用来生成5*5指纹的图像块的划分****/
		int ColNum_5;
		int RowNum_5;
		int ColStep_5;
		int RowStep_5;
		/*****用来生成7*7指纹的图像的大小****/
		int ImageResizeWidth_5;
		int ImageResizeHeight_5;
	}ImgParameter;   //VideoFingerParameter

	int FINGER_MASK_BYTE_5;
	int FINGER_MASK_BYTE_7;
	//int FINGER_MASK_BYTE2;
	//int FINGER_MASK_BYTE3;

	void GenerateVideoFinger_7(IplImage *src, char *FingerOutput, char *FingerPosition_confirm);
	void GenerateVideoFinger_5(IplImage *src, char *FingerOutput, char *FingerPosition_confirm);
	
	void GenerateVideoFinger2(Mat &src, char *FingerOutput);

	void OneFingerCount(CvMat * ImgIntegral, char * FingerOutput, int rCenter, int cCenter, int position);
	void SevenFingerCount(CvMat *ImgIntegral, char *FingerOutput, int rCenter, int cCenter);
	void EightFingerCount(CvMat *ImgIntegral, char *FingerOutput, int rCenter, int cCenter,int mask);
	void FourFingerCount(CvMat *ImgIntegral, char *FingerOutput, int rCenter, int cCenter);
	void AddFingerPosition(char *FingerOutput, char *FingerPosition_confirm);
	int TranslateFingerprint(std::vector<char>::iterator start, std::vector<char>::iterator end, const char *fingerOutput);

};

#endif