#ifndef FINGERPRINTGENERATOR_INCLUDE_H
#define FINGERPRINTGENERATOR_INCLUDE_H

#include <vector>
#include <opencv\cv.h>
#include <opencv\highgui.h>

#include "imgproc/imgproc.hpp"
#include "opencv2\opencv.hpp"  
using namespace std;
using namespace cv;

//FingerprintGenerator�����ڶ�ͼ����ȡ��Ƶָ��
class FingerprintGenerator
{
public:
	//���캯��
	FingerprintGenerator();
	//GenerateFingerprint������ȡ�����ͼ��src����Ƶָ�ƣ����ɵ���Ƶָ��Ϊ11Byte���ȣ������ͼ��src��������ROI��
	//��ֻ����ROI�������Ƶָ��
	std::vector<char> GenerateFingerprint(IplImage *src, char *FingerPosition_confirm);

private:
	struct
	{
		/*****��������ָ�Ƶ�ͼ���Ļ���****/
		int ColNum;
		int RowNum;
		int ColStep;
		int RowStep;
		/*****��������ָ�Ƶ�ͼ��Ĵ�С****/
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