#ifndef GET_BMP_DATA
#define GET_BMP_DATA

#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "opencv2/opencv.hpp"

//��BYTE *lpBmpData�л�ȡ��ͼ������
int Get_BMP_data(IplImage *pImg, IplImage *pImg_temp, IplImage *pImgBlackDecide, BYTE *lpBmpData, RECT &captureRect);

// �����Ҷ�ͼ���ֱ��ͼ
int ImageBlackDecide(IplImage **ppImage);

#endif