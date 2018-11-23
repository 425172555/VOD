#ifndef GET_BMP_DATA
#define GET_BMP_DATA

#include <opencv\cv.h>
#include <opencv\highgui.h>
#include "opencv2/opencv.hpp"

//从BYTE *lpBmpData中获取到图像数据
int Get_BMP_data(IplImage *pImg, IplImage *pImg_temp, IplImage *pImgBlackDecide, BYTE *lpBmpData, RECT &captureRect);

// 创建灰度图像的直方图
int ImageBlackDecide(IplImage **ppImage);

#endif